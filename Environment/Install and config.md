# Manually download Raspbian OS

When copying image to SD card, go to ``boot``, add an ``ssh`` file (just create New document and do nothing on the file).

As of the November 2016 release, Raspbian has the SSH server disabled by default.

**Note**: without adding ssh on boot, user will run into error after typing ``ssh pi@192.168.1.x`` 

``error: ssh: connect to host 192.168.1.13 port 22: Connection refused``

Default ``username`` is ``pi`` and password is ``raspberry``.

# Using Raspberry Pi Imager

In Ubuntu 16.04, install ``rpi-imager``: ``snap install rpi-imager``

After successfully installed, run command ``rpi-imager``. Desktop app ``rpi-imager`` will appear, then choose the OS wished to flashed. However, it will fail to flash the OS into the SD card after writting 100%.

# First time boot

4 ways

* Connect to WiFi in first time boot
* Connect LAN cable from Raspberry to the computer, which is already connected to the Internet, then ``ssh`` to access to that Raspberry
* Connect LAN cable to router, then ``ssh``
* Connect with USB, using Serial, then in Putty, enter CLI by Serial (choose Port, baudrate) ``screen /dev/ttyUSB0 115200``

If Raspberry pi successfully connected to the network, user can ping ``raspberrypi.local`` normally: ``ping raspberrypi.local``

## Connect to WiFi in first time boot

**Step 1**: Connect SD card to SD card adapter, there will be 2 mount: ``boot`` and ``rootfs``.

**Step 2**: Go to ``rootfs``, edit file ``/etc/wpa-supplicant/wpa-supplicant.conf``

```shell
ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev
update_config=1
network={
  ssid="your ssid"
  psk="your pass"
}
```

## Connect with USB in first time boot

With the original Raspbian image, user can't access to Raspbian OS at the first time by USB UART as this image has its USB UART disabled. To enable USB UART pins for later use, use ``sudo raspi-config``

Generally, the ability to access the Embedded Linux OS of the single board computer depends on the image file setup. Some single board computer like Nano Pi Duo allow user to access its OS when first time booted by the USB UART pins.

# VNC

After log in, you need to have the remote desktop

first, install VNC in Raspberry

Read from: "Enabling VNC Server" to "ENABLING VNC SERVER AT THE COMMAND LINE"

**Reference**: [VNC remote access](https://www.raspberrypi.org/documentation/remote-access/vnc/README.md)

Then install VNC in computer

Then open VNC in computer, type the IP address. when connecting, Username: pi, Pass: raspberry
