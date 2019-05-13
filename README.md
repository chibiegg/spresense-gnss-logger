# SPRESENSE GNSS Logger

## Getting started with SPRESENSE SDK

### Requrements

* Git
* Docker
* Python
* kconfig-frontends

Refer "[macOSでSPRESENSE SDKを使った開発環境を用意する](https://qiita.com/chibiegg/items/11836bb5cef8bf314103)" .

### Checkout repositories

```bash
git clone --recursive https://github.com/sonydevworld/spresense.git
cd spresense # spresense/
git clone https://github.com/chibiegg/spresense-gnss-logger.git gnsslogger
```

### Load default config

```bash
cd sdk # spresense/sdk/
cp -r ../gnsslogger/configs configs/gnsslogger
tools/config.py --kernel release
tools/config.py gnsslogger/logger
```

### Build

```bash
cd ../ # spresense/
source ./spresense_env.sh
spresense make -j8 buildkernel
spresense make -j8
```

### Flash

```bash
./tools/flash.sh -c /dev/tty.SLAB_USBtoUART nuttx.spk
```


## Getting started with Arduino IDE

Open `logger/logger.ino` .
