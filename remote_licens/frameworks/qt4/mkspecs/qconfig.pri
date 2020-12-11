#configuration
CONFIG +=  cross_compile shared def_files_disabled no_mocdepend release stl qt_no_framework
QT_ARCH = generic
QT_EDITION = OpenSource
QT_CONFIG +=  minimal-config small-config medium-config large-config full-config no-pkg-config accessibility shared embedded reduce_exports ipv6 clock-gettime clock-monotonic mremap getaddrinfo ipv6ifname getifaddrs inotify system-jpeg system-png png freetype system-zlib nis concurrent svg release

#versioning
QT_VERSION = 4.8.7
QT_MAJOR_VERSION = 4
QT_MINOR_VERSION = 8
QT_PATCH_VERSION = 7

#namespaces
QT_LIBINFIX = 
QT_NAMESPACE = 
QT_NAMESPACE_MAC_CRC = 

# sysroot
linux-arm-gnueabihf-g++ {
  QT_SYSROOT      += $$quote($$(VIRGO_PRODUCT_OUT)/sysroot)
  QMAKE_CFLAGS    += --sysroot=$$QT_SYSROOT
  QMAKE_CXXFLAGS  += --sysroot=$$QT_SYSROOT
  QMAKE_LFLAGS    += --sysroot=$$QT_SYSROOT
}

QMAKE_RPATHDIR += "$$(VIRGO_BUILD_TOP)/frameworks/qt4/lib"
