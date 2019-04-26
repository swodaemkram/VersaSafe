/*

	lsusb
Bus 001 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub
Bus 003 Device 005: ID 17e9:436e DisplayLink 
Bus 003 Device 004: ID 2109:0813 VIA Labs, Inc. 
Bus 003 Device 003: ID 17e9:436e DisplayLink 
Bus 003 Device 002: ID 2109:0813 VIA Labs, Inc. 
Bus 003 Device 001: ID 1d6b:0003 Linux Foundation 3.0 root hub
Bus 002 Device 005: ID 8087:0a2a Intel Corp. 
Bus 002 Device 003: ID 413c:2003 Dell Computer Corp. Keyboard
Bus 002 Device 010: ID 04d8:900a Microchip Technology, Inc. PICkit3
Bus 002 Device 008: ID 06cd:0121 Keyspan USA-19hs serial adapter
Bus 002 Device 009: ID 04d8:000a Microchip Technology, Inc. CDC RS-232 Emulation Demo
Bus 002 Device 007: ID 04b4:6570 Cypress Semiconductor Corp. 
Bus 002 Device 006: ID 03f0:0b4a Hewlett-Packard 
Bus 002 Device 004: ID 2109:2813 VIA Labs, Inc. 
Bus 002 Device 002: ID 2109:2813 VIA Labs, Inc. 
Bus 002 Device 001: ID 1d6b:0002 Linux Foundation 2.0 root hub


root@garys-box:/home/garyc/Desktop/VersaSafe/src# ./findusb
/dev/ttyUSB0 - Keyspan__a_division_of_InnoSys_Inc._Keyspan_USA-19H
/dev/input/mouse0 - Logitech_USB_Optical_Mouse
/dev/input/event4 - Logitech_USB_Optical_Mouse
/dev/ttyACM0 - FKI_Security_Group_LLC._Apunix_LPM-01__
/dev/input/event3 - Dell_Dell_USB_Keyboard
/dev/snd/controlC2 - DisplayLink_Dell_D3100_USB3.0_Dock_1709272943



	lsusb -t
/:  Bus 03.Port 1: Dev 1, Class=root_hub, Driver=xhci_hcd/4p, 5000M
    |__ Port 3: Dev 2, If 0, Class=Hub, Driver=hub/4p, 5000M
        |__ Port 1: Dev 3, If 0, Class=Vendor Specific Class, Driver=, 5000M
        |__ Port 1: Dev 3, If 1, Class=Application Specific Interface, Driver=, 5000M
        |__ Port 2: Dev 4, If 0, Class=Hub, Driver=hub/4p, 5000M
        |__ Port 3: Dev 5, If 0, Class=Vendor Specific Class, Driver=, 5000M
        |__ Port 3: Dev 5, If 1, Class=Application Specific Interface, Driver=, 5000M
        |__ Port 3: Dev 5, If 2, Class=Audio, Driver=snd-usb-audio, 5000M
        |__ Port 3: Dev 5, If 3, Class=Audio, Driver=snd-usb-audio, 5000M
        |__ Port 3: Dev 5, If 4, Class=Audio, Driver=snd-usb-audio, 5000M
        |__ Port 3: Dev 5, If 5, Class=Communications, Driver=cdc_ncm, 5000M
        |__ Port 3: Dev 5, If 6, Class=CDC Data, Driver=cdc_ncm, 5000M
/:  Bus 02.Port 1: Dev 1, Class=root_hub, Driver=xhci_hcd/11p, 480M
    |__ Port 3: Dev 2, If 0, Class=Hub, Driver=hub/4p, 480M
        |__ Port 2: Dev 4, If 0, Class=Hub, Driver=hub/4p, 480M
            |__ Port 1: Dev 6, If 0, Class=Human Interface Device, Driver=usbhid, 1.5M
            |__ Port 2: Dev 7, If 0, Class=Hub, Driver=hub/4p, 480M
                |__ Port 1: Dev 9, If 1, Class=CDC Data, Driver=cdc_acm, 12M
                |__ Port 1: Dev 9, If 0, Class=Communications, Driver=cdc_acm, 12M
            |__ Port 3: Dev 8, If 0, Class=Vendor Specific Class, Driver=keyspan, 12M
            |__ Port 4: Dev 10, If 0, Class=Human Interface Device, Driver=usbhid, 12M
    |__ Port 4: Dev 3, If 0, Class=Human Interface Device, Driver=usbhid, 1.5M
    |__ Port 7: Dev 5, If 1, Class=Wireless, Driver=btusb, 12M
    |__ Port 7: Dev 5, If 0, Class=Wireless, Driver=btusb, 12M
/:  Bus 01.Port 1: Dev 1, Class=root_hub, Driver=ehci-pci/2p, 480M
    |__ Port 1: Dev 2, If 0, Class=Hub, Driver=hub/8p, 480M


	cat /etc/fstab


	dmesg | grep tty
[    0.000000] console [tty0] enabled
[    0.772522] serial8250: ttyS0 at I/O 0x3f8 (irq = 4, base_baud = 115200) is a 16550A
[   36.707198] cdc_acm 2-3.2.2.1:1.0: ttyACM0: USB ACM device
[   36.862468] usb 2-3.2.3: Keyspan 1 port adapter converter now attached to ttyUSB0


	for ubuntu

	usb-devices (this is a bash script)

T:  Bus=02 Lev=03 Prnt=04 Port=03 Cnt=04 Dev#= 10 Spd=12  MxCh= 0
D:  Ver= 2.00 Cls=00(>ifc ) Sub=00 Prot=00 MxPS= 8 #Cfgs=  1
P:  Vendor=04d8 ProdID=900a Rev=00.02
S:  Manufacturer=Microchip Technology Inc.
S:  Product=PICkit 3
S:  SerialNumber=BUR164233248
C:  #Ifs= 1 Cfg#= 1 Atr=80 MxPwr=500mA
I:  If#= 0 Alt= 0 #EPs= 2 Cls=03(HID  ) Sub=00 Prot=00 Driver=usbhid


usbview


/sys/kernel/debug/usb/devices
ls /sys/bus/usb/devices

lsusb -v



*/


