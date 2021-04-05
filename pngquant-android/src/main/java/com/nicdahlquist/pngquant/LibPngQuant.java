package com.nicdahlquist.pngquant;

import java.io.File;

public class LibPngQuant {

    public boolean pngQuantFile(File inputFile, File outputFile) {
        //Use default quality in the windows batch file bundled with pngQuant
        return pngQuantFile(inputFile, outputFile,50,100);
    }

    public boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality) {
        //Use lowest speed to get the best quality
        return pngQuantFile(inputFile, outputFile, minQuality, maxQuality, 1);
    }

    public boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality, int speed) {
        //Use the default dither value.
        return pngQuantFile(inputFile, outputFile, minQuality, maxQuality, speed, 1f);
    }

    public boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality, int speed, float floydDitherAmount) {
        if (inputFile == null) throw new NullPointerException();
        if (!inputFile.exists()) throw new IllegalArgumentException();
        if (outputFile == null) throw new NullPointerException();
        if (outputFile.length() != 0) throw new IllegalArgumentException();
        if (maxQuality < 0 || maxQuality > 100) throw new IllegalArgumentException();
        if (minQuality < 0 || minQuality > 100) throw new IllegalArgumentException();
        if (maxQuality < minQuality) throw new IllegalArgumentException();
        if (speed < 1 || speed > 11) throw new IllegalArgumentException();
        if (floydDitherAmount < 0f || floydDitherAmount > 1f) throw new IllegalArgumentException();

        String inputFilename = inputFile.getAbsolutePath();
        String outputFilename = outputFile.getAbsolutePath();

        return nativePngQuantFile(inputFilename, outputFilename, minQuality, maxQuality, speed, floydDitherAmount);
    }

    static {
        System.loadLibrary("pngquantandroid");
    }

    private static native boolean nativePngQuantFile(String inputFilename, String outputFilename, int minQuality, int maxQuality, int speed, float floydDitherAmount);

}
