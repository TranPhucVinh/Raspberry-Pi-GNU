# Overview
This is a tutorial to set up network for busybox. Basically, for a host to communicate to the Internet, 2 fundamental things are required: 
* An available IP in the local network: This can be achieved by statically setup the IP or using DHCP ([step 1: Get DHCP IP from the router](#step-1-get-dhcp-ip-from-the-router))
* Config the routing table with the IP of the ISP gateway so that the host's packets can be routed to Internet devices: [Step 2: Setup the routing table](#step-2-setup-the-routing-table)

Beside that, for easier communication, DNS resolver is required so that we can communicate directly with the URL, instead of using the IP. This is implemented in [step 3: Setup DNS](#step-3-setup-dns)

# Step 1: Get DHCP IP from the router

IP for the Raspbian Busybox can be setup statically like in the [Setup communication between host PC and the Raspberry Pi board containing this customized Raspbian then run an a.out file](Local%20network%20communication.md#setup-communication-between-host-pc-and-the-raspberry-pi-board-containing-this-customized-raspbian-then-run-an-aout-file) implementation. However, for automatic implementation, DHCP is prefered.

First let create directory ``/etc/network``, then create file ``interfaces`` inside that directory, which is identical to ``/etc/network/interfaces`` on Ubuntu.

Then add the following content to that file:
```sh
auto lo
iface lo inet loopback
auto eth0
iface eth0 inet dhcp
```

``iface eth0 inet dhcp``: instruct the system to configure the eth0 interface to obtain its network configuration dynamically via DHCP. When the system brings up the ``eth0`` interface, it will send out a DHCP request to a DHCP server on the network to obtain an IP address, subnet mask, default gateway, DNS servers, etc.

And then running the command ``"ifup -a"`` to bring up network for all interfaces
```sh
~ # ifup -a
run-parts: /etc/network/if-pre-up.d: No such file or directory
run-parts: /etc/network/if-up.d: No such file or directory
ifup: can't open '/var/run/ifstate.new': No such file or directory
```

* ``ifup``: bring a network interface up
* ``ifup -a``: Bring up all network interfaces

* ``/etc/network/if-pre-up.d/``: Scripts in this directory are run before bringing the interface up
* ``/etc/network/if-up.d/``: Scripts in this directory are run after bringing the interface up

For the first issue ``run-parts: /etc/network/if-pre-up.d: No such file or directory``, we just simply need to create that directory:
```sh
~ # mkdir /etc/network/if-pre-up.d
```
For the second issue ``run-parts: /etc/network/if-up.d: No such file or directory``, we just simply need to create that directory:
```sh
~ # mkdir /etc/network/if-up.d
```
Last issue: ``can't open '/var/run/ifstate.new': No such file or directory``
- From my experiences, everything have to do with ``var`` or ``run`` usually run on ramfs
```sh
~ # mkdir /var
# the next command is to mount 256M of RAM partion onto /var
~ # mount -t tmpfs -o size=256M tmpfs /var
~ # mkdir /var/run
```
**Note**: Command ``ifup -a`` on Busybox simply checks for the existence inside all 3 folders ``/etc/network/if-pre-up.d/``, ``/etc/network/if-up.d/`` and ``/etc/network/if-pre-up.d``, without worrying about the content inside them. What we have done so far is simply creating 3 empty folders to run ``ifup -a``.

Now the command ``ifup -a`` is ready
```sh
~ # ifup -a
ip: RTNETLINK answers: File exists
udhcpc: started, v1.37.0.git
udhcpc: broadcasting discover
udhcpc: broadcasting select for 192.168.1.8, server 192.168.1.1
udhcpc: lease of 192.168.1.8 obtained from 192.168.1.1, lease time 86400
```
# Step 2: Setup the routing table
We have obtain the dhcp IP from router. Based on the result of step 1, we have the IP ``192.168.1.8``. Let config it into eth0 interface
```sh
~ # ifconfig eth0 192.168.1.8 netmask 255.255.255.0
~ # ip a
1: lo: <LOOPBACK,UP,LOWER_UP> mtu 65536 qdisc noqueue qlen 1000
    link/loopback 00:00:00:00:00:00 brd 00:00:00:00:00:00
    inet 127.0.0.1/8 scope host lo
       valid_lft forever preferred_lft forever
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> mtu 1500 qdisc pfifo_fast qlen 1000
    link/ether b8:27:eb:83:52:3b brd ff:ff:ff:ff:ff:ff
    inet 192.168.1.8/24 brd 192.168.1.255 scope global eth0
       valid_lft forever preferred_lft forever
```

But we're still not able to ping a public domain like ``google.com``.

That happens as the routing table of Busybox, by default, doesn't have the IPS gateway IP to direct the packet to the Internet:

```sh
~ # route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
192.168.1.0     0.0.0.0         255.255.255.0   U     0      0        0 eth0
```
While the routing table of Ubuntu includes:
```sh
$ route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.168.1.1     0.0.0.0         UG    100    0        0 eth0
192.168.1.0     0.0.0.0         255.255.255.0   U     100    0        0 eth0
```
From that, we can see that we the crucial rule to communicate with the Internet. Basically, for any public IP that we send out, we route it through the gateway (which is your IPS router) so it can redirect it and send to the internet. Now let add that rules. As can be seen from the routing table of Ubuntu which has the router IP address of ``192.168.1.1``, add this address to the routing table of the Busybox:
```sh
~ # route add -net 0.0.0.0 gw 192.168.1.1 eth0
~ # route -n
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.168.1.1     0.0.0.0         UG    0      0        0 eth0
192.168.1.0     0.0.0.0         255.255.255.0   U     0      0        0 eth0
```
Now we're able to ping public IP address. E.g, we can ping to the IP address ``142.251.220.14`` of google.com but we can't still ping direcyly to the URL ``google.com``. That happens as we haven't had the DNS config on the Busybox
