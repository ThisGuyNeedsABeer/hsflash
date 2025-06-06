#!/bin/bash
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
RED='\033[0;31m'
WHITE='\033[1;37m'
NC='\033[0m' 
echo -e "${GREEN}██╗  ██╗███████╗   ███████╗██╗      █████╗ ███████╗██╗  ██╗"
echo -e "${GREEN}██║  ██║██╔════╝   ██╔════╝██║     ██╔══██╗██╔════╝██║  ██║"
echo -e "${GREEN}███████║███████╗   █████╗  ██║     ███████║███████╗███████║"
echo -e "${GREEN}██╔══██║╚════██║   ██╔══╝  ██║     ██╔══██║╚════██║██╔══██║"
echo -e "${GREEN}██║  ██║███████║██╗██║     ███████╗██║  ██║███████║██║  ██║"
echo -e "${GREEN}╚═╝  ╚═╝╚══════╝╚═╝╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝"
echo -e "${WHITE}                  * HOTSPOT FLASH EZ *${NC}"
echo ""

# GPIO pin assignments
#BOOT0=20
#NRST=21
#BIN_FILE="dvm-firmware-hs-hat-dual.bin"
#SERIAL_PORT="/dev/ttyAMA0"

# Check for stm32flash
STM32FLASH_STATUS="missing"
if command -v stm32flash &> /dev/null; then
  STM32FLASH_STATUS="found"
  echo -e "${GREEN}[SUCCESS] stm32flash was found.${NC}"
else
  echo -e "${YELLOW}[WARN] stm32flash is not installed.${NC}"
  read -p "Install stm32flash now? [Y/n]: " choice
  choice=${choice:-Y}
  if [[ "$choice" =~ ^[Yy]$ ]]; then
    sudo apt update
    sudo apt install -y stm32flash
    if [[ $? -ne 0 ]]; then
      echo -e " {$RED}[ERROR] Failed to install stm32flash. Exiting.${NC}"
      exit 1
    fi
    STM32FLASH_STATUS="found"
  else
    echo -e "${RED}[FATAL] stm32flash was is required for flashing. Exiting.${NC}"
  fi
fi

read -p "Enter BOOT0 GPIO pin [default 20]: " BOOT0
BOOT0=${BOOT0:-20}

read -p "Enter NRST GPIO pin [default 21]: " NRST
NRST=${NRST:-21}

while [[ -z "$BIN_FILE" ]]; do
  read -p "Enter firmware filename (e.g. dvm-firmware-hs-hat-dual.bin): " BIN_FILE
  if [[ ! -f "$BIN_FILE" ]]; then
    echo -e "${YELLOW}[WARN] File '$BIN_FILE' not found. Please enter a valid filename.${NC}"
    BIN_FILE=""
  fi
done

read -p "Enter serial port [default /dev/ttyAMA0]: " SERIAL_PORT
SERIAL_PORT=${SERIAL_PORT:-/dev/ttyAMA0}

Summary
echo ""
echo "===== Configuration Summary ====="
echo "BOOT0 GPIO      : $BOOT0"
echo "NRST GPIO       : $NRST"
echo "BIN_FILE        : $BIN_FILE"
echo "SERIAL_PORT     : $SERIAL_PORT"
echo "stm32flash      : $STM32FLASH_STATUS"
echo "================================="
echo ""
Confirm or cancel
read -p "Proceed with flashing? [Y/n]: " confirm
confirm=${confirm:-Y}
if [[ ! "$confirm" =~ ^[Yy]$ ]]; then
  echo -e "${RED}[FATAL] Operation cancelled by user.${NC}"
  exit 0
fi

echo ""
echo "[INFO] Proceeding with firmware flash setup..."
echo "[INFO] Setting BOOT0 high (GPIO$BOOT0)..."
sudo raspi-gpio set $BOOT0 op dh

echo "[INFO] Asserting reset (GPIO$NRST low)..."
sudo raspi-gpio set $NRST op dl
sleep 0.1

echo "[INFO] Releasing reset (GPIO$NRST high)..."
sudo raspi-gpio set $NRST op dh
sleep 0.2  # Give time to enter bootloader

echo "[INFO] Flashing firmware..."
sudo stm32flash -v -w "$BIN_FILE" -R "$SERIAL_PORT"

FLASH_STATUS=$?

echo "[INFO] Setting BOOT0 low (GPIO$BOOT0)..."
sudo raspi-gpio set $BOOT0 op dl

echo "[INFO] Resetting MCU to boot from flash..."
sudo raspi-gpio set $NRST op dl
sleep 0.1
sudo raspi-gpio set $NRST op dh

if [ $FLASH_STATUS -eq 0 ]; then
    echo -e "${GREEN}[SUCCESS] Flashing complete and STM32 restarted successfully.${NC}"
else
    echo -e "${RED}[FAIL] Unable to flash firmware. Please review all connections, GPIO assignments, and serial ports.${NC}"
fi
