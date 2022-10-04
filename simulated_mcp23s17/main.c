#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"

const uint LED_PIN = PICO_DEFAULT_LED_PIN;

void bufScan()
{
    char buffer[1024];

    while (true)
    {
        scanf("%1024s", buffer);
        printf("%s\n", buffer);
        if (strcmp(buffer, "o") == 0)
        {
            gpio_put(LED_PIN, 1);
        }
        else if (strcmp(buffer, "f") == 0)
        {
            gpio_put(LED_PIN, 0);
        }
    }
}

char charScan()
{
    if (uart_is_readable(uart0))
    {
        while (uart_is_readable(uart0))
        {
            char ch = uart_getc(uart0);
            if (ch != '\n' && ch != '\r')
            {
                // printf("%c\n", ch);
                // if (ch == 'o')
                // {
                //     gpio_put(LED_PIN, 1);
                //     return ch;
                // }
                // if (ch == 'f')
                // {
                //     gpio_put(LED_PIN, 0);
                //     return ch;
                // }
                return ch;
            }
        }
    }
    return '\0';
}

#define MCP23S17_RD_OPCODE 0x41 // Device Op code for Read
#define MCP23S17_RD_DUMMY 0x00  // Dummy data during read
#define MCP23S17_BK0_IODIR_A 0x00
#define MCP23S17_BK0_IOCON_A 0x0A

#ifdef PICO_DEFAULT_SPI_CSN_PIN
static inline void cs_select() {
    // It doesn't seem that the "nop"s are required for the mcp23s17 at 1MHz.
    // Add them if you need them for your device.
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
    asm volatile("nop \n nop \n nop");
}
#endif

// Read from a register:
// |-- Device Opcode --|-- Reg Addr--|-- OutData --| = 3 bytes
static uint8_t read_register(uint8_t regAddr)
{
    uint8_t wbuf[2];
    wbuf[0] = MCP23S17_RD_OPCODE;
    wbuf[1] = regAddr;

    // Assert /CS
    cs_select();

    // Write Opcode and Reg-Addr (2 bytes)
    int bytesWritten = spi_write_blocking(spi_default, wbuf, 2);

    uint8_t rbuf[1];
    if (bytesWritten > 0)
    {
        // Read out data (1 byte)
        // MCP doesn't care what is sent during reads.
        spi_read_blocking(spi_default, MCP23S17_RD_DUMMY, rbuf, 1);
    }

    // De-Assert /CS
    cs_deselect();

    return rbuf[0];
}

static uint8_t write_byte(uint8_t value)
{
    uint8_t wbuf[1];
    wbuf[0] = value;

    // Assert /CS
    cs_select();

    int bytesWritten = spi_write_blocking(spi_default, wbuf, 1);

    // De-Assert /CS
    cs_deselect();

    return bytesWritten;
}

static uint8_t write_byte_no_cs(uint8_t value)
{
    uint8_t wbuf[1];
    wbuf[0] = value;

    // Assert /CS
    cs_select();

    int bytesWritten = spi_write_blocking(spi_default, wbuf, 1);

    // De-Assert /CS
    cs_deselect();

    return bytesWritten;
}

int main()
{
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else

    bi_decl(bi_program_description("This is SPI master."));
    bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();
    uint8_t byteRead;

    // This example will use SPI0 at 100KHz. Max is 3.9MHz
    uint freqAchieved = spi_init(spi_default, 100 * 1000);
    printf("SPI frequency achieved: %d\n", freqAchieved);
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

    while (true)
    {
        char ch = charScan();
        if (ch == '1')
        {
            gpio_put(LED_PIN, 1);
            printf("Reading MCP23S17_BK0_IODIR_A: 0x41, 0x00\n");
            byteRead = read_register(MCP23S17_BK0_IODIR_A);
            printf("IODIRA = (%02x)\n", byteRead);
        }
        else if (ch == '2')
        {
            gpio_put(LED_PIN, 0);
            printf("Reading MCP23S17_BK0_IOCON_A: 0x41, 0x0A\n");
            byteRead = read_register(MCP23S17_BK0_IOCON_A);
            printf("IOCON = (%02x)\n", byteRead);
        }
        else if (ch == '3')
        {
            gpio_put(LED_PIN, 0);
            printf("Sending: 0x41 = MCP23S17_RD_OPCODE\n");
            int bytesWritten = write_byte(MCP23S17_RD_OPCODE);
            printf("bytesWritten = (%d)\n", bytesWritten);
        }
        else if (ch == '4')
        {
            gpio_put(LED_PIN, 1);
            printf("Sending: 0xE4\n");
            int bytesWritten = write_byte(0xE4);
            printf("bytesWritten = (%d)\n", bytesWritten);
            sleep_ms(2000);
            printf("Sending: 0x22\n");
            bytesWritten = write_byte(0x22);
            printf("bytesWritten = (%d)\n", bytesWritten);
        }
        else if (ch == '5')
        {
            gpio_put(LED_PIN, 0);
            uint8_t sendCnt = 0x0A;
            for (uint8_t i = 0; i < 255; i++)
            {
                printf("Sending: (%08x)\n", sendCnt);
                write_byte(i);
                // printf("bytesWritten = (%d)\n", bytesWritten);
                sleep_ms(100);
            }
        }
        else if (ch == '6')
        {
            uint8_t wbuf[1];
            wbuf[0] = 0x41;

            gpio_put(LED_PIN, 1);
            printf("Sending: 0x41\n");

            // Assert /CS
            cs_select();

            spi_write_blocking(spi_default, wbuf, 1);
            // printf("bytesWritten = (%d)\n", bytesWritten);

            // sleep_ms(100);

            // printf("Sending: 0x0A\n");
            wbuf[0] = 0x0A;
            spi_write_blocking(spi_default, wbuf, 1);

            // De-Assert /CS
            cs_deselect();

            // printf("bytesWritten = (%d)\n", bytesWritten);
        }
        else if (ch == '7')
        {
            uint8_t wbuf[2];
            wbuf[0] = 0x41;
            wbuf[1] = 0x0A;

            // 0100_0001 0000_1010
            // 1F = 0001_1111
            gpio_put(LED_PIN, 0);
            printf("Sending: 0x41,0x0A\n");

            // Assert /CS
            cs_select();

            spi_write_blocking(spi_default, wbuf, 2);
            // printf("bytesWritten = (%d)\n", bytesWritten);

            // De-Assert /CS
            cs_deselect();

            // printf("bytesWritten = (%d)\n", bytesWritten);
        }
        else if (ch == '8')
        {
            uint8_t wbuf[2];
            wbuf[0] = 0x41;
            wbuf[1] = 0x0A;

            gpio_put(LED_PIN, 1);
            printf("Sending: 0x41,0x0A\n");

            // Assert /CS
            cs_select();

            spi_write_blocking(spi_default, wbuf, 2);
            // printf("bytesWritten = (%d)\n", bytesWritten);

            // De-Assert /CS
            cs_deselect();

            // printf("bytesWritten = (%d)\n", bytesWritten);
        }
        else if (ch == '9')
        {
            uint8_t wbuf[3];
            wbuf[0] = 0x41;
            wbuf[1] = 0x0A;
            wbuf[2] = 0x00;
            uint8_t rbuf[3];
            rbuf[0] = 0x00;
            rbuf[1] = 0x00;
            rbuf[2] = 0x00;

            gpio_put(LED_PIN, 1);
            printf("Sending: 0x41,0x0A\n");

            // Assert /CS
            cs_select();

            int bwr = spi_write_read_blocking(spi_default, wbuf, rbuf, 3);

            // De-Assert /CS
            cs_deselect();

            printf("bytesWritten = (%d)\n", bwr);
            printf("byte0 returned = (%02x)\n", rbuf[0]);
            printf("byte1 returned = (%02x)\n", rbuf[1]);
            printf("byte2 returned = (%02x)\n", rbuf[2]);
            // 0111_1111 1000_000
            // 
        }
        else if (ch == 'a')
        {
            uint8_t wbuf[3];
            wbuf[0] = 0x41;
            wbuf[1] = 0x0F;
            wbuf[2] = 0x00;
            uint8_t rbuf[3];
            rbuf[0] = 0x00;
            rbuf[1] = 0x00;
            rbuf[2] = 0x00;

            gpio_put(LED_PIN, 1);
            printf("Sending: 0x41,0x0F\n");

            // Assert /CS
            cs_select();

            int bwr = spi_write_read_blocking(spi_default, wbuf, rbuf, 3);

            // De-Assert /CS
            cs_deselect();

            printf("bytesWritten = (%d)\n", bwr);
            printf("byte0 returned = (%02x)\n", rbuf[0]);
            printf("byte1 returned = (%02x)\n", rbuf[1]);
            printf("byte2 returned = (%02x)\n", rbuf[2]);
            // 0111_1111 1000_000
            // 
        }
        else if (ch == 'b')
        {
            uint8_t wbuf[3];
            wbuf[0] = 0x41;
            wbuf[1] = 0x00;
            wbuf[2] = 0x00;
            uint8_t rbuf[3];
            rbuf[0] = 0x00;
            rbuf[1] = 0x00;
            rbuf[2] = 0x00;

            gpio_put(LED_PIN, 1);
            printf("Sending: 0x41,0x00,0x00\n");

            // Assert /CS
            cs_select();

            int bwr = spi_write_read_blocking(spi_default, wbuf, rbuf, 3);

            // De-Assert /CS
            cs_deselect();

            printf("bytesWritten = (%d)\n", bwr);
            printf("byte0 returned = (%02x)\n", rbuf[0]);
            printf("byte1 returned = (%02x)\n", rbuf[1]);
            printf("byte2 returned = (%02x)\n", rbuf[2]);
            // 0111_1111 1000_000
            // 
        }    }
#endif
}