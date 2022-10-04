#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

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

void charScan()
{
    while (true)
    {
        if (uart_is_readable(uart0))
        {
            while (uart_is_readable(uart0))
            {
                char ch = uart_getc(uart0);
                if (ch != '\n' && ch != '\r')
                {
                    printf("%c\n", ch);
                    if (ch == 'o')
                    {
                        gpio_put(LED_PIN, 1);
                    }
                    if (ch == 'f')
                    {
                        gpio_put(LED_PIN, 0);
                    }
                }
            }
        }
    }
}

int main()
{
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else

    // bi_decl(bi_program_description("This is simulated MCP23S17."));
    // bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();

    while (true)
    {
        charScan();
    }
#endif
}