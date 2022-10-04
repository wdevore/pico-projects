add_executable(io_uart
    main.c
)

pico_enable_stdio_usb(io_uart 0)
pico_enable_stdio_uart(io_uart 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(io_uart)

target_link_libraries(io_uart
    pico_stdlib
)
