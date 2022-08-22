#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "pico/binary_info.h"
#include "hardware/spi.h"

#include "mcp23s17.h"

const uint LED_PIN = 25;

#ifdef PICO_DEFAULT_SPI_CSN_PIN
static inline void cs_select() {
    // It doesn't seem that the "nop"s are required for the mcp23s17 at 1MHz.
    // Add them if you need them for your device.
    // asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Active low
    // asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    // asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    // asm volatile("nop \n nop \n nop");
}
#endif

// Write to a register:
// |-- Device Opcode --|-- Reg Addr--|-- InData --| = 3 bytes
#if defined(spi_default) && defined(PICO_DEFAULT_SPI_CSN_PIN)
static int write_register(uint8_t regAddr, uint8_t data) {
    uint8_t buf[3];
    buf[0] = MCP23S17_WR_OPCODE;
    buf[1] = regAddr;
    buf[2] = data;

    // Assert /CS
    cs_select();

    // Write 3 bytes
    int bytesWritten = spi_write_blocking(spi_default, buf, 3);

    // De-Assert /CS
    cs_deselect();

    // sleep_ms(1);
    return bytesWritten;
}

// Read from a register:
// |-- Device Opcode --|-- Reg Addr--|-- OutData --| = 3 bytes
static uint8_t read_register(uint8_t regAddr) {
    uint8_t wbuf[2];
    wbuf[0] = MCP23S17_RD_OPCODE;
    wbuf[1] = regAddr;

    // Assert /CS
    cs_select();

    // Write Opcode and Reg-Addr (2 bytes)
    int bytesWritten = spi_write_blocking(spi_default, wbuf, 2);

    uint8_t rbuf[1];
    if (bytesWritten > 0) {
        // Read out data (1 byte)
        // MCP doesn't care what is sent during reads.
        spi_read_blocking(spi_default, MCP23S17_RD_DUMMY, rbuf, 1);
    }

    // De-Assert /CS
    cs_deselect();

    return rbuf[0];
}

static uint8_t read_register2(uint8_t regAddr) {
    uint8_t wbuf[2];
    wbuf[0] = MCP23S17_RD_OPCODE;
    wbuf[1] = regAddr;

    uint8_t rbuf[2];

    // Assert /CS
    cs_select();

    // Write Opcode and Reg-Addr  and data (3 bytes)
    int bytesWritten = spi_write_read_blocking(spi_default, wbuf, rbuf, 2);
    printf("read_register2: (%d) bytes\n", bytesWritten);
    printf("rbuf[0]: (%02x) byte\n", rbuf[0]);
    printf("rbuf[1]: (%02x) byte\n", rbuf[1]);

    // De-Assert /CS
    cs_deselect();

    return rbuf[0];
}

#endif

int blinky_bank0();
int blinky_bank1();
int read_iocon();
int read_iocon_bank0();

// -----------------------------------------------------------------
// Entry point
// -----------------------------------------------------------------
int main()
{
    blinky_bank0();
    // read_iocon_bank0();
}

// -------------------------------------------------------
// Read IOCON register as Bank 1
// Not sure if it is working yet.
// -------------------------------------------------------
int read_iocon() {
    bi_decl(bi_program_description("MCP23S17 IO expander test."));
    bi_decl(bi_1pin_with_name(LED_PIN, "Read IOCON"));
    
    stdio_init_all();
    

#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/mcp23s17 example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else
    printf("_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
    printf("SPI INIT\n");

    // This example will use SPI0 at 1MHz.
    spi_init(spi_default, 1000 * 1000);
    spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    // SPI mode 0 means that Clock Idle is low
    gpio_put(PICO_DEFAULT_SPI_SCK_PIN, 0);

    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));

    int bytesWritten;

    printf("Pausing for trigger\n");
    sleep_ms(5 * 1000);
    printf("Setting registers\n");

    // -----------------------------------------------
    // Now configure MCP
    // See readme for explanation of values.
    // -----------------------------------------------
    // We need to switch the BANK bit first so the rest of
    // Transactions use the correct Addresses
    uint8_t mask = MCP23S17_IOCR_BANK | MCP23S17_IOCR_SEQOP; // 0xA0
    printf("mask = (%02x)\n", mask);
    bytesWritten = write_register(MCP23S17_IOCR_BANK0, mask);
    printf("Wrote to IOCON (%d) bytes\n", bytesWritten);

    uint8_t byteRead = read_register(MCP23S17_IOCR_BANK0);
    printf("IOCON = (%02x)\n", byteRead);

    bytesWritten = write_register(MCP23S17_IODIR_A, 0b00000001);
    printf("Wrote to IODIR (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_INTCON_A, 0b00000001);
    printf("Wrote to INTCON (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_DEFVAL_A, 0b00000001);
    printf("Wrote to DEFVAL (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_GPPU_A, 0b00000001);
    printf("Wrote to GPPU (%d) bytes\n", bytesWritten);

    // printf("Pausing before Loop\n");
    // sleep_ms(5 * 1000);

    int cnt = 0;
    // -----------------------------------------------
    // Now we loop and read registers
    // -----------------------------------------------
    while (1)
    {
        printf("(%d) -----------------------\n", cnt);
        uint8_t byteRead = read_register(MCP23S17_IOCR_BANK0);
        printf("IOCON = (%02x)\n", byteRead);
        byteRead = read_register(MCP23S17_IODIR_A);
        printf("IODIRA = (%02x)\n", byteRead);
        byteRead = read_register(MCP23S17_DEFVAL_A);
        printf("DEFVAL = (%02x)\n", byteRead);
        byteRead = read_register(MCP23S17_GPPU_A);
        printf("GPPU = (%02x)\n", byteRead);
        sleep_ms(1000);
        cnt++;
    }
#endif

    return 0;
}

// -------------------------------------------------------
// Read IOCON register as Bank 0 (default reset state)
// -------------------------------------------------------
int read_iocon_bank0() {
    bi_decl(bi_program_description("MCP23S17 IO expander test."));
    bi_decl(bi_1pin_with_name(LED_PIN, "Read IOCON"));
    
    stdio_init_all();
    

#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/mcp23s17 example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else
    printf("_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-\n");
    printf("SPI INIT\n");

    // This example will use SPI0 at 1MHz.
    spi_init(spi_default, 1000 * 1000);
    spi_set_format(spi_default, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    // SPI mode 0 means that Clock Idle is low
    gpio_put(PICO_DEFAULT_SPI_SCK_PIN, 0);

    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));

    int bytesWritten;
    uint8_t byteRead;

    printf("Pausing for trigger\n");
    sleep_ms(5 * 1000);
    printf("Setting registers\n");

    // -----------------------------------------------
    // Now configure MCP
    // See readme for explanation of values.
    // -----------------------------------------------
    // We need to switch the BANK bit first so the rest of
    // Transactions use the correct Addresses
    uint8_t mask = MCP23S17_IOCR_SEQOP; // 0x20
    printf("mask = (%02x)\n", mask);

    bytesWritten = write_register(MCP23S17_BK0_IOCON_A, mask);
    printf("Wrote to IOCON (%d) bytes\n", bytesWritten);
    byteRead = read_register(MCP23S17_BK0_IOCON_A);
    printf("Read IOCON = (%02x)\n", byteRead);

    bytesWritten = write_register(MCP23S17_BK0_IODIR_A, 0b00000001);
    printf("Wrote to IODIR (%d) bytes\n", bytesWritten);
    byteRead = read_register(MCP23S17_BK0_IODIR_A);
    printf("Read IODIR = (%02x)\n", byteRead);

    bytesWritten = write_register(MCP23S17_BK0_INTCON_A, 0b00000001);
    printf("Wrote to INTCON (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_BK0_DEFVAL_A, 0b00000001);
    printf("Wrote to DEFVAL (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_BK0_GPPU_A, 0b00000001);
    printf("Wrote to GPPU (%d) bytes\n", bytesWritten);

    // printf("Pausing before Loop\n");
    // sleep_ms(5 * 1000);

    int cnt = 0;
    // -----------------------------------------------
    // Now we loop and read registers
    // -----------------------------------------------
    while (true)
    {
        printf("(%d) -----------------------\n", cnt);
        byteRead = read_register(MCP23S17_BK0_IOCON_A);
        printf("IOCON = (%02x)\n", byteRead);
        byteRead = read_register(MCP23S17_BK0_IODIR_A);
        printf("IODIRA = (%02x)\n", byteRead);
        byteRead = read_register(MCP23S17_BK0_DEFVAL_A);
        printf("DEFVAL = (%02x)\n", byteRead);
        byteRead = read_register(MCP23S17_BK0_GPPU_A);
        printf("GPPU = (%02x)\n", byteRead);
        sleep_ms(1000);
        cnt++;
    }
#endif

    return 0;
}

// -------------------------------------------------------
// Output only: Blinky bank 0
// -------------------------------------------------------
int blinky_bank0() {
    bi_decl(bi_program_description("MCP23S17 IO expander test."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
    
    stdio_init_all();
    

#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/mcp23s17 example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else
    // This example will use SPI0 at 1MHz.
    spi_init(spi_default, 1000 * 1000);

    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    // SPI mode 0 means that Clock Idle is low
    gpio_put(PICO_DEFAULT_SPI_SCK_PIN, 0);

    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));

    printf("Pausing for trigger\n");
    sleep_ms(5 * 1000);
    printf("Setting registers\n");

    uint8_t byteRead;
    int bytesWritten;

    // -----------------------------------------------
    // Now configure MCP
    // See readme for explanation of values.
    // -----------------------------------------------
    // We need to switch the BANK bit first so the rest of
    // Transactions use the correct Addresses
    uint8_t mask = MCP23S17_IOCR_SEQOP; // 0x20
    bytesWritten = write_register(MCP23S17_BK0_IOCON_A, mask);
    printf("Wrote to IOCON (%d) bytes\n", bytesWritten);
    byteRead = read_register(MCP23S17_BK0_IOCON_A);
    printf("Read IOCON = (%02x)\n", byteRead);

    bytesWritten = write_register(MCP23S17_BK0_IODIR_A, 0b00000001);
    printf("Wrote to IODIR (%d) bytes\n", bytesWritten);
    byteRead = read_register(MCP23S17_BK0_IODIR_A);
    printf("Read IODIR = (%02x)\n", byteRead);

    bytesWritten = write_register(MCP23S17_BK0_INTCON_A, 0b00000001);
    printf("Wrote to INTCON (%d) bytes\n", bytesWritten);
    byteRead = read_register(MCP23S17_BK0_INTCON_A);
    printf("Read INTCON = (%02x)\n", byteRead);

    bytesWritten = write_register(MCP23S17_BK0_DEFVAL_A, 0b00000001);
    printf("Wrote to DEFVAL (%d) bytes\n", bytesWritten);
    byteRead = read_register(MCP23S17_BK0_DEFVAL_A);
    printf("Read DEFVAL = (%02x)\n", byteRead);

    bytesWritten = write_register(MCP23S17_BK0_GPPU_A, 0b00000001);
    printf("Wrote to GPPU (%d) bytes\n", bytesWritten);
    byteRead = read_register(MCP23S17_BK0_GPPU_A);
    printf("Read GPPU = (%02x)\n", byteRead);

    // -----------------------------------------------
    // Now we loop and toggle GPA1
    // -----------------------------------------------
    while (1)
    {
        // Turn off all Port A pins. Only GPA0 isn't affected because it is
        // an input pin defined above.
        bytesWritten = write_register(MCP23S17_BK0_GPIO_A, 0b00000000);
        byteRead = read_register(MCP23S17_BK0_GPIO_A);
        printf("Off: Read GPIO_A = (%02x)\n", byteRead);
        sleep_ms(200);
        // Turn on Port A bit 0 pin
        bytesWritten = write_register(MCP23S17_BK0_GPIO_A, 0b00000010);
        byteRead = read_register(MCP23S17_BK0_GPIO_A);
        printf("On: Read GPIO_A = (%02x)\n", byteRead);
        sleep_ms(50);
    }
#endif

    return 0;
}

int blinky_bank1() {
    bi_decl(bi_program_description("MCP23S17 IO expander test."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
    
    stdio_init_all();
    

#if !defined(spi_default) || !defined(PICO_DEFAULT_SPI_SCK_PIN) || !defined(PICO_DEFAULT_SPI_TX_PIN) || !defined(PICO_DEFAULT_SPI_CSN_PIN)
#warning spi/mcp23s17 example requires a board with SPI pins
    puts("Default SPI pins were not defined");
#else
    // This example will use SPI0 at 1MHz.
    spi_init(spi_default, 1000 * 1000);

    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);

    // Make the SPI pins available to picotool
    bi_decl(bi_3pins_with_func(PICO_DEFAULT_SPI_RX_PIN, PICO_DEFAULT_SPI_TX_PIN, PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI));

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);

    // Make the CS pin available to picotool
    bi_decl(bi_1pin_with_name(PICO_DEFAULT_SPI_CSN_PIN, "SPI CS"));

    // SPI mode 0 means that Clock Idle is low
    gpio_put(PICO_DEFAULT_SPI_SCK_PIN, 0);

    // -----------------------------------------------
    // Now configure MCP
    // See readme for explanation of values.
    // -----------------------------------------------
    // We need to switch the BANK bit first so the rest of
    // Transactions use the correct Addresses
    uint8_t mask = MCP23S17_IOCR_BANK | MCP23S17_IOCR_SEQOP; // 0xA0
    int bytesWritten = write_register(MCP23S17_IOCR_BANK0, mask);
    printf("Wrote to IOCON (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_IODIR_A, 0b00000001);
    printf("Wrote to IODIR (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_INTCON_A, 0b00000001);
    printf("Wrote to INTCON (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_DEFVAL_A, 0b00000001);
    printf("Wrote to DEFVAL (%d) bytes\n", bytesWritten);

    bytesWritten = write_register(MCP23S17_GPPU_A, 0b00000001);
    printf("Wrote to GPPU (%d) bytes\n", bytesWritten);

    // -----------------------------------------------
    // Now we loop and toggle GPA1
    // -----------------------------------------------
    while (1)
    {
        // Turn off all Port A pins
        bytesWritten = write_register(MCP23S17_GPIO_A, 0b00000000);
        puts("All pins off");
        sleep_ms(1000);
        // Turn on Port A bit 0 pin
        bytesWritten = write_register(MCP23S17_GPIO_A, 0b00000001);
        puts("Port A bit 0 on");
        sleep_ms(1000);
    }
#endif

    return 0;
}

