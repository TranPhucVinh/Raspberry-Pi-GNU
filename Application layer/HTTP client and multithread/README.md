* HTTP Client: ``httpClient.c``
* HTTP server: ``httpServer.php``

### Feature

* Raspberry Pi as HTTP client with multithread:
    * Thread 1: Blink LED
    * Thread 2: Read button status
    * Thread 3: Send data to HTTP server
* HTTP server runs PHP code to handle data from HTTP client.
* Server updates data received from Raspberry to database.
