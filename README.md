# SPRESENSE GNSS Logger

## Getting started with SPRESENSE SDK

### Requrements

* Git
* Spresense SDK

Refer "[Spresense SDK スタートガイド (CLI 版)](https://developer.sony.com/develop/spresense/docs/sdk_set_up_ja.html)" .

### Checkout repositories

```bash
git clone --recursive https://github.com/sonydevworld/spresense.git
cd spresense # spresense/
git clone https://github.com/chibiegg/spresense-gnss-logger.git gnsslogger
```

### Load default config

```bash
cd sdk # spresense/sdk/
source ~/spresenseenv/setup
./tools/config.py -d ../gnsslogger autostart
```

### Build

In `spresense/sdk/` .

```bash
source ~/spresenseenv/setup
make -j8
```

### Flash

```bash
./tools/flash.sh -c /dev/tty.SLAB_USBtoUART nuttx.spk
```


## Getting started with Arduino IDE

Open `logger/logger.ino` .
