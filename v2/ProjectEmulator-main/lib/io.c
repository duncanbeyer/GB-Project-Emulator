#include <../headers/io.h>
#include <timer.h>
#include <cpu.h>
#include <dma.h>
#include <unistd.h>

static char serial_data[2]; // FF01 and FF02. 

u8 ly = 0;

u8 io_read(u16 address) { // Read the Serial Transfer Data and Serial Transfer Control.
    if (address == 0xFF01) {
        return serial_data[0];
    }
    if (address == 0xFF02) {
        return serial_data[1];
    }

    if (BETWEEN(address, 0xFF04, 0xFF07)) { // If reading timer registers
        return timer_read(address);
    }

    if (address == 0xFF0F) { // If reading the interrupt flags
        return cpu_get_int_flags();
    }
    if (address = 0xFF44) { // ly register
        return ly++;
        // return 0;
    }

    printf("UNSUPPORTED BUS read %04X\n", address);
    return 0;
}
void io_write(u16 address, u8 value) {
    if (address == 0xFF01) {
        serial_data[0] = value;
        return;
    }
    if (address == 0xFF02) {
        serial_data[1] = value;
        return;
    }
    
    if (BETWEEN(address, 0xFF04, 0xFF07)) { // If reading timer registers
        timer_write(address, value);
        return;
    }

    if (address = 0xFF0F) {
        cpu_set_int_flags(value);
        return;
    }
    if (address = 0xFF46) {
        dma_start(value);
        printf("\n \n \n \n \n DMA START!\n \n \n \n \n");
        return;
    }

    printf("UNSUPPORTED BUS READ %04X\n", address);
    sleep(1000);
    return;
}