#include <dbg.h>
#include <bus.h>
#include <cpu.h>

// FILE *fp2;
// Debugging stuff

static char dbg_msg[1024] = {0};
static int msg_size = 0;

void dbg_update() {
    if (bus_read(0xFF02) == 0x81) { // If there is a transfer requested
        char c = bus_read(0xFF01);

        dbg_msg[msg_size++] = c;

        bus_write(0xFF02, 0);
    }
}

void dbg_print() {
    if (dbg_msg[0]) {
        printf("DBG: %s\n", dbg_msg);
        // fp2 = fopen("hram.txt", "w");
        // fprintf(fp2, "DBG: %s\n", dbg_msg);
        // fclose(fp2);
    }
}

