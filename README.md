# pico-projects
Various RaspberryPi Pico projects

# Notes:
- Cmake docs: https://cmake.org/cmake/help/v3.13/command/project.html?highlight=project

## Creating a new project and configuring.

- Create a new project folder under *pico-projects*.
- cd into new folder.
- Create *build* directory
- Create CMakeLists.txt
- Copy pico_sdk_import.cmake from pico-sdk/external
- Create your *main.c* file and update CMakeLists.txt
- **important** In VSCode, right-click on *CMakeLists.txt* file and *Configure All Projects*. This will configure things such as intellisence and such.

OR

# 8.3. Automating project creation
The pico project generator, automatically creates a "stub" project with all the necessary files to allow it to build. If you
want to make use of this you’ll need to go ahead and clone the project creation script from its Git repository,

```
$ git clone https://github.com/raspberrypi/pico-project-generator.git
```
It can then be run in graphical mode,
```
$ cd pico-project-generator
$ ./pico_project.py --gui
```

which will bring up a GUI interface allowing you to configure your project

# Building for Picoprobe

- $ cd /media/iposthuman/Nihongo/Hardware/PicoRP2040/test/build/
- (*first time on a change*) cmake ..
- $ make -j4
- $ sudo openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program test.elf verify reset exit"

# Terminal

## Picoprobe
``` $ minicom -b 115200 -o -D /dev/ttyACM0 ```

## Standalone
``` $ minicom -b 115200 -o -D /dev/ttyUSB0 ```
