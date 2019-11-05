package com.nicdahlquist.pngquant.testapp;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import androidx.annotation.NonNull;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import static android.graphics.Bitmap.CompressFormat.PNG;

class PngUtils {

    private PngUtils() {}

    static File createTestPngFile(@NonNull Context context) {
        Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), R.drawable.test0);
        File file = getTempFile(context);

        try (OutputStream outputStream = new FileOutputStream(file)) {
            bitmap.compress(PNG, 100, outputStream);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        return file;
    }

    static File getTempFile(@NonNull Context context) {
        try {
            File outputDir = context.getExternalCacheDir();
            return File.createTempFile("temp", "png", outputDir);
        } catch (IOException e) {
           throw new RuntimeException(e);
        }
    }

}
