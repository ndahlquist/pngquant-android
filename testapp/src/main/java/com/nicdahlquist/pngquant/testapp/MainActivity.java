package com.nicdahlquist.pngquant.testapp;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;
import java.util.concurrent.CompletableFuture;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final TextView textView = findViewById(R.id.textView);

        File file = PngUtils.createTestPngFile(this);

        final Button button = findViewById(R.id.button);
        button.setOnClickListener(v -> {
            textView.setText("Processing...");

            CompletableFuture<Long> future = new CompletableFuture<>();
            new PngQuantTask(this, file, future).execute();
            future.thenAccept(l -> textView.setText("Processing completed in " + l + " millis"));
        });
    }
}
