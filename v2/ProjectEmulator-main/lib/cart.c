#include <cart.h>

typedef struct {
    char filename [1024];
    u32 rom_size;
    u8 *rom_data;
    rom_header *header;
} cart_context;

static cart_context ctx;

static const char *ROM_TYPES[] = { // Cart type, determined by a byte in cart header
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

static const char *LIC_CODE[0xA5] = { // License names, indexed by a byte in cart header
    [0x00] = "None",
    [0x01] = "Nintendo R&D1",
    [0x08] = "Capcom",
    [0x13] = "Electronic Arts",
    [0x18] = "Hudson Soft",
    [0x19] = "b-ai",
    [0x20] = "kss",
    [0x22] = "pow",
    [0x24] = "PCM Complete",
    [0x25] = "san-x",
    [0x28] = "Kemco Japan",
    [0x29] = "seta",
    [0x30] = "Viacom",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Ocean/Acclaim",
    [0x34] = "Konami",
    [0x35] = "Hector",
    [0x37] = "Taito",
    [0x38] = "Hudson",
    [0x39] = "Banpresto",
    [0x41] = "Ubi Soft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "angel",
    [0x47] = "Bullet-Proof",
    [0x49] = "irem",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "American sammy",
    [0x54] = "Konami",
    [0x55] = "Hi tech entertainment",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x58] = "Mattel",
    [0x59] = "Milton Bradley",
    [0x60] = "Titus",
    [0x61] = "Virgin",
    [0x64] = "LucasArts",
    [0x67] = "Ocean",
    [0x69] = "Electronic Arts",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbund",
    [0x73] = "sculptured",
    [0x75] = "sci",
    [0x78] = "THQ",
    [0x79] = "Accolade",
    [0x80] = "misawa",
    [0x83] = "lozc",
    [0x86] = "Tokuma Shoten Intermedia",
    [0x87] = "Tsukuda Original",
    [0x91] = "Chunsoft",
    [0x92] = "Video system",
    [0x93] = "Ocean/Acclaim",
    [0x95] = "Varie",
    [0x96] = "Yonezawa/s’pal",
    [0x97] = "Kaneko",
    [0x99] = "Pack in soft",
    [0xA4] = "Konami (Yu-Gi-Oh!)"
};

const char *cart_lic_name() { // Retrieves the publisher license based on the cart header
    if (ctx.header->new_lic_code <= 0xA4) {
        return LIC_CODE[ctx.header->lic_code];
    }
    return "UNKNOWN";
}

const char *cart_type_name() { // Retrieves the type of cart
    if (ctx.header->type <= 0x22) {
        return ROM_TYPES[ctx.header->type];
    }
    return "UNKNOWN";
}

bool cart_load(char *cart) {
    snprintf(ctx.filename, sizeof(ctx.filename), "%s", cart);

    FILE *fp = fopen(cart, "r"); // Open the ROM file

    if (!fp) { // If the ROM file failed to open properly
        printf("Failed to open: %s\n", cart);
        return false;
    }

    printf("Opened: %s\n", cart);

    fseek(fp, 0, SEEK_END); // fseek moves fp to the end of the file
    ctx.rom_size = ftell(fp);// ftell sets ctx.rom_size equal to the size of the file

    rewind(fp); // rewind moves fp back to the beginning of the file

    ctx.rom_data = malloc(ctx.rom_size); // set rom_data to a pointer to memory of 
    // needed size
    fread(ctx.rom_data, ctx.rom_size, 1, fp); // read rom_size from fp into rom_data
    fclose(fp); // close fp

    ctx.header = (rom_header *)(ctx.rom_data + 0x100); 
    ctx.header->title[15] = 0; // set the checksum to 0


    printf("Cartridge Loaded:\n");
    printf("\t Title    : %s\n", ctx.header->title);
    printf("\t Type     : %2.2X (%s)\n", ctx.header->type, cart_type_name());
    printf("\t ROM Size : %d KB\n", 32 << ctx.header->rom_size);
    printf("\t RAM Size : %2.2X\n", ctx.header->ram_size);
    printf("\t LIC Code : %2.2X (%s)\n", ctx.header->lic_code, cart_lic_name());
    printf("\t ROM Vers : %2.2X\n", ctx.header->version);

    u8 checksum = 0;
    for (u16 check_addr = 0x0134; check_addr <= 0x014c; check_addr++) {
        checksum = checksum - ctx.rom_data[check_addr] - 1;
    }

    printf("\t Checksum : %2.2X (%s)\n", ctx.header->checksum, (checksum & 0xFF) ? "PASSED" : "FAILED");
    return true;
}

// ROM ONLY is the only type supported so far
u8 cart_read(u16 address) { // Retrieves a byte of data from the cart
    return ctx.rom_data[address];
}

void cart_write(u16 address, u8 value) {
    printf("cart_write(%04X)\n", address);
    // ctx.rom_data[address] = 0xFF & value;
}
