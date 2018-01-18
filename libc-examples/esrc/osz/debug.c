#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "syslib/cpm_sysfunc.h"

#define false   0
#define true    1


extern char DFS;
#define ISN_LOAD    "LD"

char output[64];

extern bool has_hydrogen;

char *
RTTop(int index)
{
    switch(index) {
        case 0:
            return "ADD A,";
            break;
        case 1:
            return "SUB A,";
            break;
        case 2:
            return "AND ";    
            break;
        case 3:
            return "OR ";    
            break;
            }
        return "*UNKNOWN!";
}

char *
RTTw(int index)
{
    switch(index) {
        case 0:
            return "RLCA";
            break;
        case 1:
            return "RRCA";
            break;
        case 2:
            return "RLA";
            break;
        case 3:
            return "RRA";
            break;
        case 4:
            return "DAA";
            break;
        case 5:
            return "CPL";
            break;
        case 6:
            return "SCF";
            break;
        case 7:
            return "CCF";
            break;
        }
    return "UNKNOWN!";

}

char * 
RTTf(int index)
{
    switch(index) {
        case 0:
            return "NZ";
            break;
        case 1:
            return " Z";
            break;
        case 2:
            return "NC";
            break;
        case 3:
            return " C";
            break;
        case 4:
            return "PO";
            break;
        case 5:
            return "PE";
            break;
        case 6:
            return " P";
            break;
        case 7:
            return " M";
            break;
        } 
    return "UNKNOWN";
}

char *
RTT (int index)
{

    switch (index)
    {
    case 0x00:
    case 0x08:
        return " B";
        break;
    case 0x01:
    case 0x09:
        return " C";
        break;
    case 0x02:
    case 0x0A:
        return " D";
        break;
    case 0x03:
    case 0x0B:
        return " E";
        break;
    case 0x04:
    case 0x0C:
        return " H";
        break;
    case 0x05:
    case 0x0D:
        return " L";
        break;
    case 0x06:
    case 0x0E:
        return "(HL)";
        break;
    case 0x07:
    case 0x0F:
        return " A";
        break;
    }
    printf ("i=%u\n", index);
    return "UNKNOWN";
}

int
debug (char *payload)
{
    unsigned int addr = 0;
    int i = 0;
    int lines = 0;
    char *ptr = NULL;
    bool disassemble = false;
    char instruction = 0;
    unsigned char *memptr = NULL;
    char *insn_text = NULL;
    int offset = 0;

    printf ("\n");

    if (strncmp (payload, "0x", 2) == 0)
    {
        addr = strtoul (payload, NULL, 16);
    }
    else
    {
        addr = strtoul (payload, NULL, 10);
    }

    disassemble = true;
    while (disassemble && lines < 23)
    {
        printf ("0x%04x %c ", addr, DFS);
        memptr = (unsigned char *) addr;
        memset (&output, 0, 64);

        /* default to instruction length of 1 */

        offset = 1;

        /* pass 1 - odd ones out */

        switch (memptr[0])
        {
        case 0x00:
            sprintf ((char *) &output, "NOP");
            goto emit;
            break;
        case 0x76:
            sprintf ((char *) &output, "HALT");
            goto emit;
            break;
        default:
            /* fall through to pass 2 */
            break;
        }


        /* pass 1A - INCS and DECS */

        if (memptr[0] < 0x40)
        {
            int regindex = 0;
            switch (memptr[0] & 0x0F)
            {
            /* switch on lower nibble */
            /* INCREMENT */
            case 0x04:
                regindex = ((memptr[0] & 0xF0) / 0x10) * 2;
                sprintf ((char *) &output, "INC %s", RTT (regindex)); 
                goto emit;
                break;
            case 0x06:
                /* LOAD BY REGINDEX */
                regindex = ((memptr[0] & 0xF0) / 0x10) * 2;
                sprintf ((char *) &output, "LD %s,&%02X", RTT (regindex), memptr[1]);
                offset = 2;
                goto emit;
                break;
            case 0x07:
                regindex = (((memptr[0] & 0xF0) / 0x10) * 2);
                sprintf ((char *) &output, "%s", RTTw (regindex));
                goto emit;
                break;
                
            /* INCREMENT */
            case 0x0C:
                regindex = ((memptr[0] & 0xF0) / 0x10) * 2;
                sprintf ((char *) &output, "INC %s", RTT (regindex + 1));
                goto emit;
                break;
            /* DECREMENT */
            case 0x05:
                regindex = ((memptr[0] & 0xF0) / 0x10) * 2;
                sprintf ((char *) &output, "DEC %s", RTT (regindex));
                goto emit;
                break;
            /* DECREMENT */
            case 0x0D:
                regindex = ((memptr[0] & 0xF0) / 0x10) * 2;
                sprintf ((char *) &output, "DEC %s", RTT (regindex + 1));
                goto emit;
                break;
            case 0x0E:
                /* LOAD BY REGINDEX+1 */
                regindex = ((memptr[0] & 0xF0) / 0x10) * 2;
                sprintf ((char *) &output, "LD %s,&%02X",
                         RTT (regindex + 1), memptr[1]);
                offset = 2;
                goto emit;
                break;
            case 0x0F:
                regindex = (((memptr[0] & 0xF0) / 0x10) * 2)+1;
                sprintf ((char *) &output, "%s", RTTw (regindex));
                goto emit;
                break;
            default:
                /* fall through to next pass */
                break;
            }
        }

        /* PASS1B - values over 0xC0 */

        /* pass 1A - INCS and DECS */

        if (memptr[0] >= 0xC0)
        {
            int regindex = 0;

            switch (memptr[0] & 0x0F) {
                    case 0x0:
                        regindex = ((((memptr[0] & 0xF0) - 0xC0) / 0x10) * 2);
                        sprintf ((char *) &output, "*RET %s", RTTf(regindex));
                        disassemble = false;
                        goto emit;
                        break;
                    case 0x06:
                        regindex = ((((memptr[0] & 0xF0) - 0xC0) / 0x10));
                        sprintf ((char *) &output, "*%s%02X", RTTop(regindex), memptr[1]);
                        offset = 2;
                        goto emit;
                        break;
                    case 0x08:
                        regindex = ((((memptr[0] & 0xF0) - 0xC0) / 0x10) * 2)+1;
                        sprintf ((char *) &output, "*RET %s", RTTf(regindex));
                        disassemble = false;
                        goto emit;
                        break;
                    }

            regindex = 0;

            /* these are a bit weird and inconsistent, so i'm special casing them for now */
            switch (memptr[0])
            {
            /* switch on lower nibble */
            case 0xC2:
                sprintf ((char *) &output, "JP   NZ,&%02X%02X", memptr[2],
                         memptr[1]);
                offset = 3;
                goto emit;
                break;
            case 0xC4:
                sprintf ((char *) &output, "CALL NZ,&%02X%02X", memptr[2],
                         memptr[1]);
                offset = 3;
                goto emit;
                break;
            case 0xD4:
                sprintf ((char *) &output, "CALL NC,&%02X%02X", memptr[2],
                         memptr[1]);
                offset = 3;
                goto emit;
                break;
            case 0xE4:
                sprintf ((char *) &output, "CALL PO,&%02X%02X", memptr[2],
                         memptr[1]);
                offset = 3;
                goto emit;
                break;
            case 0xF4:
                sprintf ((char *) &output, "CALL P,&%02X%02X", memptr[2],
                         memptr[1]);
                offset = 3;
                goto emit;
                break;
            }
        }


        /* pass 2 - obvious patterns in the instruction matrix */

        switch (memptr[0] & 0xF0)
        {
        case 0x40:
            if (memptr[0] >= 0x48)
            {
                sprintf ((char *) &output, "LD C, %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            else
            {
                sprintf ((char *) &output, "LD B, %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            goto emit;
            break;
        case 0x50:
            if (memptr[0] > 0x58)
            {
                sprintf ((char *) &output, "LD E, %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            else
            {
                sprintf ((char *) &output, "LD D, %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            goto emit;
            break;
        case 0x60:
            if (memptr[0] > 0x68)
            {
                sprintf ((char *) &output, "LD L, %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            else
            {
                sprintf ((char *) &output, "LD HL, %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            goto emit;
            break;
        case 0x70:
            if (memptr[0] >= 0x78)
            {
                sprintf ((char *) &output, "LD A, %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            else
            {
                sprintf ((char *) &output, "LD (HL), %s",
                         RTT ((memptr[0] & 0x0F)));
            }
            goto emit;
            break;
        case 0xB0:
            if (memptr[0] >= 0xB8)
            {
                sprintf ((char *) &output, "CP %s", RTT ((memptr[0] & 0x0F)));
            }
            else
            {
                sprintf ((char *) &output, "OR %s", RTT ((memptr[0] & 0x0F)));
            }
            goto emit;
            break;
        default:
            /* fall through to pass 3 */
            break;
        }

        /*  - stuff that didn't get picked up by first pass */
        switch (memptr[0])
        {
        case 0x01:
            sprintf ((char *) &output, "LD BC,0x%02X%02X", (char) memptr[1],
                     (char) memptr[2]);
            offset = 3;
            break;
        case 0x0A:
            sprintf ((char *) &output, "LD A, (BC)");
            break;
        case 0x10:
            sprintf ((char *) &output, "DJNZ (PC+0x%02X%02X)", (char) memptr[1],
                     (char) memptr[2]);
            offset = 3;
            break;
        case 0x11:
            sprintf ((char *) &output, "LD DE,0x%02X%02X", (char) memptr[1],
                     (char) memptr[2]);
            offset = 3;
            break;
        case 0x12:
            sprintf ((char *) &output, "LD (DE),A");
            break;
        case 0x13:
            sprintf ((char *) &output, "INC DE");
            break;
        case 0x18:
            sprintf ((char *) &output, "JR (PC+0x%02X%02X)", (char) memptr[1],
                     (char) memptr[2]);
            offset = 3;
            break;
        case 0x19:
            sprintf ((char *) &output, "ADD HL,DE");
            break;
        case 0x1A:
            sprintf ((char *) &output, "LD A, (DE)");
            break;
        case 0x20:
            sprintf ((char *) &output, "JR NZ, (PC+0x%02X%02X)",
                     (char) memptr[2], (char) memptr[1]);
            offset = 3;
            break;
        case 0x21:
            sprintf ((char *) &output, "LD HL,0x%02X%02X", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0x22:
            sprintf ((char *) &output, "LD (%02X%02X), HL", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0x23:
            sprintf ((char *) &output, "INC HL");
            break;
        case 0x28:
            sprintf ((char *) &output, "JR Z, (PC+0x%02X%02X)",
                     (char) memptr[2], (char) memptr[1]);
            offset = 3;
            break;
        case 0x2A:
            sprintf ((char *) &output, "LD HL, (%02X%02X)", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0x30:
            sprintf ((char *) &output, "JR NC, (PC+0x%02X%02X)",
                     (char) memptr[2], (char) memptr[2]);
            offset = 3;
            break;
        case 0x31:
            sprintf ((char *) &output, "LD SP,0x%02X%02X", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0x32:
            sprintf ((char *) &output, "LD (%02X%02X), A", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0x39:
            sprintf ((char *) &output, "ADD HL, SP");
            break;
        case 0x3A:
            sprintf ((char *) &output, "LD A, (%02X%02X)", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0xAF:
            sprintf ((char *) &output, "XOR A");
            break;
        case 0xC1:
            sprintf ((char *) &output, "POP BC");
            break;
        case 0xC3:
            sprintf ((char *) &output, "JP 0x%02X%02X", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            /* this is an unconditional jump, so stop dissassembly here, because we're won't be executing the next 
               instruction directly after, ever*/
            disassemble = false;
            break;
        case 0xC5:
            sprintf ((char *) &output, "PUSH BC");
            break;
        case 0xCA:
            sprintf ((char *) &output, "JP Z, 0x%02X%02X", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0xCD:
            sprintf ((char *) &output, "CALL 0x%02X%02X", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0xC9:
            sprintf ((char *) &output, "RET");
            disassemble = false;
            break;
        case 0xCF:
            sprintf ((char *) &output, "RST 8H");
            break;
        case 0xD1:
            sprintf ((char *) &output, "POP DE");
            break;
        case 0xD3:
            sprintf ((char *) &output, "OUT 0x%02X, A", (char) memptr[1]);
            offset = 2;
            break;
        case 0xD5:
            sprintf ((char *) &output, "PUSH DE");
            break;
        case 0xD9:
            sprintf ((char *) &output, "EXX");
            break;
        case 0xDA:
            sprintf ((char *) &output, "JP C, (0x%02X%02X)", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0xDB:
            sprintf ((char *) &output, "IN 0x%02X, A", (char) memptr[1]);
            offset = 2;
            break;
        case 0xDD:
            /* DD prefixed opcode */
            switch (memptr[1])
            {
            case 0x21:
                sprintf ((char *) &output, "LD IX, 0x%02X%02X",
                         (char) memptr[2], (char) memptr[3]);
                offset = 4;
                break;
            case 0x39:
                sprintf ((char *) &output, "ADD IX, SP");
                offset = 2;
                break;
            case 0x66:
                sprintf ((char *) &output, "LD H, (IX+0x%02X)",
                         (char) memptr[2]);
                offset = 3;
                break;
            case 0x6E:
                sprintf ((char *) &output, "LD L, (IX+0x%02X)",
                         (char) memptr[2]);
                offset = 3;
                break;
            case 0xE1:
                sprintf ((char *) &output, "POP IX");
                offset = 2;
                break;
            case 0xE5:
                sprintf ((char *) &output, "PUSH IX");
                offset = 2;
                break;
            case 0xEB:
                sprintf ((char *) &output, "EX   DE,HL");
                offset = 1;
                break;
            default:
                sprintf ((char *) &output, "??? -> [%02X%02X]",
                         (char) memptr[0], (char) memptr[1]);
                offset = 2;
                disassemble = false;
                break;
            }
            break;
        case 0xE1:
            sprintf ((char *) &output, "POP HL ");
            break;
        case 0xE3:
            sprintf ((char *) &output, "EX (SP),HL ");
            break;
        case 0xE5:
            sprintf ((char *) &output, "PUSH HL");
            break;
        case 0xEB:
            sprintf ((char *) &output, "EX   DE,HL");
            break;
        case 0xED:
            switch (memptr[1])
            {
            case 0x73:
                sprintf ((char *) &output, "LD (%02X%02X), SP",
                         (char) memptr[2], (char) memptr[1]);
                offset = 3;
                break;
            case 0x7B:
                sprintf ((char *) &output, "LD SP, (%02X%02X)",
                         (char) memptr[2], (char) memptr[1]);
                offset = 3;
                break;
            /* ED prefixed opcode */
            default:
                sprintf ((char *) &output, "??? -> [%02X%02X]",
                         (char) memptr[0], (char) memptr[1]);
                offset = 2;
                disassemble = false;
                break;
            }
            break;
        case 0xEE:
            sprintf ((char *) &output, "XOR &%02x", memptr[1]);
            offset = 2;
            break;
        case 0xEF:
            sprintf ((char *) &output, "RST 28h");
            break;
        case 0xF1:
            sprintf ((char *) &output, "POP AF");
            break;
        case 0xF3:
            sprintf ((char *) &output, "DI");
            break;
        case 0xF9:
            sprintf ((char *) &output, "LD SP, HL");
            break;
        case 0xFA:
            sprintf ((char *) &output, "JP M,(%02X%02X)", (char) memptr[2],
                     (char) memptr[1]);
            offset = 3;
            break;
        case 0xFB:
            sprintf ((char *) &output, "EI");
            break;
        case 0xFC:
            sprintf ((char *) &output, "CALL M,(%02X%02X)", (char) memptr[2],
                     (char) memptr[1]);
            break;
        case 0xFD:
            /* FD prefixed opcode */
            switch (memptr[1])
            {
            default:
                sprintf ((char *) &output, "??? -> [%02X%02X]",
                         (char) memptr[0], (char) memptr[1]);
                offset = 2;
                disassemble = false;
                break;
            }
            break;
        case 0xFE:
            sprintf ((char *) &output, "CP %02X", (char) memptr[1]);
            offset = 2;
            break;
        case 0xFF:
            sprintf ((char *) &output, "RST38H");
            break;
        default:
            sprintf ((char *) &output, "??? -> [%02X]", (char) memptr[0]);
            disassemble = false;
            break;
        }

emit:

        printf ("%-18s %c ", output, DFS);
        for (i = 0; i < offset; i++)
        {
            printf ("%02X", memptr[i]);
        }
        printf ("\n");
        addr += offset;
        lines++;
        if (lines >= 23)
        {
            if (continue_or_exit ())
            {
                lines = 0;
                printf ("\n");
            }
        }

    }

    printf ("\n");

    return 0;
}
