# Upload
sudo openocd -f interface/picoprobe.cfg -f target/rp2040.cfg -c "program test_project.elf verify reset exit"

