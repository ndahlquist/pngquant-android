cmake_minimum_required(VERSION 3.4.1)


# build application's shared lib
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -DANDROID_ARM_NEON=TRUE -DCV_CPU_HAS_SUPPORT_NEON")

add_library(pngquant-android SHARED

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
     native_glue.c
     pngquant.c
    )

#include_directories(multiview
#            ../../../../lib/src/
#            ../../../../lib/external/
#            ../../../../lib/external/opencv2/contrib/ximgproc/include)

#target_link_libraries(multiview
#                      android
#                      jnigraphics
#                      ${OpenCV_LIBS}
#                      log)