#
# the object parsed by Makefile.build
#
always  := dfs.bin
GCOV_PROFILE := n

# read LOMBO_DRAMFREQ_START's value from memory.h, and set it to DRAMFREQ_START
DRAMFREQ_START=`sed -n '/^\#define LOMBO_DRAMFREQ_START/p' $(srctree)/arch/arm/mach-lombo/include/mach/memory.h | awk '{print $$3}'`

KBUILD_CFLAGS := -g -c -nostdlib -march=armv7-a -marm -fno-unwind-tables -fno-asynchronous-unwind-tables -mlittle-endian -O0 -mno-unaligned-access
# KBUILD_CFLAGS := $(filter-out -O2, $(KBUILD_CFLAGS)) -Os
KBUILD_CFLAGS += -I. -I$(KDIR)/include -I$(KDIR)/arch/arm/mach-lombo/include
KBUILD_CFLAGS += -mfloat-abi=hard -mfpu=vfp
KBUILD_CPPFLAGS := $(filter-out -D__KERNEL__, $(KBUILD_CPPFLAGS))
LD_FLAGS = -static --defsym=DRAMFREQ_START=$(DRAMFREQ_START)

dfs-y := entry.o lombo_dram_main.o lombo_dram_dfs.o lombo_dram_printk.o lib1funcs.o common.o
dfs-y := $(addprefix $(obj)/,$(dfs-y))

$(obj)/dfs.bin: dfs_clean $(obj)/dfs.axf FORCE
	$(Q)$(CROSS_COMPILE)objcopy -O binary $(obj)/dfs.axf $(obj)/dfs.bin

$(obj)/dfs.axf: $(obj)/dfs.scf $(dfs-y) FORCE
	$(Q)$(CROSS_COMPILE)ld -T $(srctree)/$(obj)/dfs.scf $(LD_FLAGS) $(LIBS) -EL  $(dfs-y) -o $(obj)/dfs.axf -Map $(obj)/dfs.map
	$(Q)$(CROSS_COMPILE)objdump -D $(obj)/dfs.axf > $(obj)/dfs.lst

clean-files := $(dfs-y) dfs.bin dfs.axf dfs.lst dfs.map
dfs_clean: FORCE
	rm -rf $(clean-files)

FORCE:
.PHONY: FORCE
