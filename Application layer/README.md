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