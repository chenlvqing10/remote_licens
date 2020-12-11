PRODUCT_COPY_FILES += \
	frameworks/tts/8k/data/eng_dict.txt:rootfs/usr/share/tts/eng_dict.txt \
	frameworks/tts/8k/data/pinyin_dict.txt:rootfs/usr/share/tts/pinyin_dict.txt \
	frameworks/tts/8k/data/surname_pinyin_dural.txt:rootfs/usr/share/tts/surname_pinyin_dural.txt \
	frameworks/tts/8k/data/surname_pinyin_single.txt:rootfs/usr/share/tts/surname_pinyin_single.txt \
	frameworks/tts/8k/data/yinsu.txt:rootfs/usr/share/tts/yinsu.txt \
	frameworks/tts/8k/data/dst_temperature.txt:rootfs/usr/share/tts/dst_temperature.txt

PRODUCT_COPY_DIRS += \
	frameworks/tts/8k/data/pinyinRaw:rootfs/usr/share/tts/pinyinRaw \
	frameworks/tts/8k/data/greetingRaw:rootfs/usr/share/tts/greetingRaw \
	frameworks/tts/8k/data/engRaw:rootfs/usr/share/tts/engRaw

#tts lib
-include frameworks/tts/8k/lib/$(TARGET_TOOLS_ARCH)/lib.mk