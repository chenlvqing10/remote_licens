KBUILD_CFLAGS := $(filter-out -O2, $(KBUILD_CFLAGS)) -Os
GCOV_PROFILE := n

SOURCE := $(srctree)/$(src)/deepsleep/power.c
ifneq ($(wildcard $(SOURCE)), )
obj-y += pm.o pm_asm.o prcm.o prcm_standby1.o
obj-$(CONFIG_LOMBO_PM_CHECK) += check_crc.o
$(obj)/pm_asm.o: $(obj)/deepsleep/suspend/suspend.bin $(obj)/deepsleep/resume/resume.bin $(obj)/deepsleep/standby/standby.bin FORCE

$(obj)/deepsleep/suspend/suspend.bin: FORCE
	$(Q)$(MAKE) $(build)=$(obj)/deepsleep/suspend clean
	$(Q)$(MAKE) $(build)=$(obj)/deepsleep/suspend

# make the compile serialized
$(obj)/deepsleep/resume/resume.bin: $(obj)/deepsleep/suspend/suspend.bin FORCE
	$(Q)$(MAKE) $(build)=$(obj)/deepsleep/resume clean
	$(Q)$(MAKE) $(build)=$(obj)/deepsleep/resume

$(obj)/deepsleep/standby/standby.bin: $(obj)/deepsleep/resume/resume.bin FORCE
	$(Q)$(MAKE) $(build)=$(obj)/deepsleep/standby clean
	$(Q)$(MAKE) $(build)=$(obj)/deepsleep/standby

FORCE:
.PHONY: FORCE

else
obj-y += libpm.o
$(obj)/libpm.o: $(src)/liblombo_pm
	cp $< $@
endif
