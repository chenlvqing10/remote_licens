MEDIA_DIR := $(call my-dir)
include $(MEDIA_DIR)/config/config.mk
include $(MEDIA_DIR)/config/dependency.mk
include $(call all-makefiles-under, $(MEDIA_DIR))
