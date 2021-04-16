## Feature

* Using Wiring Pi
* Raspberry Pi as a HTTP client with multithread sends data to HTTP server.
* HTTP server runs PHP code to handle received data from HTTP client.
* Web browser

### Raspberry

3 thread :

* Thread 1: Send data
* Thread 2: Control LED by button
* Thread 3: Blink LED

### HTTP Server

* Update data on database
* Prinout data

### Webpage feature

* Has clock display
* HTTP client with AJAX (``script.js``)
* Press button in webpage and upate status on database
* Send a POST request with name “request” from Javascript to HTTP server
* Program: ``webpage.php``