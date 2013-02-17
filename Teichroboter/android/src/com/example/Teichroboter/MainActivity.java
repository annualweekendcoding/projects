package com.example.Teichroboter;

import java.io.IOException;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends Activity {

    // the transceiver-object
    private AndropodTransceiver andropodTransceiver = new AndropodTransceiver();
    private TextView textView;

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        andropodTransceiver.resumeTransceiver();
    }

    @Override
    protected void onDestroy()
    {
        andropodTransceiver.suspendTransceiver();
        super.onDestroy();
    }

    public void onButtonClick(View view) {
        textView = (TextView) findViewById(R.id.textView);
        if (andropodTransceiver.isOpen()) {
            byte[] buffer = {1};
            try
            {
                andropodTransceiver.write(AndropodTransceiver.Type.APOD_TELTYPE_DATA, (short)1, buffer);
                textView.setText("On gesendet");
            }
            catch (IOException e)
            {
                textView.setText("On senden fehlgeschlagen: " + e.getLocalizedMessage());
            }
        }
    }

    public void offButtonClick(View view)
    {
        textView = (TextView) findViewById(R.id.textView);
        if (andropodTransceiver.isOpen()) {
            byte[] buffer = {0};
            try
            {
                andropodTransceiver.write(AndropodTransceiver.Type.APOD_TELTYPE_DATA, (short)1, buffer);
                textView.setText("Off gesendet");
            }
            catch (IOException e)
            {
                textView.setText("Off senden fehlgeschlagen: " + e.getLocalizedMessage());
            }
        }
    }


}
