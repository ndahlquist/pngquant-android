package com.nicdahlquist.pngquant.testapp;

import android.content.Context;
import android.os.AsyncTask;

import androidx.annotation.NonNull;

import com.nicdahlquist.pngquant.LibPngQuant;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.Objects;
import java.util.concurrent.CompletableFuture;

class PngQuantTask extends AsyncTask<Void, Void, Long> {

    private final WeakReference<Context> mContext;
    private final File mInputPngFile;
    private final CompletableFuture<Long> mCompletableFuture;

    PngQuantTask(@NonNull Context context, @NonNull File inputFile, @NonNull CompletableFuture<Long> completableFuture) {
        mContext = new WeakReference<>(context);
        mInputPngFile = Objects.requireNonNull(inputFile);
        mCompletableFuture = Objects.requireNonNull(completableFuture);
    }

    @Override
    protected Long doInBackground(Void... voids) {
        Context context = mContext.get();
        if (context == null) return -1L;

        File outputFile = PngUtils.getTempFile(context);

        long startMillis = System.currentTimeMillis();
        new LibPngQuant().pngQuantFile(mInputPngFile, outputFile);
        long endMillis = System.currentTimeMillis();

        return endMillis - startMillis;
    }

    @Override
    protected void onPostExecute(Long l) {
        super.onPostExecute(l);

        mCompletableFuture.complete(l);
    }

}
