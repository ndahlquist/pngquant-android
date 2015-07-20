package com.nicdahlquist.pngquant.testapp;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

public class MainActivity extends Activity {

    private final PngQuantTest mPngQuantTest = new PngQuantTest();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = (Button) findViewById(R.id.button);
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                long startMillis = System.currentTimeMillis();
                mPngQuantTest.testPngQuant(MainActivity.this);
                long endMillis = System.currentTimeMillis();

                Log.i("MainActivity", "Processing took " + (endMillis - startMillis) + " millis.");
            }
        });
    }
}
