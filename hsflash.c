#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_banner() {
    printf("██╗  ██╗███████╗   ███████╗██╗      █████╗ ███████╗██╗  ██╗\n");
    printf("██║  ██║██╔════╝   ██╔════╝██║     ██╔══██╗██╔════╝██║  ██║\n");
    printf("███████║███████╗   █████╗  ██║     ███████║███████╗███████║\n");
    printf("██╔══██║╚════██║   ██╔══╝  ██║     ██╔══██║╚════██║██╔══██║\n");
    printf("██║  ██║███████║██╗██║     ███████╗██║  ██║███████║██║  ██║\n");
    printf("╚═╝  ╚═╝╚══════╝╚═╝╚═╝     ╚══════╝╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝\n");
    printf("                  * HOTSPOT FLASH EZ *\n\n");
}

int file_exists(const char *filename) {
    return access(filename, F_OK) == 0;
}

void set_gpio(const char *pin, const char *mode, const char *level) {
    char cmd[128];
    snprintf(cmd, sizeof(cmd), "sudo raspi-gpio set %s %s %s", pin, mode, level);
    system(cmd);
}

int main() {
    print_banner();

    char BOOT0[8] = "20";
    char NRST[8] = "21";
    char BIN_FILE[256] = "";
    char SERIAL_PORT[256] = "/dev/ttyAMA0";
    char buffer[256];

    int stm32found = system("which stm32flash > /dev/null 2>&1") == 0;

    if (stm32found) {
        printf("[SUCCESS] stm32flash was found.\n\n");
    } else {
        printf("[WARN] stm32flash is not installed.\n");
        printf("Install stm32flash now? [Y/n]: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (buffer[0] == 'Y' || buffer[0] == 'y' || buffer[0] == '\n') {
            if (system("sudo apt update && sudo apt install -y stm32flash") != 0) {
                printf("[ERROR] Failed to install stm32flash. Exiting.\n");
                return 1;
            }
            stm32found = 1;
        } else {
            printf("[FATAL] stm32flash is required for flashing. Exiting.\n");
            return 1;
        }
    }

    printf("Enter BOOT0 GPIO pin [default 20]: ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] != '\n') sscanf(buffer, "%7s", BOOT0);

    printf("Enter NRST GPIO pin [default 21]: ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] != '\n') sscanf(buffer, "%7s", NRST);

    while (1) {
        printf("Enter firmware filename (e.g. dvm-firmware-hs-hat-dual.bin): ");
        fgets(BIN_FILE, sizeof(BIN_FILE), stdin);
        BIN_FILE[strcspn(BIN_FILE, "\n")] = 0;
        if (file_exists(BIN_FILE)) break;
        printf("[WARN] File '%s' not found. Please enter a valid filename.\n", BIN_FILE);
    }

    printf("Enter serial port [default /dev/ttyAMA0]: ");
    fgets(buffer, sizeof(buffer), stdin);
    if (buffer[0] != '\n') sscanf(buffer, "%255s", SERIAL_PORT);

    printf("\n===== Configuration Summary =====\n");
    printf("BOOT0 GPIO      : %s\n", BOOT0);
    printf("NRST GPIO       : %s\n", NRST);
    printf("BIN_FILE        : %s\n", BIN_FILE);
    printf("SERIAL_PORT     : %s\n", SERIAL_PORT);
    printf("stm32flash      : %s\n", stm32found ? "found" : "missing");
    printf("=================================\n\n");

    printf("Proceed with flashing? [Y/n]: ");
    fgets(buffer, sizeof(buffer), stdin);
    if (!(buffer[0] == 'Y' || buffer[0] == 'y' || buffer[0] == '\n')) {
        printf("[FATAL] Operation cancelled by user.\n");
        return 0;
    }

    printf("[INFO] Proceeding with firmware flash setup...\n");
    printf("[INFO] Setting BOOT0 high (GPIO%s)...\n", BOOT0);
    set_gpio(BOOT0, "op", "dh");

    printf("[INFO] Asserting reset (GPIO%s low)...\n", NRST);
    set_gpio(NRST, "op", "dl");
    usleep(100000);

    printf("[INFO] Releasing reset (GPIO%s high)...\n", NRST);
    set_gpio(NRST, "op", "dh");
    usleep(200000);

    printf("[INFO] Flashing firmware...\n");
    char flash_cmd[512];
    snprintf(flash_cmd, sizeof(flash_cmd), "sudo stm32flash -v -w %s -R %s", BIN_FILE, SERIAL_PORT);
    int flash_status = system(flash_cmd);

    printf("[INFO] Setting BOOT0 low (GPIO%s)...\n", BOOT0);
    set_gpio(BOOT0, "op", "dl");

    printf("[INFO] Resetting MCU to boot from flash...\n");
    set_gpio(NRST, "op", "dl");
    usleep(100000);
    set_gpio(NRST, "op", "dh");

    if (flash_status == 0) {
        printf("[SUCCESS] Flashing complete and STM32 restarted successfully.\n");
    } else {
        printf("[FAIL] Unable to flash firmware. Please review all connections, GPIO assignments, and serial ports.\n");
    }

    return 0;
}
