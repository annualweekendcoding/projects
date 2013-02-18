package com.example.Fahrregler;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

public class UsbAccessoryActivity extends Activity
{
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        final Intent intent = new Intent(this, FahrreglerActivity.class);
        startActivity(intent);
    }
}