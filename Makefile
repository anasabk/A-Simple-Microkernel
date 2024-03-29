SEARCH_PATH = include

GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -I$(SEARCH_PATH)
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/gdt.o \
		  obj/asm/loader.o \
		  obj/main.o \
		  obj/iolib.o \
          obj/hwcomm/port.o \
		  obj/asm/interruptstubs.o \
		  obj/hwcomm/interrupts.o \
		  obj/drivers/mouse.o \
		  obj/drivers/keyboard.o \
		  obj/multitasking/process.o \
		  obj/multitasking/processmanager.o \
		  obj/memory/memmanager.o \
		  obj/kernel.o \
		  obj/common/tests.o

flavors = obj/flavor1.o \
		  obj/flavor2.o \
		  obj/flavor3.o
		  
flavor ?= flavor1

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

microkernel.bin: linker.ld $(objects) obj/$(flavor).o
	ld $(LDPARAMS) -T $< -o $@ $(objects) obj/$(flavor).o

install: microkernel.bin
	sudo cp $< /boot/microkernel.bin

microkernel.iso: microkernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' 						>> iso/boot/grub/grub.cfg
	echo 'set default=0' 						>> iso/boot/grub/grub.cfg
	echo '' 									>> iso/boot/grub/grub.cfg
	echo 'menuentry "My Operating System" {' 	>> iso/boot/grub/grub.cfg
	echo '	multiboot /boot/microkernel.bin' 	>> iso/boot/grub/grub.cfg
	echo '	boot' 								>> iso/boot/grub/grub.cfg
	echo '}' 									>> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

run: microkernel.iso
	(killall VirtualBoxVM && sleep 1) || true
	VirtualBoxVM --startvm "My Micro Kernel" --dbg &

.PHONY: clean
clean:
	rm -rf obj microkernel.bin microkernel.iso
