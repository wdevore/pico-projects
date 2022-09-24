# Upload
**Incorrect**
```
$ cd Nihongo/Hardware/PicoRP2040/pico-projects/build/test
$ sudo openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program test_project.elf verify reset exit"
```

**Incorrect**
```
$ cd Nihongo/Hardware/PicoRP2040/pico-projects/build/test
$ sudo openocd -f interface/picoprobe.cfg -f target/stm32f1x.cfg -c "program test_project.elf verify reset exit"
```

**Doesn't work**
```
$ cd Nihongo/Hardware/PicoRP2040/pico-projects/build/test
$ sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "program test_project.elf verify reset exit"

Open On-Chip Debugger 0.11.0-g228ede4-dirty (2022-09-24-15:24)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
swd
Info : Hardware thread awareness created
Info : Hardware thread awareness created
Info : RP2040 Flash Bank Command
Info : CMSIS-DAP: SWD  Supported
Info : CMSIS-DAP: FW Version = 0256
Info : CMSIS-DAP: Serial# = 0700000100420019390000084e50364aa5a5a5a597969908
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : SWCLK/TCK = 1 SWDIO/TMS = 1 TDI = 0 TDO = 0 nTRST = 0 nRESET = 1
Error: CMSIS-DAP command CMD_DAP_SWJ_CLOCK failed.
Error: No Valid JTAG Interface Configured.
```


```
$ cd Nihongo/Hardware/PicoRP2040/pico-projects/build/test
$ sudo openocd -f interface/cmsis-dap.cfg -f target/stm32f1x.cfg -c "program test_project.elf verify reset exit"

[sudo] password for iposthuman: 
Open On-Chip Debugger 0.11.0-g228ede4-dirty (2022-09-24-15:24)
Licensed under GNU GPL v2
For bug reports, read
	http://openocd.org/doc/doxygen/bugs.html
swd
Info : CMSIS-DAP: SWD  Supported
Info : CMSIS-DAP: FW Version = 0256
Info : CMSIS-DAP: Serial# = 0700000100420019390000084e50364aa5a5a5a597969908
Info : CMSIS-DAP: Interface Initialised (SWD)
Info : SWCLK/TCK = 1 SWDIO/TMS = 1 TDI = 0 TDO = 0 nTRST = 0 nRESET = 1
Info : CMSIS-DAP: Interface ready
Info : clock speed 1000 kHz
Error: Error connecting DP: cannot read IDR
Info : DAP init failed
in procedure 'program'
** OpenOCD init failed **
shutdown command invoked```


# Pico baseboard
The source code is at: https://github.com/wuxx/pico-lab

## Instructions for openocd 0.11.0
when compiling, you need to add support for cmsis-dap

$ ./bootstrap
$ ./configure --enable-cmsis-dap

add 2 lines to cmsis-dap.cfg
- cmsis_dap_backend hid
- transport select swd

command for reference
```
$ sudo ./src/openocd -s tcl -f tcl/interface/cmsis-dap.cfg -f tcl/target/stm32f1x.cfg
```

## Edit
```/usr/local/share/openocd/scripts/interface``` and remove the *spaces* from the cmsis parameter.

From ```cmsis_ dap_ backend hid``` to ```cmsis_dap_backend```.

## hidapi
```$ sudo apt install libhidapi-dev```