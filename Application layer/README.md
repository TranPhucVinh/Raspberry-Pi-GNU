# SSH

**SSH sever**: Raspberry hosts an SSH server (supported by OpenSSH) for other SSH client like Ubuntu to access to it

## SSH with ESP8266 as Access point

User can SSH enter Raspberry Pi normally with ESP8266 as Access point

* **Step 1**: Set up Access point for ESP8266
* **Step 2**: Connect your computer to that access point
* **Step 3**: Set up SSID, Password of the AP for Raspberry Pi in ``/etc/wpa-supplicant/wpa-supplicant.conf``
* **Step 4**: Using nmap to scan for Raspberry Pi IP: ``nmap -sP 192.168.4.*``. Notice, raspbery pi might not be named in nmap scan result.
* **Step 5**: SSH to Raspberry Pi from computer: ``ssh pi@192.168.4.x``

# [HTTP client and multithread](HTTP%20client%20and%20multithread)

## Features

* Raspberry Pi as HTTP client with multithread:
    * Thread 1: Blink LED
    * Thread 2: Read button status
    * Thread 3: Send data to HTTP server
* HTTP server runs PHP code to handle data from HTTP client.
* Server updates data received from Raspberry to database.

## Source codes

* HTTP Client: [httpClient.c]()
* HTTP server: [httpServer.php]()

# [HTTP example project](HTTP%20example%20project)

## Features

* Raspberry Pi as a HTTP client sends data to a HTTP server.
* HTTP server runs PHP code to handle the received data from HTTP client.

## Raspberry

3 threads:

* Thread 1: Send data
* Thread 2: Control LED by button
* Thread 3: Blink LED

## HTTP Server

* Update data on database
* Prinout data
* Program: [index.php](HTTP%20example%20project/index.php)

**Webpage feature**

* Has clock display
* HTTP client with AJAX: [script.js](HTTP%20example%20project/script.js)
* Press button in webpage and upate status on database
* Send a POST request with name “request” from Javascript to HTTP server
* Program: [webpage.php](HTTP%20example%20project/webpage.php)