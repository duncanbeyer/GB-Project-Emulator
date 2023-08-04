#include <../headers/io.h>
#include <timer.h>
#include <cpu.h>
#include <dma.h>
#include <lcd.h>
#include <unistd.h>

static char serial_data[2]; // FF01 and FF02. 

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

    if (BETWEEN(address, 0xFF40, 0xFF4B)) { // If reading timer registers
        return lcd_read(address);
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
    if (BETWEEN(address, 0xFF40, 0xFF4B)) { // If reading timer registers
        lcd_write(address, value);
        return;
    }

    printf("UNSUPPORTED BUS READ %04X\n", address);
    sleep(1);
    return;
}