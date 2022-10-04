# Description
This project is for testing a SystemVerilog SPI module interacting with a SPI device.

We send two bytes and expect one byte in return.

- assert /SS.
- 0x41
- 0x0A
- fpga Top module detects sequence and returns 0x28
- de-assert /SS

Second sequence

- assert /SS.
- 0x41
- 0x00
- fpga Top module detects sequence and returns 0xF9
- de-assert /SS

# UART setup for communication
Using minicom we need to turn off *Hardware Flow Control* manully. Press:

- Ctrl-A
- o
- select "Serial port setup"
- f

This disables flow-control so you can talk to the pico. Or you can use the */etc/minirc/minirc.pico_io* in the workflow below. Also see root readme.md.

# Build/Run workflow
See root readme.md for creating and configuring project prior to running *make*

```
$ cd build/simulated_mcp23s17
$ make
- Hold Reset and Boot.
- Release Reset then Boot and wait for Pico to mount.
$ cp simulated_mcp23s17cd.uf2 /media/<path to uf2>/RPI-RP2
$ minicom -b 115200 -o -D /dev/ttyUSB0 pico_io
```

# Upload
## Via Picoprobe
sudo openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program mcp23s17.elf verify reset exit"

## Via drag/drop UF2
- hold Reset and Boot
- let go of Reset
- let go of Boot
- Wait for it to mount.
- Then copy or drag the *.uf2* file to the mounted drive.

# Logic Analyzer
```
LA               Pico        Ribbon
gray    = MOSI = GP19 = Tx   brown
brown   = SCL  = GP18 = CLK  red
red     = /SS  = GP17 = CS   yellow
orange  = MISO = GP16 = Rx   green
```