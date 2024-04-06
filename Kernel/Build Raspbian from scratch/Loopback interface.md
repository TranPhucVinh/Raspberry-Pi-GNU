By default, the loopback interface isn't automatically booted up when the Raspbian Busybox is booted up.

# Fail in internal TCP sockets communication

Loopback interface not booted up results in failure in the [one-on-one communication when a TCP sender sends string to a TCP receiver](https://github.com/TranPhucVinh/C/tree/master/Transport%20layer#a-tcp-sender-sends-string-to-a-tcp-receiver).

**Test steps**: 
* Build both tcp_sgl_rx_sgl_sndr and tcp_sgl_sndr_sgl_rx program for Armbian environment then TFTP them to the board
* As we can only access Busybox Raspbian on only 1 USB-UART Window, ``tcp_sgl_rx_sgl_sndr`` must be run as the background process in order to run tcp_sgl_sndr_sgl_rx later 

**Result**
```sh
~ # ./tcp_sgl_rx_sgl_sndr& // Must run tcp_sgl_rx_sgl_sndr as the background process as can only access Busybox Raspbian on only 1 USB-UART Window 
~ # Create TCP receiver socket successfully 
Set socket to reuse address successfully 
Start TCP socket receiver successfully through binding 
Waiting for a TCP sender to connect ... 
~ # ./tcp_sgl_sndr_sgl_rx 
Create TCP socket successfully 
Can't connect to TCP server with error 101
```

From that, we can see that ``tcp_sgl_rx_sgl_sndr`` starts successfully, although the loopback interfaced isn't started. That happens as the address ``INADDR_ANY`` is binded

```c
receiver_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
```
When ``INADDR_ANY`` is specified in the bind call, the socket will be bound to all local interfaces, e.g Ethernet interface (``eth0``), loopback interface (``lo``). In this case, when ``lo`` isn't booted, ``tcp_sgl_rx_sgl_sndr`` will bind to the Ethernet interface (``eth0``).

As ``lo`` isn't booted, ``tcp_sgl_sndr_sgl_rx`` fails with error ``101`` (Network is unreachable)
# HTTP server, which binds to loopback interface, runs on Busybox

When having the Ethernet interface (``eth0``) well setup in Busybox, this [HTTP server program](https://github.com/TranPhucVinh/C/blob/master/Application%20layer/HTTP%20server/multithread_http_server.c) runs successfully. 

That happens as it uses ``INADDR_ANY`` in the bind call, which will bind to the  Ethernet interface (``eth0``) as ``lo`` isn't booted:

```c
http_server_addr.sin_addr.s_addr = htonl(INADDR_ANY); 
```

[multithread_http_server_loopback_interface.c](multithread_http_server_loopback_interface.c) program fails to run when the loopback interface (``lo``) isn't booted up:

**Result**:
```
~ # ./multithread_http_server_loopback_interface
Create HTTP server socket successfully
Unable to set socket to reuse address
Fail to bind socket to local address
```
