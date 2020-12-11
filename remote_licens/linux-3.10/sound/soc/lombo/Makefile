# Lombo Asoc platform Support
obj-$(CONFIG_SND_SOC_N7V0_I2S) += lombo_i2s.o
obj-$(CONFIG_SND_SOC_N7V0_I2S) += lombo_mock_dma.o
obj-$(CONFIG_SND_SOC_N7V1_I2S0) += lombo_i2s.o
obj-$(CONFIG_SND_SOC_N7V1_I2S0) += lombo_mock_dma.o
obj-$(CONFIG_SND_SOC_N7V1_I2S1) += lombo_i2s.o
obj-$(CONFIG_SND_SOC_N7V1_I2S1) += lombo_mock_dma.o
obj-$(CONFIG_SND_SOC_LOMBO_N9) += lombo_audio_dai.o

# N7 Asoc Support
I2S_SOURCE := $(srctree)/$(src)/csp/i2s/*.c
ifneq ($(wildcard $(I2S_SOURCE)), )
obj-$(CONFIG_SND_SOC_LOMBO) += csp/i2s/
else
obj-y += liblombo_i2s.o
$(obj)/liblombo_i2s.o: $(src)/liblombo_i2s
	cp $< $@
endif

AC_SOURCE := $(srctree)/$(src)/csp/ac/*.c
ifneq ($(wildcard $(AC_SOURCE)), )
obj-$(CONFIG_SND_SOC_LOMBO) += csp/ac/
else
obj-y += liblombo_ac.o
$(obj)/liblombo_ac.o: $(src)/liblombo_ac
	cp $< $@
endif
obj-$(CONFIG_SND_SOC_LOMBO_N7CODEC) += n7_codec.o
obj-$(CONFIG_SND_SOC_LOMBO_N7CODEC) += n7v1_codec.o
obj-$(CONFIG_SND_SOC_LOMBO_N7CODEC) += lombo_n7.o
obj-$(CONFIG_SND_SOC_LOMBO_N9) += n9_codec.o
obj-$(CONFIG_SND_SOC_LOMBO_N9) += lombo_n9.o