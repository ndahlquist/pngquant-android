
## pngquant-android
[![Circle CI](https://circleci.com/gh/ndahlquist/pngquant-android.svg?style=svg)](https://circleci.com/gh/ndahlquist/pngquant-android)
[![Download](https://api.bintray.com/packages/ndahlquist/maven/pngquant-android/images/download.svg) ](https://bintray.com/ndahlquist/maven/pngquant-android/_latestVersion)

[pngquant](https://pngquant.org/) with basic Java bindings for Android.
![pngquant](https://pngquant.org/pngquant-logo.png)

### Usage
In your build.gradle:
```groovy
dependencies {
    compile 'com.ndahlquist:pngquant-android:0.2'
}

```

In your Android app:
**Optimizing the image with default settings:** 
```java
File inputPngFile = getYourPng();
File outputPngFile = getOutputFile();
new LibPngQuant().pngQuantFile(inputFile, outputFile);
```
**Alternative commands with more options:**
```java
new LibPngQuant().pngQuantFile(inputFile, outputFile, minQuality, maxQuality);
```
```java
new LibPngQuant().pngQuantFile(inputFile, outputFile, minQuality, maxQuality, speed);
```
```java
new LibPngQuant().pngQuantFile(inputFile, outputFile, minQuality, maxQuality, speed, floydDitherAmount);
```

### Building
- This project includes git submodules; please make sure to `git clone --recursive`. Alternatively, `git submodule init; git submodule update`.
- On OS X, please install wget: `brew install wget`.
- From the top-level directory, run `./gradlew installDebug`. This will download all dependencies, build the library, and install a test application to a connected Android device.
