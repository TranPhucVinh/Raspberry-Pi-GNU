## Connect BLE device to Raspberry Pi

**Step 1**: ``sudo bluetoothctl`` to enter Bluetooth control CLI. ``[bluetooth]#`` will then appear

**Step 2**: Start scanning with ``scan on``

```sh
[bluetooth]#scan on
```

**Step 3**: Pair to BLE device that wished to connect with ``pair`` command

```sh
[bluetooth]#pair DC:2C:26:02:30:A2
```

**Step 4**: If pairing in step 3 successfully, connect to that device

```sh
[bluetooth]#connect DC:2C:26:02:30:A2
```
