# ouija

arduino ghost project 👻

# Run Arduino on WSL2 with VS code support

You need [USBIPD](https://github.com/dorssel/usbipd-win/releases). Install the `.msi` file,

```powershell
# run as admin...
uspipd list
usbipd bind --busid 1-2
usbipd attach --wsl --busid 1-2
# ...
usbipd detach --busid 1-2
usbipd unbind --busid 1-2
```

You might want to reattach it when there is problem with usb connection

```shell
# in WSL2
lsusb # check port
```

```shell
sudo chmod a+rw /dev/ttyACM0 # give permissions if not given
```

1. install `Arduino` VS Code add-on for coloring
2. install `arduino-cli`
   ensure you have proper avr core set (can be downloaded from Arduino VS Code add-on)
   (go through hell with path assignment)
3. Command Palette: `"C/C++: Select a Configuration..." -> Arduino`

```shell
alias ac=arduino-cli
ac compile -p /dev/ttyACM0 -b arduino:avr:uno -u ~/projects/00_ongoing/ouija/
```
