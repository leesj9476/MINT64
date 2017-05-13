#!/bin/bash
qemu-system-x86_64 -L . -m 64 -drive file=/home/lsuj/MINT64/Disk.img,format=raw,if=floppy -localtime -M pc
