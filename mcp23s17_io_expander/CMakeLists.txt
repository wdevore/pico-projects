add_executable(mcp23s17
    mcp23s17.h
    main.c
)

pico_enable_stdio_usb(mcp23s17 0)
pico_enable_stdio_uart(mcp23s17 1)

pico_add_extra_outputs(mcp23s17)

target_link_libraries(mcp23s17
    pico_stdlib
    hardware_spi
)

# create map/bin/hex file etc.
pico_add_extra_outputs(mcp23s17)
