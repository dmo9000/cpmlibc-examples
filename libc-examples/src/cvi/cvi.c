#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <cpm_sysfunc.h>
#include <tty.h>

extern enum tty_type TERM;
#define CONSOLE_WIDTH       80
#define CONSOLE_HEIGHT 	    25
#define MAX_LINES 			1024
#define BUFSIZE 			4096
#define BLOCK_SIZE			128

#define MODE_NONE           0
#define MODE_INSERT         1
#define MODE_COMMAND        2
#define MAX_CMD             40

#define COMMAND_DEFAULT     0
#define COMMAND_ENTRY       1

int x = 0;
int y = 0;
char *filename = NULL;
static char *edlines[MAX_LINES];
char *current_line;
char databuf[BUFSIZE];
off_t dataseek = 0;
size_t filesize = 0;
bool savestate = false;
bool buffer_dirty = false;
size_t bytes_free = 0;
int linecount = 0;
bool running = false;
int cx = 0;
int cy = 0;
char lastchar = 0;
int current_mode = MODE_NONE;
int linebuf_offset = 0;
int command_mode = COMMAND_DEFAULT;
int previous_mode = MODE_NONE;
size_t initial_segment_size = 0;
uint16_t *program_break = NULL;
int dcount = 0;


void setup_display();
void setup_document();
void update_status_line();
int read_file_to_memory(FILE *fh, size_t fsize, char *mem);
int write_document_to_file();
int setup_line_pointers(char *ptr, size_t limit);
int display_line(int l, bool clear_to_end);
int process_command();
int change_mode(int);
int insert_byte(char c, off_t addr, int line);
int delete_byte(off_t addr, int line);
void reflow_display();
int insert_newline(bool move_to_newline);
int get_line_len(int line);
bool dispatch_cursor_movement(char c);
void cursor_up();
void cursor_down();
void cursor_left();
void cursor_right();
void delete_previous();
void delete_current();


char cmdbuf[MAX_CMD];
int cmdptr = 0;

int main(int argc, char *argv[])
{
    FILE *input_file = NULL;
    struct stat sbuf;
    int rc = 0;
    uint16_t heap_size = 0;
    int i = 0;
    char c = 0;
    char *tmp_ptr = NULL;
    off_t modify_point = 0;
    off_t addr1 = 0;
    off_t addr2 = 0;

    __sdcc_heap_init();
    //bytes_free = heap_init();
    initial_segment_size = bytes_free;
    program_break = (uint16_t*) sbrk(0);
    memset(program_break, 0, bytes_free);
    //printf("program_break = 0x%04x\n", program_break);
    //printf("bytes_free = 0x%04x\n", bytes_free);

    if (argc > 1) {
        filename = argv[1];
    }

    memset(&sbuf, 0, sizeof(struct stat));
    if (filename) {
        rc = stat(filename, &sbuf);
        filesize = sbuf.st_size;
        //printf("rc = %d, filesize = %d, errno = %d, %s\n", rc, filesize, errno, strerror(errno));
    }

    if (filesize > bytes_free) {
        printf("File is too large to fit in memory.\n");
        exit(1);
    }


    for (i = 0 ; i < MAX_LINES; i++) {
        edlines[i] = NULL;
    }

    if (rc == 0) {
        /* open in text mode - UNIX doesn't make any distinction, but CP/M does, so we leave the 'b' flag out */

        input_file = fopen(filename, "r");
        if (!input_file) {
            buffer_dirty = true;
            savestate = false;
        } else {
            if (!read_file_to_memory(input_file, filesize, program_break)) {
                printf("rc = %d, filesize = %d, errno = %d, %s\n", rc, filesize, errno, strerror(errno));
                exit(1);
            } else {

                fclose(input_file);
            }
        }
    } else {
        /* file was not found. we are starting a new document */
        setup_document();
    }

    linecount = setup_line_pointers(program_break, filesize);

    TERM=TERM_VT100;
    setup_display();
    reflow_display();

    running = true;

    /* main loop */

    set_cursor(cx,cy);
    change_mode(MODE_NONE);

    while (running) {
        c = cpm_getchar_noecho();
        lastchar = c;

        /* handle keystrokes */

        if (lastchar > 0) {
            switch (current_mode) {

            /* COMMAND MODE PROCESSING HERE */

            case MODE_COMMAND:
                switch (command_mode) {

                /* PROCESS DIFFERENTLY FOR DEFAULT / LINE ENTRY MODE */

                case COMMAND_ENTRY:
                    switch (lastchar) {
                    case '\r':
                    case '\n':
                        process_command();
                        change_mode(MODE_NONE);
                        break;
                    default:
                        if (lastchar >= 32 && lastchar < 128) {
                            cmdbuf[strlen(cmdbuf)] = lastchar;
                        }
                        update_status_line();
                        break;
                    }
                    break;

                case COMMAND_DEFAULT:
                    switch(lastchar) {
                    case 0x1B:
                        /* let the top level escape handler take it */
                        //printf("received ESC\n");
                        break;
                    case '[':
                        //printf("received ESCANSI\n");
                        break;
                    case 'i':
                        change_mode(MODE_INSERT);
                        break;
                    case 'd':
                        /* delete line, but only if pressed twice */
                        dcount++;
                        if (dcount >= 2) {
                            char *tmp =(char *)  addr2;
                            set_cursor(cx, cy);
                            printf("\x1b[M");
                            display_line(CONSOLE_HEIGHT-2, true);
                            addr1 = (uint16_t) edlines[cy];
                            addr2 = (uint16_t) edlines[cy+1];



                            if (addr2 == NULL) {
                                addr2 = addr1 + get_line_len(cy);
                                tmp = (char *) addr2;
                                while ((char) tmp[0] != '\0') {
                                    tmp++;
                                }
                                addr2 = (uint16_t) tmp;
                            }

                            if (addr1 >= addr2) {
                                clear_screen();
                                printf("addr1 >= addr2 (0x%04x >= 0x%04x)\n", addr1, addr2);
                                exit(1);
                            }

                            /* reindex lines */
                            for (i = 0; i < (addr2-addr1); i++ ) {
                                delete_byte(addr2, cy);
                            }
                            for (i = cy; i < linecount ; i++) {
                                edlines[i] = edlines[i+1];
                            }
                            edlines[MAX_LINES-1] = NULL;
                            linecount--;
                            cx = 0;
                            set_cursor(cx, cy);
                            dcount = 0;
                            change_mode(MODE_COMMAND);
                        }

                        break;
                    /* TODO: not really sure if these are the same thing */
                    case '|':
                    case 'b':
                        /* jump to start of line */
                        cx  = 0;
                        set_cursor(cx,cy);
                        show_cursor(true);
                        change_mode(MODE_COMMAND);
                        break;
                    case 'e':
                        /* jump to end of line */
                        cx = (get_line_len(cy) - 1);
                        set_cursor(cx,cy);
                        show_cursor(true);
                        change_mode(MODE_COMMAND);
                        break;
                    case 'h':
                        /* move one character to the left */
                        cursor_left();
                        break;
                    case 'j':
                        /* move down one line */
                        cursor_down();
                        break;
                    case 'k':
                        /* move up one line */
                        cursor_up();
                        break;
                    /* move one character to the right */
                    case 'l':
                        cursor_right();
                        break;
                    case 'o':
                        /* insert line after */
                        cx = 0;
                        if (cy < linecount) {
                            cy++;
                        }
                        set_cursor(cx,cy);
                        insert_newline(true);
                        update_status_line();
                        //cy--;
                        set_cursor(cx,cy);
                        change_mode(MODE_INSERT);
                        break;
                    case 'x':
                        /* delete character */
                        delete_current();
                        break;
                    case ':':
                    case '/':
                        command_mode = COMMAND_ENTRY;
                        memset(&cmdbuf, 0, MAX_CMD);
                        cmdbuf[0] = lastchar;
                        update_status_line();
                        break;
                    case 0x7F:
                        /* delete character */
                        delete_previous();
                        break;
                    default:
                        switch (lastchar) {
                        case 0x41:
                        case 0x42:
                        case 0x43:
                        case 0x44:
                            dispatch_cursor_movement(lastchar);
                            //change_mode(MODE_INSERT);
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;

            /* "NONE" MODE - ALL WE CAN DO IS SWITCH TO INSERT OR COMMAND MODES */

            case MODE_NONE:
                change_mode(MODE_INSERT);
                switch(lastchar) {
                case 0x1B:              /* ESCAPE  - SWITCH TO COMMAND MODE */
                    change_mode(MODE_COMMAND);
                    break;
                default:                /* OTHERWISE - SWITCH TO INSERT MODE */
                    change_mode(MODE_INSERT);
                    break;
                }
                break;

            /* INSERT MODE */

            case MODE_INSERT:
                switch (lastchar) {
                case 0x7F:
                    /* delete character */
                    delete_previous();
                    break;
                case 10:
                case 13:
                    /* insert newline */
                    insert_newline(true);
                    cy++;
                    set_cursor(cx, cy);
                    update_status_line();
                    show_cursor(true);
                    break;
                case 27:
                    change_mode(MODE_COMMAND);
                    break;
                default:
                    /* insert character into line */
                    if (edlines[cy] == NULL) {
                        /* uncharted territory */
                        if (!cy) {
                            /* line 0 */
                            setup_document();
                        } else {
                            printf("line was dead ... cy = %d, linecount = %d\n", cy, linecount);
                            exit(1);
                        }
                    }

                    modify_point = (off_t) edlines[cy];
                    modify_point += cx;
                    insert_byte(lastchar, (off_t) modify_point, cy);
                    set_cursor(cx, cy);
                    if (get_line_len(cy) > cx+1) {
                        /* optimization hack for UE4 - don't do VT100 insert if we are inserting at the end of the line (performance reasons) */
                        printf("\x1b[1@");
                    }
                    cpm_putchar(lastchar);
                    update_status_line();
                    /* ANSI insert character seems to be broken */
                    if (cx < CONSOLE_WIDTH-1) {
                        cx++;
                    }
                    set_cursor(cx, cy);
                    buffer_dirty = true;
                    savestate = false;
                    break;
                }
            }
        }
    }

exit_editor:
    update_status_line();

    /*
    if (input_file) {
        fclose(input_file);
    }
    */

    clear_screen();
    set_cursor(0,0);

    exit(0);
}

void reflow_display()
{

    int y = 0;
    show_cursor(false);
    for (y = 0 ; y < CONSOLE_HEIGHT - 1; y++) {
        display_line(y, true);
    }
    show_cursor(true);

}

void setup_display()
{
    int y = 0;
    clear_screen();
    reflow_display();
    update_status_line();
}

void update_status_line()
{
    char needsave = ' ';
    if (!savestate && buffer_dirty) {
        needsave = '*';
    }

    switch (current_mode) {
    case MODE_NONE:
    case MODE_INSERT:
        set_cursor(0, CONSOLE_HEIGHT-1);
        set_reverse(true);
        show_cursor(false);
        printf("%s[ {%d}  %d,%d : %c%s (%u/%u) [%d/%d LINES] (%03d)]%s", TTY_FOREGROUND_GREEN, (char *) current_mode, cx+1, linebuf_offset+cy+1,
               needsave, (filename ? filename : "No file"), filesize, (bytes_free - filesize), linecount, (MAX_LINES-linecount), lastchar, TTY_ATTR_RESET);
        set_reverse(false);
        printf("\x1b[K");
        show_cursor(true);
        set_cursor(cx, cy);
        set_cursor(0, CONSOLE_HEIGHT);
        printf("\x1b[K");
        set_cursor(cx, cy);
        break;
    case MODE_COMMAND:
        set_cursor(0, CONSOLE_HEIGHT - 1);
        printf("\x1B[K%s", cmdbuf);
        set_cursor(cx, cy);
        break;
    }

}

int read_file_to_memory(FILE *fh, size_t fsize, char *ptr)
{
    char *eofptr = NULL;
    char *myptr = ptr;
    off_t i = 0;
    int rc = 0;
    //printf("read_file_to_memory(0x%04x, %u, 0x%04x)\n", fh, fsize, ptr);
    for (i = 0 ; i < (fsize / BLOCK_SIZE)+1; i++) {
        //printf("read chunk %d\n", i);
        fseek(fh, i * BLOCK_SIZE, SEEK_SET);
        rc = fread(myptr, BLOCK_SIZE, 1, fh);
        myptr += (rc * BLOCK_SIZE);
        if (feof(fh)) {
            /*  update the size of the memory image, if fcntl() gives us more accurate information about EOF (ie. when file is opened as non-binary) */
            filesize = ftell(fh);
            eofptr = memchr(program_break, 0x1A, initial_segment_size);

            if (eofptr) {
                printf("block contains 0x1A!\n");
                eofptr[0] = '\0';
            }
            goto file_loaded;
        }
    }


file_loaded:

    //printf("filesize = %u\n", filesize);

    return 1;
}

int setup_line_pointers(char *ptr, size_t limit)
{
    char *myptr = NULL;
    int i = 0;
    int lc = 0;
    char *tp = NULL;
    myptr = ptr;
    // printf("setup_line_pointers()\n");


    for (i = 0; i < MAX_LINES; i++) {
        while (myptr[0] == '\n') {
            myptr++;
        }
        tp =  (char *) memchr(myptr, '\r', limit);
        if (!tp) {
            tp = (char *) memchr(myptr, '\n', limit);
        }
        if (!tp) {
            return (lc);
        }
        edlines[i] = (const char *) myptr;
        myptr = tp+1;
        lc ++;
    }
    return lc;
}

int display_line(int l, bool clear_to_end)
{
    int i = 0;
    char *ptr = edlines[l];
    set_cursor(0, l);
    if (clear_to_end) {
        printf("\x1b[K");
    }

    if (edlines[l] == 0x0) {
        set_cursor(0, l);
        printf("%s~%s", TTY_FOREGROUND_HALF_WHITE, TTY_ATTR_RESET);
        return 0;
    }


    while (ptr[0] != '\n' && ptr[0] != '\r' && ptr[0] != '\0' && i < CONSOLE_WIDTH) {
        cpm_putchar(ptr[0]);
        ptr++;
        i++;
    }

    return (0);
}


int process_command()
{
    bool do_save = false;
    bool command_error = false;
    int newlinenum = 0;

    char *ptr = (char *) &cmdbuf;
    if (ptr[0] != ':') {
        return 0;
    }
    ptr++;
    if (atoi(ptr) > 0) {
        /* line number jump */
        newlinenum = atoi(ptr) - 1;
        /* cannot jump beyond final line - move there instead */
        if (newlinenum > (linecount - 1)) {
            newlinenum = (linecount - 1);
        }
        if (newlinenum >= 0 && newlinenum < CONSOLE_HEIGHT -1) {
            cy = newlinenum;
            cx = 0;
            set_cursor(cx, cy);
            change_mode(MODE_INSERT);
            update_status_line();
        }
        return 0;
    }

    while (ptr[0] != '\0') {
        switch(ptr[0]) {
        case 10:
        case 13:
        case '!':
            break;
        case 'w':
            /* write */
            write_document_to_file();
            break;
        case 'q':
            /* quit */
            running = false;
            break;
        default:
            command_error = true;
            break;
        }
        ptr ++;
    }


    if (command_error == true) {
        set_cursor(0,CONSOLE_HEIGHT-1);
        /* clear to end of line */
        printf("\x1B[K%s", cmdbuf);
        set_cursor(0,CONSOLE_HEIGHT-1);
        printf("%s%sE492: not an editor command: %s %s", TTY_ATTR_REVERSE_ON, TTY_FOREGROUND_RED, ptr, TTY_ATTR_RESET);
    }

    command_mode = COMMAND_DEFAULT;

    return 0;

}

int change_mode(int newmode)
{


    switch(newmode) {
    case MODE_NONE:
        current_mode = MODE_NONE;
        command_mode = COMMAND_DEFAULT;
        dcount = 0;
        update_status_line();
        break;
    case MODE_INSERT:
        current_mode = MODE_INSERT;
        command_mode = COMMAND_DEFAULT;
        dcount = 0;
        update_status_line();
        break;
    case MODE_COMMAND:
        memset(&cmdbuf, 0, MAX_CMD);
        current_mode = MODE_COMMAND;
        command_mode = COMMAND_DEFAULT;
        dcount = 0;
        update_status_line();
        break;
    }

    return 0;

}

int insert_byte(char c, off_t addr, int line)
{
    off_t segment_length = 0;
    int i = 0;
    char *ptr = (char *) addr;

    if (!ptr) {
        printf("insert_byte: NULL ptr\n");
        exit(1);
    }


    segment_length = (off_t) ((off_t) initial_segment_size - ((off_t) addr - program_break) - 1);
    memmove(addr+1, addr, segment_length);
    ptr[0] = c;
    filesize++;
    i = line+1;
    while (i < MAX_LINES) {
        if (edlines[i] != NULL) {
            edlines[i]++;
        }
        i++;
    }
    return(0);
}

int delete_byte(off_t addr, int line)
{
    off_t segment_length = 0;
    int i = 0;
    char *ptr = (char *) addr;
    segment_length = (off_t) ((off_t) initial_segment_size - ((off_t) addr - program_break) - 1);
    memmove(addr, addr + 1, segment_length - 1);
    filesize--;
    i = line+1;
    while (i < MAX_LINES) {
        if (edlines[i] != NULL) {
            edlines[i]--;
        }
        i++;

    }
    buffer_dirty = true;
    savestate = false;

    return (0);
}


int insert_newline(bool move_to_newline)
{
    off_t modify_point = 0;
    off_t i = 0;

    if (edlines[cy] == NULL) {
        edlines[cy] = edlines[cy-1];
        edlines[cy] += get_line_len(cy-1);
        while (edlines[cy][0] != '\0') {
            edlines[cy]++;
        }
        //printf("edlines[cy] = 0x%04x\n", edlines[cy]);
        modify_point = (off_t) edlines[cy];
        insert_byte('\n', (off_t) modify_point, cy);
        insert_byte('\r', (off_t) modify_point, cy);
        if (move_to_newline) {
            cx = 0;
        }
        display_line(cy, true);
        set_cursor(cx, cy);
        show_cursor(true);
        linecount++;
        buffer_dirty = true;
        savestate = false;
        return 0;
    }

    modify_point = (off_t) edlines[cy];
    modify_point += cx;
    insert_byte('\n', (off_t) modify_point, cy);
    insert_byte('\r', (off_t) modify_point, cy);
    for (i = (MAX_LINES-1); i > cy+1; i--) {
        edlines[i] = edlines[i-1];
    }
    edlines[cy+1] = (char *) modify_point+2;
    /* VT100/ANSI - insert line */
    set_cursor(0, cy+1);
    printf("\x1b[L");
    set_cursor(0, cy);
    display_line(cy, true);
    if (move_to_newline) {
        cy++;
        display_line(cy, true);
    } else {
        display_line(cy, true);
        display_line(cy+1, true);
    }
    set_cursor(cx, cy);
    linecount++;
    update_status_line();

    if (move_to_newline) {
        cy--;
        cx = 0;
    }

    set_cursor(cx, cy);
    show_cursor(true);
    buffer_dirty = true;
    savestate = false;
    return 0;
}

int get_line_len(int line)
{

    int length = 0;
    char *ptr = edlines[line];

    while (ptr[0] != '\n' && ptr[0] != '\r' && ptr[0] != '\0' && length < CONSOLE_WIDTH) {
        ptr++;
        length++;
    }

    return length;
}

bool dispatch_cursor_movement(char c)
{
    switch (c) {
    case 0x41:
        cursor_up();
        return true;
        break;
    case 0x42:
        cursor_down();
        return true;
        break;
    case 0x43:
        cursor_right();
        return true;
        break;
    case 0x44:
        cursor_left();
        return true;
        break;
    default:
        printf("dispatch cursor unknown code c = 0x%02x (default)\n", c);
        exit(1);
        break;
    }

    return false;
}


void cursor_up()
{
    if (cy) {
        cy--;
        if (cx > get_line_len(cy) - 1) {
            cx = get_line_len(cy) - 1;
        }
        set_cursor(cx,cy);
        show_cursor(true);
    }

}

void cursor_down()
{
    /* move down one line */
    if (cy < CONSOLE_HEIGHT-3) {
        if (cy < (linecount-1)) {
            cy++;
            if (cx > get_line_len(cy) - 1) {
                cx = get_line_len(cy) - 1;
            }
            set_cursor(cx,cy);
            show_cursor(true);
        }
    }

}

void cursor_left()
{
    if (cx) {
        cx--;
        set_cursor(cx,cy);
        show_cursor(true);
    }
}

void cursor_right()
{
    if (cx < (get_line_len(cy) - 1)) {
        cx++;
    }
    set_cursor(cx,cy);
    show_cursor(true);

}

void  delete_previous()
{
    off_t modify_point = 0;

    /* delete character */
    if (cx > 0) {
        modify_point = (off_t) edlines[cy];
        modify_point += (cx - 1);
        set_cursor(cx-1, cy);
        delete_byte((off_t) modify_point, cy);
        printf("\x1b[1P");
        cx --;
    }
    update_status_line();
    set_cursor(cx, cy);
    show_cursor(true);
    buffer_dirty = true;
    savestate = false;

}


void delete_current()
{

    off_t modify_point = 0;
    /* delete character */
    modify_point = (off_t) edlines[cy];
    modify_point += cx;
    delete_byte((off_t) modify_point, cy);
    set_cursor(cx, cy);
    printf("\x1b[1P");
    if (cx >= get_line_len(cy)) {
        cx --;
    }
    update_status_line();
    set_cursor(cx, cy);
    show_cursor(true);
    buffer_dirty = true;
    savestate = false;

}

void setup_document()
{


    /* this gets called if we are starting from a blank file */

    off_t modify_point = 0;
    edlines[cy] = program_break;
    modify_point = (off_t) edlines[cy];
    insert_byte('\n', (off_t) modify_point, cy);
    insert_byte('\r', (off_t) modify_point, cy);
    /* refresh line, to get rid of the default tilde that would hang around if we just inserted */
    display_line(cy, true);
    linecount++;
    buffer_dirty = true;
    savestate = false;


}


int write_document_to_file()
{
    FILE *outfile = NULL;
    int block = 0;
    int lines = 0;
    char *ptr = (char *) program_break;
    int rc = 0;
    off_t offset = 0;
    char *eofmarker = 0;
    /*
    printf("write_document_to_file(%s)\n", filename);
    printf("[writing %d lines, %u bytes]\n", linecount, filesize);
    */
    outfile = fopen(filename, "w");

    if (!outfile) {
        printf("couldn't open file, error = %s\n", strerror(errno));
        exit(1);
    }

    eofmarker = (char *) program_break + filesize;
    eofmarker[0] = 0x1A;

    if (outfile) {
        while (ptr < ((program_break + filesize))) {
            rc = fseek(outfile, offset, SEEK_SET);
            if (rc) {
                printf("fseek(): %s\n", strerror(errno));
                exit(1);
            }
            if (ptr < eofmarker) {
                rc = fwrite((const void*) ptr, 128, 1, outfile);
                if (rc != 1) {
                    printf("fwrite(): %s\n", strerror(errno));
                    exit(1);
                }
            }
            offset += 128;
            ptr += 128;
            block ++;
        }
        fclose(outfile);
    }

write_done:

    eofmarker[0] = 0x0;
    reflow_display();
    return 0;
}
