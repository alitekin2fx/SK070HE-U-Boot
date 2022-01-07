## Build SPL and U-Boot
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- am335x_evm_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j6
cp ./MLO ../tftp/; cp ./u-boot.img ../tftp/

################# UART ##############################
## Prepare HMI for UART transfer
1) Set 5. dip switch to ON position
2) Connect COM1 to your host machine
3) Power on the HMI

## Prepare HOST machine for UART transfer
# sudo usermod -a -G tty $USER
# sudo usermod -a -G dialout $USER
picocom -b 115200 /dev/ttyUSB0

## Boot HMI Over UART
sx -kb --xmodem ./spl/u-boot-spl.bin < /dev/ttyUSB0 > /dev/ttyUSB0
sx -kb --ymodem ./u-boot.img < /dev/ttyUSB0 > /dev/ttyUSB0

## Flashing over UART
# Flash SPL to NAND
loady ${loadaddr}; nand erase.part NAND.SPL; nand write ${loadaddr} NAND.SPL 0x20000;
sx -kb --ymodem ./MLO < /dev/ttyUSB0 > /dev/ttyUSB0
# Flash U-Boot to NAND
loady ${loadaddr}; nand erase.part NAND.u-boot; nand write ${loadaddr} NAND.u-boot 0x100000;
sx -kb --ymodem ./u-boot.img < /dev/ttyUSB0 > /dev/ttyUSB0
# Combined
loady ${loadaddr}; nand erase.part NAND.SPL; nand write ${loadaddr} NAND.SPL 0x20000;loady ${loadaddr}; nand erase.part NAND.u-boot; nand write ${loadaddr} NAND.u-boot 0x100000;
sx -kb --ymodem ./MLO < /dev/ttyUSB0 > /dev/ttyUSB0; sx -kb --ymodem ./u-boot.img < /dev/ttyUSB0 > /dev/ttyUSB0

################# TFTP ##############################
## Prepare HMI for tftp transfer
# https://bootlin.com/blog/tftp-nfs-booting-beagle-bone-black-wireless-pocket-beagle/
setenv ipaddr 192.168.0.100
setenv serverip 192.168.0.1
setenv ethact usb_ether
setenv usbnet_devaddr f8:dc:7a:00:00:02
setenv usbnet_hostaddr f8:dc:7a:00:00:01

## Prepare HOST for tftp transfer
sudo apt install tftpd-hpa
nmcli con add type ethernet ifname enxf8dc7a000001 ip4 192.168.0.1/24
# Change TFTP_DIRECTORY to "/mnt/tftp" in "/etc/default/tftpd-hpa" file
sudo service tftpd-hpa restart

## Flashing via TFTP
nand erase.chip
tftp ${loadaddr} MLO; nand erase.part NAND.SPL; nand write ${loadaddr} NAND.SPL 0x20000;
tftp ${loadaddr} u-boot.img; nand erase.part NAND.u-boot; nand write ${loadaddr} NAND.u-boot 0x100000;

