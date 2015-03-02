package com.example.Teichroboter;

import android.app.Service;
import android.content.Intent;
import android.os.*;
import android.widget.Toast;

import java.io.IOException;

/**
 * Created with IntelliJ IDEA.
 * User: mirko
 * Date: 18.02.13
 * Time: 12:59
 * To change this template use File | Settings | File Templates.
 */
public class TeichroboterService extends Service {
    /** Command to the service to display a message */
    static final int MSG_SET_LOG = 1;
    static final int MSG_SAY_HELLO = 2;
    static final int MSG_LED = 3;

    private Messenger logMessenger = null;

    /**
     * Handler of incoming messages from clients.
     */
    class IncomingHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case MSG_SET_LOG:
                    logMessenger = msg.replyTo;
                    break;
                case MSG_SAY_HELLO:
                    Toast.makeText(getApplicationContext(), "hello!", Toast.LENGTH_SHORT).show();
                    break;
                case MSG_LED:
                    if (andropodTransceiver.isOpen()) {
                        byte[] buffer = {(byte)msg.arg1};
                        try
                        {
                            andropodTransceiver.write(AndropodTransceiver.Type.APOD_TELTYPE_DATA, (short)1, buffer);
                        }
                        catch (IOException e)
                        {
                            Toast.makeText(getApplicationContext(), "Write-Fehler :"+e.getMessage(), Toast.LENGTH_SHORT).show();
                        }
                    }
                    break;
                default:
                    super.handleMessage(msg);
                    break;
            }
        }
    }

    /**
     * Target we publish for clients to send messages to IncomingHandler.
     */
    final Messenger mMessenger = new Messenger(new IncomingHandler());

    /**
     * When binding to the service, we return an interface to our messenger
     * for sending messages to the service.
     */
    @Override
    public IBinder onBind(Intent intent) {
        andropodTransceiver.resumeTransceiver();
        new TelnetSocket();
        return mMessenger.getBinder();
    }

    @Override
    public void onDestroy()
    {
        andropodTransceiver.suspendTransceiver();
        super.onDestroy();
    }

    private void log(String s)
    {
        if (logMessenger ==null) return;
        Message msg = new Message();
        Bundle bundle = new Bundle();
        bundle.putString("text", s);
        msg.setData(bundle);
        try {
            logMessenger.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
    }

    // Handler um Messages vom unterlagerten Thread zu verarbeiten
    private Handler receiveHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            Bundle bundle = msg.getData();
            log(bundle.getString("text"));
            super.handleMessage(msg);
        }
    };
    // the transceiver-object
    private AndropodTransceiver andropodTransceiver = new AndropodTransceiver(receiveHandler);
}
