LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libpng
LOCAL_SRC_FILES := \
    libpng-android/jni/png.c \
    libpng-android/jni/pngerror.c \
    libpng-android/jni/pngget.c \
    libpng-android/jni/pngmem.c \
    libpng-android/jni/pngpread.c \
    libpng-android/jni/pngread.c \
    libpng-android/jni/pngrio.c \
    libpng-android/jni/pngrtran.c \
    libpng-android/jni/pngrutil.c \
    libpng-android/jni/pngset.c \
    libpng-android/jni/pngtest.c \
    libpng-android/jni/pngtrans.c \
    libpng-android/jni/pngwio.c \
    libpng-android/jni/pngwrite.c \
    libpng-android/jni/pngwtran.c \
    libpng-android/jni/pngwutil.c

LOCAL_SHARED_LIBRARIES := -lz
LOCAL_CFLAGS           := -O3
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/libpng-android/jni/.

include $(BUILD_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE           := libpngquant
LOCAL_SRC_FILES        := \
     pngquant/rwpng.c \
     pngquant/lib/blur.c \
     pngquant/lib/libimagequant.c \
     pngquant/lib/mediancut.c \
     pngquant/lib/mempool.c \
     pngquant/lib/nearest.c \
     pngquant/lib/pam.c \
     pngquant/lib/viter.c
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/pngquant/.
LOCAL_CFLAGS           := -std=c99 -O3
LOCAL_STATIC_LIBRARIES := libpng

include $(BUILD_STATIC_LIBRARY)



include $(CLEAR_VARS)
LOCAL_MODULE           := pngquantandroid
LOCAL_LDLIBS           := -lz \
                          -llog \
                          -lm
LOCAL_STATIC_LIBRARIES += libpng libpngquant
LOCAL_CFLAGS           := -std=c99 -O3
LOCAL_SRC_FILES        += native_glue.c \
                          pngquant.c

include $(BUILD_SHARED_LIBRARY)
