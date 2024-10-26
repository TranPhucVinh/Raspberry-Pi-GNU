# SSH

**SSH sever**: Raspberry hosts an SSH server (supported by OpenSSH) for other SSH client like Ubuntu to access to it

## SSH with ESP8266 as Access point

User can SSH enter Raspberry Pi normally with ESP8266 as an Access point

* **Step 1**: Set up Access point for ESP8266
* **Step 2**: Connect your computer to that access point
* **Step 3**: Set up SSID, password of the AP for Raspberry Pi in ``/etc/wpa-supplicant/wpa-supplicant.conf``
* **Step 4**: Using nmap to scan for Raspberry Pi IP: ``nmap -sP 192.168.4.*``. Notice, raspberry pi might not be named in nmap scan result.
* **Step 5**: SSH to Raspberry Pi from computer: ``ssh pi@192.168.4.x``

# HTTP
## [HTTP multithread client to send data](HTTP/HTTP%multithread%client%20to%20send%20data)
## [HTTP multithread client to send and receive data](HTTP/HTTP%20multithread%20client%20to%20send%20and%20receive%20data)
# DHCP

``dhcpcd.service`` is a Systemd service which is only available in Raspbian to handle DHCP. ``dhcpcd.service`` is located inside ``/lib/systemd/system``. Its content:

```sh
[Unit]
Description=DHCP Client Daemon
Wants=network.target
Before=network-online.target
Documentation=man:dhcpcd(8)

[Service]
Type=forking
ExecStart=/usr/sbin/dhcpcd -b -q
Restart=always

[Install]
WantedBy=multi-user.target
```

To set up a new static IP for the wlan0 network interface on Raspberry Pi, we have to modify the ``/etc/dhcpcd.conf`` file. Add the following lines at the end of the ``dhcpcd.conf`` file:

```sh
interface wlan0
static ip_address=192.168.1.100/24
static routers=192.168.1.1
static domain_name_servers=192.168.1.1
```

Then restart the ``dhcpcd.service``: ``sudo systemctl restart dhcpcd``