# [HTTP multithread client to send data](HTTP%multithread%client%20to%20send%20data)

## Features

* Raspberry Pi as a multithreaded HTTP client:
    * Thread 1: Blink LED
    * Thread 2: Read button status
    * Thread 3: Send data included button status, frame number and serial device number to HTTP server
* HTTP server runs PHP code to handle data from HTTP client.
* Server updates data received from Raspberry to database.

## Source codes

* HTTP Client: [httpClient.c]()
* HTTP server: [httpServer.php]()

# [HTTP multithread client to send and receive data](HTTP%20multithread%20client%20to%20send%20and%20receive%20data)

## Features

* Raspberry Pi as a HTTP client sends data to a HTTP server.
* HTTP server runs PHP code to handle the received data from HTTP client.

## Raspberry

3 threads:
* Thread 1: Send data included button status, frame number and serial device number to HTTP server, then read respond to control LED
* Thread 2: Control LED by button on webpage based on parsing HTTP responsed
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
