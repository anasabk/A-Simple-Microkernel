CURRENT_PATH = $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
SEARCH_PATH = include
OBJ_PATH = obj

GPPPARAMS = -m32 -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -I$(SEARCH_PATH)
ASPARAMS = --32
LDPARAMS = -melf_i386

objects = obj/kernel.o \
		  obj/gdt.o \
		  obj/asm/loader.o \
		  obj/main.o \
		  obj/iolib.o \
          obj/hwcomm/port.o \
		  obj/asm/interruptstubs.o \
		  obj/hwcomm/interrupts.o \
		  obj/drivers/keyboard.o \
		#   obj/memorymanagement.o \
		#   obj/syscalls.o \
		#   obj/multitasking.o \

obj/%.o: src/%.cpp
	mkdir -p $(@D)
	g++ $(GPPPARAMS) -o $@ -c $<

obj/%.o: src/%.s
	mkdir -p $(@D)
	as $(ASPARAMS) -o $@ $<

microkernel.bin: linker.ld $(objects)
	ld $(LDPARAMS) -T $< -o $@ $(objects)

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
	VirtualBoxVM --startvm "My Micro Kernel" &

.PHONY: clean
clean:
	rm -rf obj microkernel.bin microkernel.iso
