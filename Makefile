AS := nasm
CC := gcc
LD := ld

ASFLAGS := -f elf
CFLAGS := -m32 -ffreestanding -fno-stack-protector -fno-builtin -Wall -Wextra -std=gnu99
LDFLAGS := -T source/link.ld -melf_i386

OBJECTS = \
	source/loader.o \
	source/kernel.o \
	source/io.o \
	drivers/framebuffer.o \
	drivers/pic.o \
	drivers/keyboard.o \
	drivers/interrupts.o \
	drivers/hardware_interrupt_enabler.o \
	drivers/interrupts_asm.o \
	drivers/interrupts_handlers.o

all: os.iso

source/loader.o: source/loader.asm
	$(AS) $(ASFLAGS) source/loader.asm -o source/loader.o

source/kernel.o: source/kernel.c
	$(CC) $(CFLAGS) -c source/kernel.c -o source/kernel.o

source/io.o: source/io.s
	$(AS) $(ASFLAGS) source/io.s -o source/io.o

drivers/framebuffer.o: drivers/framebuffer.c drivers/framebuffer.h
	$(CC) $(CFLAGS) -c drivers/framebuffer.c -o drivers/framebuffer.o

drivers/pic.o: drivers/pic.c drivers/pic.h
	$(CC) $(CFLAGS) -c drivers/pic.c -o drivers/pic.o

drivers/keyboard.o: drivers/keyboard.c drivers/keyboard.h
	$(CC) $(CFLAGS) -c drivers/keyboard.c -o drivers/keyboard.o

drivers/interrupts.o: drivers/interrupts.c drivers/interrupts.h
	$(CC) $(CFLAGS) -c drivers/interrupts.c -o drivers/interrupts.o

drivers/hardware_interrupt_enabler.o: drivers/hardware_interrupt_enabler.s
	$(AS) $(ASFLAGS) drivers/hardware_interrupt_enabler.s -o drivers/hardware_interrupt_enabler.o

drivers/interrupts_asm.o: drivers/interrupts_asm.s
	$(AS) $(ASFLAGS) drivers/interrupts_asm.s -o drivers/interrupts_asm.o

drivers/interrupts_handlers.o: drivers/interrupts_handlers.s
	$(AS) $(ASFLAGS) drivers/interrupts_handlers.s -o drivers/interrupts_handlers.o

iso/boot/kernel.elf: $(OBJECTS) source/link.ld
	mkdir -p iso/boot/grub
	$(LD) $(LDFLAGS) $(OBJECTS) -o iso/boot/kernel.elf

iso/boot/grub/stage2_eltorito:
	mkdir -p iso/boot/grub
	cp /opt/os/stage2_eltorito iso/boot/grub/stage2_eltorito

os.iso: iso/boot/kernel.elf iso/boot/grub/stage2_eltorito
	genisoimage -R \
		-b boot/grub/stage2_eltorito \
		-no-emul-boot \
		-boot-load-size 4 \
		-A os \
		-input-charset utf8 \
		-quiet \
		-boot-info-table \
		-o os.iso \
		iso

run: os.iso
	qemu-system-i386 -nographic -boot d -cdrom os.iso -m 32 -d cpu -D logQ.txt

clean:
	rm -f source/*.o drivers/*.o iso/boot/kernel.elf os.iso logQ.txt
