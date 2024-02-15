#
# Copyright (C) 2024 The LineageOS Project
#           (C) 2024 Paranoid Android
#
# SPDX-License-Identifier: Apache-2.0
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := audio_amplifier.tas2562
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := tas2562.c
LOCAL_VENDOR_MODULE := true

LOCAL_C_INCLUDES += \
    $(call include-path-for, audio-route) \
    $(call include-path-for, audio-utils) \
    $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/techpack/audio/include \
    external/tinycompress/include \
    vendor/qcom/opensource/audio-hal/primary-hal/hal \
    vendor/qcom/opensource/audio-hal/primary-hal/hal/audio_extn \
    vendor/qcom/opensource/audio-hal/primary-hal/hal/msm8974

LOCAL_HEADER_LIBRARIES += \
    qti_kernel_headers \
    libhardware_headers

LOCAL_SHARED_LIBRARIES += \
    audio.primary.$(TARGET_BOARD_PLATFORM) \
    libcutils \
    liblog \
    libtinyalsa

include $(BUILD_SHARED_LIBRARY)
