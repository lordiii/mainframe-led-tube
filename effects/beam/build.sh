#/bin/bash

arm-none-eabi-gcc -static -c -fno-exceptions -ffreestanding -mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -O3 -o fx.o src/fx.cpp
arm-none-eabi-gcc -N -static -Tlinker.ld -mthumb -nodefaultlibs -nostdlib -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-d16 -flto -Wl,--defsym,SYSTEM=0x20010000+64*1024 -O3 -o effect fx.o

