package com.nicdahlquist.pngquant;

import java.io.File;

public class LibPngQuant {

    public boolean pngQuantFile(File inputFile, File outputFile) {
        if (inputFile == null) throw new NullPointerException();
        if (!inputFile.exists()) throw new IllegalArgumentException();
        if (outputFile == null) throw new NullPointerException();
        if (outputFile.length() != 0) throw new IllegalArgumentException();

        String inputFilename = inputFile.getAbsolutePath();
        String outputFilename = outputFile.getAbsolutePath();

        //Using default quality from the pngquant windows bat file
        return nativePngQuantFile(inputFilename, outputFilename, 50, 100, 1);
    }

    public boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality) {
        if (inputFile == null) throw new NullPointerException();
        if (!inputFile.exists()) throw new IllegalArgumentException();
        if (outputFile == null) throw new NullPointerException();
        if (outputFile.length() != 0) throw new IllegalArgumentException();
        if (maxQuality < 0 || maxQuality > 100) throw new IllegalArgumentException();
        if (minQuality < 0 || minQuality > 100) throw new IllegalArgumentException();
        if (maxQuality < minQuality) throw new IllegalArgumentException();

        String inputFilename = inputFile.getAbsolutePath();
        String outputFilename = outputFile.getAbsolutePath();

        //Use lowest speed to get the best quality
        return nativePngQuantFile(inputFilename, outputFilename, minQuality, maxQuality, 1);
    }

    public boolean pngQuantFile(File inputFile, File outputFile, int minQuality, int maxQuality, int speed) {
        if (inputFile == null) throw new NullPointerException();
        if (!inputFile.exists()) throw new IllegalArgumentException();
        if (outputFile == null) throw new NullPointerException();
        if (outputFile.length() != 0) throw new IllegalArgumentException();
        if (maxQuality < 0 || maxQuality > 100) throw new IllegalArgumentException();
        if (minQuality < 0 || minQuality > 100) throw new IllegalArgumentException();
        if (maxQuality < minQuality) throw new IllegalArgumentException();
        if (speed < 1 || speed > 11) throw new IllegalArgumentException();

        String inputFilename = inputFile.getAbsolutePath();
        String outputFilename = outputFile.getAbsolutePath();

        return nativePngQuantFile(inputFilename, outputFilename, minQuality, maxQuality, speed);
    }

    static {
        System.loadLibrary("pngquantandroid");
    }

    private static native boolean nativePngQuantFile(String inputFilename, String outputFilename, int minQuality, int maxQuality, int speed);

}
