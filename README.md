The C binary is the preferred flashing method over the shell script. I only added the script because it was where the idea started.

There should not be a need for any special libraries. Orignally compiled on a Raspberry Pi 3B+.

```Shell
gcc hsflash.c -o hsflash
```
