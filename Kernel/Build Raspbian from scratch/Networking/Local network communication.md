# Loopback interface
By default, the loopback interface isn't automatically booted up when the Raspbian Busybox is booted up.

## Fail in internal TCP sockets communication

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
## HTTP server, which binds to loopback interface, runs on Busybox

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
# Setup communication between host PC and the Raspberry Pi board containing this customized Raspbian then run an a.out file

**Prerequisites**:
* Raspberry Pi board and host PC connect to the same network, we will use [TFTP](https://github.com/TranPhucVinh/Linux-Shell/tree/master/Application%20layer#tftp) for file transferring between them. Raspberry Pi already has the network connection through **its Ethernet interface**.
* [TFTP server setup on host PC](https://github.com/TranPhucVinh/Linux-Shell/tree/master/Application%20layer#tftp)

When first installing the minimal rootfs on the system, network interfaces doesn't acquire the IP address, ``ip a`` will be:
```sh
~ # ip a
1: lo: <LOOPBACK> mtu 65536 qdisc noop qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
2: eth0: <BROADCAST,MULTICAST> mtu 1500 qdisc noop qlen 1000
    link/ether b8:27:eb:83:52:3b brd ff:ff:ff:ff:ff:ff
```

We need to setup its IP to have the same subnet with the host PC for TFTP file transferring. Use [ifconfig]():
```sh
ifconfig eth0 192.168.1.15 netmask 255.255.255.0 broadcast 192.168.1.255
```
In this case, IP ``192.168.1.15`` of this customized Raspbian will have the same subnet with the host PC, e.g ``192.168.1.13``

Then assign IP ``192.168.1.15`` to eth0 interface permanently then add it to the routing table by ``route`` command:

```sh
# ifconfig eth0 up
# route add -net 192.168.1.0/24 eth0
```

Then compile ``main.c``, a C source code, in host PC by **aarch64-linux-gnu-gcc** compiler (using [-static](https://github.com/TranPhucVinh/C/blob/master/Environment/GCC%20compiler.md#-static) for [static library](https://github.com/TranPhucVinh/C/blob/master/Environment/Static%20library.md))

```sh
aarch64-linux-gnu-gcc -static main.c
```
With TFTP is well setup on host PC, copy the newly built ``a.out`` into ``/var/lib/tftpboot`` for customized Raspbian to get it via TFTP later:

```sh
sudo cp a.out /var/lib/tftpboot
```

In the customized Raspbian board, get that a.out file via TFTP:

```sh
tftp -g -r a.out 192.168.1.13
```
Then file ``a.out`` will be able to run on the customized Raspbian.

**Note**: 
* If don't compile with **-static**, running **a.out** on this Busybox will result in: ``-/bin/sh: ./a.out: not found``
* The above implementation simply targets on internal communication inside a local network and isn't support communication with IP on the Internet as the default routing table of Busybox doesn't have the IP to route it through the IPS gateway.
