package com.nicdahlquist.pngquant.testapp;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import com.nicdahlquist.pngquant.LibPngQuant;

public class PngQuantTest {

    private static final int IGNORED_COMPRESSION_VALUE = 100;

    public void testPngQuant(Context context) {
        try {
            File inputFile = getTempFile(context);
            writeBitmapToFile(decodeResource(context, R.drawable.test0), inputFile);

            File outputFile = getTempFile(context);

            new LibPngQuant().pngQuantFile(inputFile, outputFile);

            Log.i("MainActivity", "Output size " + ((float) outputFile.length() / inputFile.length()));
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    private File getTempFile(Context context) throws IOException {
        File outputDir = context.getExternalCacheDir();
        return File.createTempFile("temp", "png", outputDir);
    }

    private void writeBitmapToFile(Bitmap bitmap, File file) throws IOException {
        FileOutputStream fileOutputStream = new FileOutputStream(file);
        bitmap.compress(Bitmap.CompressFormat.PNG, IGNORED_COMPRESSION_VALUE, fileOutputStream);
        fileOutputStream.close();
    }

    private Bitmap decodeResource(Context context, int drawableRes) {
        return BitmapFactory.decodeResource(context.getResources(), drawableRes);
    }
}
