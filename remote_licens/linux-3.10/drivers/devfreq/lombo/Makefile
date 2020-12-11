KBUILD_CFLAGS := $(filter-out -O2, $(KBUILD_CFLAGS)) -Os
GCOV_PROFILE := n

$(obj)/dfs.o: $(obj)/dfs/dfs.bin
obj-y	+= lombo_dramfreq.o dfs.o

$(obj)/dfs/dfs.bin: FORCE
	$(Q)$(MAKE) $(build)=$(obj)/dfs

FORCE:
.PHONY: FORCE

