#
# Copyright (C) YuqiaoZhang(HanetakaChou)
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

# https://developer.android.com/ndk/guides/android_mk

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := OpenEXRCore

LOCAL_SRC_FILES := \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/attributes.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/base.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/channel_list.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/chunk.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/coding.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/compression.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/context.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/debug.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/decoding.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/encoding.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/float_vector.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_b44.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_b44_table.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_dwa.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_huf.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_piz.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_pxr24.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_rle.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_structs.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/internal_zip.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/memory.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/opaque.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/pack.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/parse_header.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/part.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/part_attr.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/preview.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/std_attr.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/string.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/string_vector.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/unpack.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/validation.c \
	$(LOCAL_PATH)/../src/lib/OpenEXRCore/write_header.c

LOCAL_CFLAGS :=

ifeq (armeabi-v7a,$(TARGET_ARCH_ABI))
LOCAL_ARM_MODE := arm
LOCAL_ARM_NEON := true
else ifeq (arm64-v8a,$(TARGET_ARCH_ABI))
LOCAL_CFLAGS +=
else ifeq (x86,$(TARGET_ARCH_ABI))
LOCAL_CFLAGS += -mf16c
LOCAL_CFLAGS += -mfma
LOCAL_CFLAGS += -mavx2
else ifeq (x86_64,$(TARGET_ARCH_ABI))
LOCAL_CFLAGS += -mf16c
LOCAL_CFLAGS += -mfma
LOCAL_CFLAGS += -mavx2
else
LOCAL_CFLAGS +=
endif

LOCAL_CFLAGS += -Wall
LOCAL_CFLAGS += -Werror=return-type

LOCAL_C_INCLUDES :=

LOCAL_CPPFLAGS := 
LOCAL_CPPFLAGS += -std=c++17

LOCAL_LDFLAGS :=
LOCAL_LDFLAGS += -Wl,--enable-new-dtags
LOCAL_LDFLAGS += -Wl,-rpath,\$$ORIGIN
LOCAL_LDFLAGS += -Wl,--version-script,$(LOCAL_PATH)/OpenEXRCore.map
LOCAL_LDFLAGS += -lm

LOCAL_STATIC_LIBRARIES :=
LOCAL_STATIC_LIBRARIES += deflate

include $(BUILD_SHARED_LIBRARY)
