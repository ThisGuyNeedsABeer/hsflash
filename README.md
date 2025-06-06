### Overview

HS.Flash aka HS.Flash(EZ) stands for "Hotspot Flash made Easy." After I flashed my first MMDVM board for use with the [dvmhost project]([url](https://github.com/DVMProject/dvmhost)) I thought this would make my life substantially easier if I had to do create another hotspot. 

Special thanks to `nyaaaaaaatalie` for helping identify `BOOT0` and `NRST` pin assignments as well as the rest of the community for answering all of my questions over in `#dvm-support` on the dvmhost Discord.

**URL:** [https://dvmproject.io/]([url](https://dvmproject.io/))

**Discord:** [https://discord.dvmproject.io/]([url](https://discord.dvmproject.io/))

The C binary is the preferred flashing method over the shell script. I only added the script because it was where the idea started. There should not be a need for any special libraries. Orignally compiled on a Raspberry Pi 3B+.

### Prerequisites

1. stm32flash: If this package is not installed on Pi the script will offer to download and install it via `apt`.
2. dvm-firmware-hs-hat-dual.bin:
  
    a. Download the firmware: https://github.com/DVMProject/dvmfirmware-hs/releases
   
    b. Place the dvm firmware in the same directory that `hsflash` will be executed in.

### Compile
```Shell
gcc hsflash.c -o hsflash
```

### Usage

```Shell
pi@W25HS02:~ $ ./hsflash
██╗  ██╗███████╗   ███████╗██╗      █████╗ ███████╗██╗  ██╗
██║  ██║██╔════╝   ██╔════╝██║     ██╔══██╗██╔════╝██║  ██║
███████║███████╗   █████╗  ██║     ███████║███████╗███████║
██╔══██║╚════██║   ██╔══╝  ██║     ██╔══██║╚════██║██╔══██║
██║  ██║███████║██╗██║     ███████╗██║  ██║███████║██║  ██║
╚═╝  ╚═╝╚══════╝╚═╝╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝
                  * HOTSPOT FLASH EZ *

[WARN] stm32flash is not installed.
Install stm32flash now? [Y/n]: y

... apt output omitted...

Enter BOOT0 GPIO pin [default 20]: 20
Enter NRST GPIO pin [default 21]: 21
Enter firmware filename (e.g. dvm-firmware-hs-hat-dual.bin): dvm-firmware-hs-hat-dual.bin
Enter serial port [default /dev/ttyAMA0]: /dev/ttyAMA0

===== Configuration Summary =====
BOOT0 GPIO      : 20
NRST GPIO       : 21
BIN_FILE        : dvm-firmware-hs-hat-dual.bin
SERIAL_PORT     : /dev/ttyAMA0
stm32flash      : found
=================================

Proceed with flashing? [Y/n]: y
[INFO] Proceeding with firmware flash setup...
[INFO] Setting BOOT0 high (GPIO20)...
[ raspi-gpio is deprecated - try `pinctrl` instead ]
[INFO] Asserting reset (GPIO21 low)...
[INFO] Releasing reset (GPIO21 high)...
[INFO] Flashing firmware...
stm32flash 0.7

http://stm32flash.sourceforge.net/

Using Parser : Raw BINARY
Size         : 57248
Interface serial_posix: 57600 8E1
Version      : 0x22
Option 1     : 0x00
Option 2     : 0x00
Device ID    : 0x0410 (STM32F10xxx Medium-density)
- RAM        : Up to 20KiB  (512b reserved by bootloader)
- Flash      : Up to 128KiB (size first sector: 4x1024)
- Option RAM : 16b
- System RAM : 2KiB
Write to memory
Erasing memory
Wrote and verified address 0x0800dfa0 (100.00%) Done.

Resetting device...
Reset done.

[INFO] Setting BOOT0 low (GPIO20)...
[INFO] Resetting MCU to boot from flash...
[SUCCESS] Flashing complete and STM32 restarted successfully.
```
