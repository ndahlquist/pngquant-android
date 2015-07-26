#!/usr/bin/env bash
# Downloads and installs the Android Native Development Kit.

set -e

if [ ! -d android-ndk-r10e ]
then
    echo "Creating a local install of the Android NDK."
    if [ "$(uname)" == "Darwin" ]
    then
        wget http://dl.google.com/android/ndk/android-ndk-r10e-darwin-x86_64.bin -O ndk.bin
    else
        if [ "$(uname -m)" == "x86_64" ]
        then
            wget http://dl.google.com/android/ndk/android-ndk-r10e-linux-x86_64.bin -O ndk.bin
        else
            wget http://dl.google.com/android/ndk/android-ndk-r10e-linux-x86.bin -O ndk.bin
        fi
    fi

    chmod a+x ndk.bin
    ./ndk.bin -y | grep -v Extracting
    rm ndk.bin
fi
