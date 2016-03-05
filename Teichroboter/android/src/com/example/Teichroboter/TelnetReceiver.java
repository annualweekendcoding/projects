package com.example.Teichroboter;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

/**
 * Created with IntelliJ IDEA.
 * User: mirko
 * Date: 18.02.13
 * Time: 15:54
 * To change this template use File | Settings | File Templates.
 */
public class TelnetReceiver extends BroadcastReceiver {
    public void onReceive(Context context, Intent intent) {
      new TelnetSocket();
    }
}
