#include "stdio.h" 
#include "cpm_sysfunc.h"

#include "common_datatypes.h"
#include "cpmbdos.h"

#include <stdlib.h>
#include <string.h>

#define READLINE_BUF_SIZE 80


typedef struct {
	uint8_t size;
	uint8_t len;
	char bytes[80];
} RS_BUFFER;

static BDOSCALL bdos_readstr;
static RS_BUFFER rs_buf;

static uint16_t ret_ba, ret_hl;
extern int rtargc;
extern uint16_t heapaddr;

uint16_t get_ret_ba()
{
    return ret_ba;
}

uint16_t get_ret_hl()
{
    return ret_hl;
}

uint16_t get_stktop()
{
		return heapaddr;

}

void _cpm_sysfunc_init() {
    /* standard CP/M command buffer at 0x0080 */
    /* this function returns the correct value for argc */
    /* FIXME: this is such a hack right now - we save the argv pointers in the "UNUSED" section at 0x0050*/
    char *argptr = (char *) 0x0080;
    unsigned char _argbytes;
    int i = 0;
    bool within_arg = false;
    int _argc = 1;
    uint16_t *argv_ptr = (uint16_t*) 0x0052;    /* first word is argv0, which we don't have, 
                                    but we'll fill it with NULL anyway */
    /* this is apparently unused in CP/M 2.2 so we will use it as pointers for argv */
    memset((void *) 0x50, 0, 0x10);


    //printf("initial argc = %d\n", *argc);
    _argbytes=argptr[0];
    
//    printf("do arguments! %u bytes [%s]\n", _argbytes, argptr+1);	
    for (i = 1; i < (_argbytes+2 ); i++) {
            switch(argptr[i]) {
            case '\0':
                if (within_arg) {
  //                  printf("X> [%c] %d 0x%04x [%s]\n", argptr[i - 1],_argc, _myargv[_argc], _myargv[_argc]);
                		_argc++;
                    } 
                /* end of argument string */
                within_arg = false;
                if (_argc >= MAX_ARGS) {
                    goto args_done;
                    }
                break;
            case ' ':
                argptr[i] = '\0';
                if (within_arg) {
    //                printf("+> [%d] 0x%04x [%s]\n", _argc, _myargv[_argc], _myargv[_argc]);
                    _argc++;
                    } 
                if (_argc >= MAX_ARGS) {
                    goto args_done;
                    }
                within_arg = false;
                break;
            default:
                if (!within_arg) {
                    //_myargv[_argc] = (argptr+i);
                    /* very hacky */
                    argv_ptr[0] = 0x80+i; 
                    argv_ptr[1] = 0x00;
                    argv_ptr++;
      //              printf("-> [%c] storing argv[%d] = 0x%04x\n", argptr[i], _argc, _myargv[_argc]);
                    within_arg = true;
                    } else {
        //            printf("-> <%c>\n", argptr[i]);
                    }
                break;
           }
    }

	// Initialize READSTR BDOS call
	bdos_readstr.func8 = C_READSTR;
	bdos_readstr.parm16 = (uint16_t)&rs_buf;

args_done:
    rtargc = (int )_argc;
}

char *cpm_gets(char *p) {
//    cprintf("cpm_gets(rs_buf.bytes = 0x%04x [%u], p=0x%04x)\n", &rs_buf.bytes, sizeof(rs_buf.bytes), p);
	memset(rs_buf.bytes, 0, sizeof(rs_buf.bytes));
	rs_buf.size = sizeof(rs_buf.bytes);
	rs_buf.len = 0;

	cpmbdos_extn(&bdos_readstr, &ret_ba, &ret_hl);

//	rs_buf.bytes[rs_buf.len] = '\n';
	strcpy(p, rs_buf.bytes);
	return p;
}


char cpm_getchar(void) {
    char f = 0;
	BDOSCALL cread = { C_READ, { (uint16_t)0 } };
	f = cpmbdos_extn(&cread, &ret_ba, &ret_hl);
    return f;
}

char cpm_getchar_noecho(void) {
    char f = 0;
	BDOSCALL cread = { C_RAWIO, { (uint16_t) 0x00ff } };
	f = cpmbdos_extn(&cread, &ret_ba, &ret_hl);
    return f;
}


void cpm_putchar(char c) {
	BDOSCALL cwrite = { C_WRITE, { (uint16_t)c } };
	cpmbdos_extn(&cwrite, &ret_ba, &ret_hl);
}

void cpm_setDMAAddr(uint16_t addr) {
	BDOSCALL fdma = { F_DMAOFF, {addr} };

	cpmbdos_extn(&fdma, &ret_ba, &ret_hl);
}

uint8_t cpm_getCurDrive(void) {
	BDOSCALL drv = { DRV_GET, { 0 } };

	return cpmbdos_extn(&drv, &ret_ba, &ret_hl);	
}

uint8_t cpm_resetDrives(void) {
	BDOSCALL drv = { DRV_ALLRESET, { 0 } };

	return cpmbdos_extn(&drv, &ret_ba, &ret_hl);	
}

uint8_t cpm_setCurDrive(uint8_t drive) {
	BDOSCALL drv = { DRV_SET, { drive } };

	return cpmbdos_extn(&drv, &ret_ba, &ret_hl);
}

void cpm_setFCBname(char *fname, char *ftype, FCB *cb) {
	int idx;
	char c;

	for (idx = 0; (idx < 8) && (fname[idx] != '\0'); idx++) {
		c = fname[idx] & 0x7F;
		if (c >= 0x61 && c <= 0x7a)
			c -= 0x20; 

		cb->filename[idx] = c;
	}

	while (idx < 8) {
		cb->filename[idx] = ' '; // Pad the filename
		idx++;
	}
	
	for (idx = 0; (idx < 3) && (ftype[idx] != '\0'); idx++) {
		c = ftype[idx] & 0x7F;
		if (c >= 0x61 && c <= 0x7a)
			c -= 0x20; 
		
		cb->filetype[idx] = c;
	}

	while (idx < 3) {
		cb->filetype[idx] = ' '; // Pad the filetype
		idx++;
	}
}

uint8_t cpm_performFileOp(FileOperation fop, FCB *cb) {
	BDOSCALL call = { 0, {(uint16_t)cb} };

	switch (fop) {
		case fop_open:
			call.func8 = F_OPEN;
			break;
		case fop_close:
			call.func8 = F_CLOSE;
			break;
		case fop_firstNameMatch:
			call.func8 = F_SMATCH;
			break;
		case fop_nextMatch:
			call.func8 = F_NMATCH;
			break;
		case fop_makeFile:
			call.func8 = F_MAKE;
			break;
		case fop_delFile:
			call.func8 = F_DELETE;
			break;
		case fop_setFileAttr:
			call.func8 = F_ATTRIB;
			break;
		case fop_readSeqRecord:
			call.func8 = F_READ;
			break;
		case fop_writeSeqRecord:
			call.func8 = F_WRITE;
			break;
		case fop_readRandRecord:
			call.func8 = F_READRAND;
			break;
		case fop_writeRandRecord:
			call.func8 = F_WRITERAND;
			break;
		case fop_updRandRecPtr:
			call.func8 = F_RANDREC;
			break;
		case fop_calcFileSize:
		default:
			call.func8 = F_SIZE;
			break;
	}

	return cpmbdos_extn(&call, &ret_ba, &ret_hl);
	//return cpmbdos(&call);
}

void cpm_reset(void) {
	BDOSCALL sreset = { S_RESET, { (uint16_t)0 } };
	cpmbdos_extn(&sreset, &ret_ba, &ret_hl);
}

bool
continue_or_exit ()
{

    unsigned char chr[READLINE_BUF_SIZE];
    printf ("  More? [Y] continue [N] exit: ");
    memset (&chr, 0, READLINE_BUF_SIZE);
    cpm_gets (chr);
    return ((chr[0] == 'Y' || chr[0] == 'y') ? true : false);
}



