PRODUCT_PACKAGES += \
    debuggerd
    
PRODUCT_FORCE_COPY_FILES += \
    device/n7v1/boards/common/rootfs/etc/init.d/S30debuggerd:rootfs/etc/init.d/S30debuggerd

BUILD_NUMBER := v2.0rc4
