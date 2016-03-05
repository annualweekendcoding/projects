package com.example.Teichroboter;

import android.app.Activity;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.*;
import android.view.View;
import android.widget.ScrollView;
import android.widget.TextView;

public class MainActivity extends Activity {
    /** Messenger for communicating with the service. */
    Messenger mService = null;

    // Handler um Messages vom Service zu verarbeiten
    private Handler logHandler = new Handler()
    {
        public void handleMessage(Message msg)
        {
            Bundle bundle = msg.getData();
            addLog(bundle.getString("text"));
            super.handleMessage(msg);
        }
    };

    /**
     * Class for interacting with the main interface of the service.
     */
    private ServiceConnection mConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder service) {
            // This is called when the connection with the service has been
            // established, giving us the object we can use to
            // interact with the service.  We are communicating with the
            // service using a Messenger, so here we get a client-side
            // representation of that from the raw IBinder object.
            mService = new Messenger(service);
            Message msg = Message.obtain(null, TeichroboterService.MSG_SET_LOG, 0, 0);
            msg.replyTo = new Messenger(logHandler);
            try {
                mService.send(msg);
            } catch (RemoteException e) {
                e.printStackTrace();
            }
        }
        public void onServiceDisconnected(ComponentName className) {
            // This is called when the connection with the service has been
            // unexpectedly disconnected -- that is, its process crashed.
            mService = null;
        }
    };

    public void sayHello(View v) {
        if (mService==null) return;
        // Create and send a message to the service, using a supported 'what' value
        Message msg = Message.obtain(null, TeichroboterService.MSG_SAY_HELLO, 0, 0);
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onStart() {
        super.onStart();
        // Bind to the service
        bindService(new Intent(this, TeichroboterService.class), mConnection, Context.BIND_AUTO_CREATE);
    }

    @Override
    protected void onStop() {
        super.onStop();
        // Unbind from the service
        unbindService(mConnection);
    }

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
    }

    public void onButtonClick(View view) {
        if (mService==null) return;
        // Create and send a message to the service, using a supported 'what' value
        Message msg = Message.obtain(null, TeichroboterService.MSG_LED, 1, 0);
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void offButtonClick(View view)
    {
        if (mService==null) return;
        // Create and send a message to the service, using a supported 'what' value
        Message msg = Message.obtain(null, TeichroboterService.MSG_LED, 0, 0);
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void toggleButtonClick(View view)
    {
        if (mService==null) return;
        // Create and send a message to the service, using a supported 'what' value
        Message msg = Message.obtain(null, TeichroboterService.MSG_LED, 2, 0);
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }

    public void flashButtonClick(View view)
    {
        if (mService==null) return;
        // Create and send a message to the service, using a supported 'what' value
        Message msg = Message.obtain(null, TeichroboterService.MSG_LED, 3, 0);
        try {
            mService.send(msg);
        } catch (RemoteException e) {
            e.printStackTrace();
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
