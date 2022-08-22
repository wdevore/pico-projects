add_executable(test_project
    test.c
)

pico_enable_stdio_usb(test_project 0)
pico_enable_stdio_uart(test_project 1)

target_link_libraries(test_project
    pico_stdlib
)

# create map/bin/hex file etc.
pico_add_extra_outputs(test_project)

