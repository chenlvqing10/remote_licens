#
# the object parsed by Makefile.build
#
#all:pot_begin.bin pot_end.bin

pot_begin.bin:pot_begin.c
	arm-linux-gnueabi-gcc -c -marm -O -o pot_begin.o pot_begin.c
	arm-linux-gnueabi-ld -T pot_wdg.scf -g pot_begin.o -o pot_begin.axf
	arm-linux-gnueabi-objcopy -O binary pot_begin.axf pot_begin.bin
	arm-linux-gnueabi-objdump -d pot_begin.axf > pot_begin.lst

pot_end.bin:pot_end.c
	arm-linux-gnueabi-gcc -c -marm -O -o pot_end.o pot_end.c
	arm-linux-gnueabi-ld -T pot_wdg.scf -g pot_end.o -o pot_end.axf
	arm-linux-gnueabi-objcopy -O binary pot_end.axf pot_end.bin
	arm-linux-gnueabi-objdump -d pot_end.axf > pot_end.lst

clean:
	rm -f pot_begin.bin pot_begin.o pot_begin.axf pot_begin.lst
	rm -f pot_end.bin pot_end.o pot_end.axf pot_end.lst
