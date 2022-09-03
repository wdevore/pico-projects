#include <stdio.h>
#include "pico/stdlib.h"
// #include "hardware/gpio.h"
// #include "pico/binary_info.h"

int main()
{
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;

    // bi_decl(bi_program_description("This is a test binary."));
    // bi_decl(bi_1pin_with_name(LED_PIN, "On-board LED"));
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    stdio_init_all();
    
    int cnt = 0;

    while (true)
    {
        gpio_put(LED_PIN, 1);
        printf("Hello world ON (%d)\n", cnt);
        sleep_ms(100);
        gpio_put(LED_PIN, 0);
        printf("Hello world OFF (%d)\n", cnt);
        cnt++;
        sleep_ms(1000);
    }
#endif
}