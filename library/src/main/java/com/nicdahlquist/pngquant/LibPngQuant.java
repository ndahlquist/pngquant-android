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

        return nativePngQuantFile(inputFilename, outputFilename);
    }

    static {
        System.loadLibrary("pngquantandroid");
    }

    private static native boolean nativePngQuantFile(String inputFilename, String outputFilename);

}
