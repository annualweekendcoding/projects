package com.example.Teichroboter;

import java.io.IOException;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity {

    // Handler um Messages vom unterlagerten Thread zu verarbeiten
    private Handler receiveHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            Bundle bundle = msg.getData();
            addLog(bundle.getString("text"));
            super.handleMessage(msg);
        }
    };
    // the transceiver-object
    private AndropodTransceiver andropodTransceiver = new AndropodTransceiver(receiveHandler);

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
        if (andropodTransceiver.isOpen()) {
            byte[] buffer = {1};
            try
            {
//                andropodTransceiver.write_raw("Hallo".getBytes());
                andropodTransceiver.write(AndropodTransceiver.Type.APOD_TELTYPE_DATA, (short)1, buffer);
                addLog("On gesendet");
            }
            catch (IOException e)
            {
                addLog("On senden fehlgeschlagen: " + e.getLocalizedMessage());
            }
        }
    }

    public void offButtonClick(View view)
    {
        if (andropodTransceiver.isOpen()) {
            byte[] buffer = {0};
            try
            {
                andropodTransceiver.write(AndropodTransceiver.Type.APOD_TELTYPE_DATA, (short)1, buffer);
                addLog("Off gesendet");
            }
            catch (IOException e)
            {
                addLog("Off senden fehlgeschlagen: " + e.getLocalizedMessage());
            }
        }
    }

    public void toggleButtonClick(View view)
    {
        if (andropodTransceiver.isOpen()) {
            byte[] buffer = {2};
            try
            {
                andropodTransceiver.write(AndropodTransceiver.Type.APOD_TELTYPE_DATA, (short)1, buffer);
                addLog("Toggle gesendet");
            }
            catch (IOException e)
            {
                addLog("Toggle senden fehlgeschlagen: " + e.getLocalizedMessage());
            }
        }
    }

    public void flashButtonClick(View view)
    {
        if (andropodTransceiver.isOpen()) {
            byte[] buffer = {3};
            try
            {
                andropodTransceiver.write(AndropodTransceiver.Type.APOD_TELTYPE_DATA, (short)1, buffer);
                addLog("Flash gesendet");
            }
            catch (IOException e)
            {
                addLog("Flash senden fehlgeschlagen: " + e.getLocalizedMessage());
            }
        }
    }

    public void addLog(String text)
    {
        TextView textView = (TextView) findViewById(R.id.textView);
        ScrollView scrollView = (ScrollView) findViewById(R.id.scrollView);
        textView.append(text+"\n");
        scrollView.fullScroll(View.FOCUS_DOWN);
    }

}
