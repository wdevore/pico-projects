add_executable(simulated_mcp23s17
    main.c
)

pico_enable_stdio_usb(simulated_mcp23s17 0)
pico_enable_stdio_uart(simulated_mcp23s17 1)

pico_add_extra_outputs(simulated_mcp23s17)

target_link_libraries(simulated_mcp23s17
    pico_stdlib
    hardware_spi
)

# create map/bin/hex file etc.
pico_add_extra_outputs(simulated_mcp23s17)
