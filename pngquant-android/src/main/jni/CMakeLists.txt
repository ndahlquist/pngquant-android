cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPNG_ARM_NEON_OPT=0 -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPNG_ARM_NEON_OPT=0 -O3")

add_library(pngquant-android SHARED
    native_glue.c
    pngquant.c

    libpng-android/jni/png.c
    libpng-android/jni/pngerror.c
    libpng-android/jni/pngget.c
    libpng-android/jni/pngmem.c
    libpng-android/jni/pngpread.c
    libpng-android/jni/pngread.c
    libpng-android/jni/pngrio.c
    libpng-android/jni/pngrtran.c
    libpng-android/jni/pngrutil.c
    libpng-android/jni/pngset.c
    libpng-android/jni/pngtest.c
    libpng-android/jni/pngtrans.c
    libpng-android/jni/pngwio.c
    libpng-android/jni/pngwrite.c
    libpng-android/jni/pngwtran.c
    libpng-android/jni/pngwutil.c

    pngquant/rwpng.c
    pngquant/lib/blur.c
    pngquant/lib/libimagequant.c
    pngquant/lib/mediancut.c
    pngquant/lib/mempool.c
    pngquant/lib/nearest.c
    pngquant/lib/pam.c
    pngquant/lib/viter.c
)

include_directories(pngquant-android
    libpng-android/jni/
    pngquant/
)

target_link_libraries(pngquant-android
    log
    z
)