# x86 Bare metal setup

This code base can be used to boot the kernel with grub on x86_64 platform

To build iso file :
        mkdir -p build/isofiles/boot/grub
        cp build/kernel.bin build/isofiles/boot/kernel.bin
        cp boot/grub.cfg build/isofiles/boot/grub
        grub-mkrescue -o hello.iso build/isofiles #2> /dev/null

Compile in 32 bit mode

To Invoke qemu -> qemu-system-x86_64 -cdrom build/hello.iso -vga std -s -serial file:serial.log -d int,cpu_reset -no-shutdown -no-reboot
