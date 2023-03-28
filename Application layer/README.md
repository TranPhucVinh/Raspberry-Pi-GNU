# SSH

**SSH sever**: Raspberry hosts an SSH server (supported by OpenSSH) for other SSH client like Ubuntu to access to it

# [HTTP client and multithread]()

## Feature

* Raspberry Pi as HTTP client with multithread:
    * Thread 1: Blink LED
    * Thread 2: Read button status
    * Thread 3: Send data to HTTP server
* HTTP server runs PHP code to handle data from HTTP client.
* Server updates data received from Raspberry to database.

## Source codes

* HTTP Client: [httpClient.c]()
* HTTP server: [httpServer.php]()

# [HTTP example project](http%20example%20project)

## Feature

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

## Webpage feature

* Has clock display
* HTTP client with AJAX: [script.js]()
* Press button in webpage and upate status on database
* Send a POST request with name “request” from Javascript to HTTP server
* Program: [webpage.php]()