.RECIPEPREFIX := >
CXX      = g++
CXXFLAGS = -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -m32 -fno-pie -nostdlib -O2 -Wall -Wextra

os.bin: boot.o kernel.o
>$(CXX) -m32 -nostdlib -no-pie -T linker.ld -o os.bin -Wl,--build-id=none boot.o kernel.o

boot.o: boot.asm
>nasm -f elf32 boot.asm -o boot.o

kernel.o: kernel.cpp
>$(CXX) $(CXXFLAGS) -c kernel.cpp -o kernel.o

run: os.bin
>qemu-system-i386 -display gtk -kernel os.bin

clean:
>rm -f *.o os.bin
