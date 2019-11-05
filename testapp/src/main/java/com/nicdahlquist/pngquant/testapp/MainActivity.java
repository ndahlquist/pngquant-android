package com.nicdahlquist.pngquant.testapp;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends Activity {

    private final PngQuantTest mPngQuantTest = new PngQuantTest();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final TextView textView = findViewById(R.id.textView);

        final Button button = findViewById(R.id.button);
        button.setOnClickListener(v -> {
            textView.setText("Processing...");

            new AsyncTask<Void, Void, Long>() {
                @Override
                protected Long doInBackground(Void... params) {
                    long startMillis = System.currentTimeMillis();
                    mPngQuantTest.testPngQuant(MainActivity.this);
                    long endMillis = System.currentTimeMillis();
                    return endMillis - startMillis;
                }

                @Override
                protected void onPostExecute(Long millis) {
                    textView.setText("Processing took " + millis + " millis.");
                }
            }.execute();
        });
    }
}
