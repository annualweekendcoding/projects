package com.example.Teichroboter;

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
        andropodTransceiver.Resume();
    }

    public void onButtonClick(View view) {
        textView = (TextView) findViewById(R.id.textView);
        if (andropodTransceiver.IsOpen()) {
            byte[] buffer = "On \r\n".getBytes();//{1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
            andropodTransceiver.Write(buffer, 5);
            textView.setText("On gesendet");
        }
    }

    public void offButtonClick(View view) {
        textView = (TextView) findViewById(R.id.textView);
        if (andropodTransceiver.IsOpen()) {
            byte[] buffer = "Off \r\n".getBytes(); //{1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            andropodTransceiver.Write(buffer, 6);
            textView.setText("Off gesendet");
        }
    }


}
