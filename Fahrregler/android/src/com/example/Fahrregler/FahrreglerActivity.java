package com.example.Fahrregler;

import java.io.FileDescriptor;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

import android.app.Activity;
//import android.app.Fragment;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
//import android.hardware.usb.UsbAccessory;
//import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.ParcelFileDescriptor;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.View;
import android.widget.TextView;
import com.android.future.usb.UsbAccessory;
import com.android.future.usb.UsbManager;

public class FahrreglerActivity extends Activity implements Runnable, ViewLogger
{
    private static final String TAG = "Fahrregler";

    private static final String ACTION_USB_PERMISSION = "com.google.android.DemoKit.action.USB_PERMISSION";

    private UsbManager mUsbManager;
    private PendingIntent mPermissionIntent;
    private boolean mPermissionRequestPending;

    UsbAccessory mAccessory;
    ParcelFileDescriptor mFileDescriptor;
    FileInputStream mInputStream;
    FileOutputStream mOutputStream;

    private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver()
    {
        @Override
        public void onReceive(Context context, Intent intent)
        {
            addLog("onReceive");
            String action = intent.getAction();
            if (ACTION_USB_PERMISSION.equals(action))
            {
                synchronized (this)
                {
   					UsbAccessory accessory = UsbManager.getAccessory(intent);
//                    UsbAccessory accessory = (UsbAccessory)intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false))
                    {
                        openAccessory(accessory);
                    }
                    else
                    {
                        Log.d(TAG, "permission denied for accessory " + accessory);
                    }
                    mPermissionRequestPending = false;
                }
            }
            else if (UsbManager.ACTION_USB_ACCESSORY_DETACHED.equals(action))
            {
                UsbAccessory accessory = UsbManager.getAccessory(intent);
//                UsbAccessory accessory = (UsbAccessory)intent.getParcelableExtra(UsbManager.EXTRA_ACCESSORY);
                if (accessory != null && accessory.equals(mAccessory))
                {
                    closeAccessory();
                }
            }
        }
    };

//    public class UsbAccessoryFragment extends Fragment
//    {
//        @Override
//        public void onActivityCreated(final Bundle savedInstanceState)
//        {
//            super.onActivityCreated(savedInstanceState);
//        }
//    }

    /**
     * Called when the activity is first created.
     */
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        mUsbManager = UsbManager.getInstance(this);
//        mUsbManager = (UsbManager)getSystemService(Context.USB_SERVICE);
        mPermissionIntent = PendingIntent.getBroadcast(this, 0, new Intent(ACTION_USB_PERMISSION), 0);
        IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        filter.addAction(UsbManager.ACTION_USB_ACCESSORY_DETACHED);
        registerReceiver(mUsbReceiver, filter);

        if (getLastNonConfigurationInstance() != null)
        {
            mAccessory = (UsbAccessory)getLastNonConfigurationInstance();
            openAccessory(mAccessory);
        }

        setContentView(R.layout.main);

        final TextView logView = (TextView)findViewById(R.id.logView);
        logView.setMovementMethod(new ScrollingMovementMethod());
    }

    @Override
    public void onResume()
    {
        super.onResume();

        Intent intent = getIntent();
        if (mInputStream != null && mOutputStream != null)
        {
            addLog("Already open");
            return;
        }

        UsbAccessory[] accessories = mUsbManager.getAccessoryList();
        UsbAccessory accessory = (accessories == null ? null : accessories[0]);
        if (accessory != null)
        {
            if (mUsbManager.hasPermission(accessory))
            {
                openAccessory(accessory);
                addLog((mAccessory != null) ? "Accessory opened" : "Accessory not opened");
            }
            else
            {
                synchronized (mUsbReceiver)
                {
                    if (!mPermissionRequestPending)
                    {
                        addLog("Request USB permission");
                        mUsbManager.requestPermission(accessory, mPermissionIntent);
                        mPermissionRequestPending = true;
                    }
                }
            }
        }
        else
        {
            Log.d(TAG, "mAccessory is null");
            addLog("mAccessory is null");
        }
    }

    @Override
    public void onPause()
    {
        super.onPause();
        closeAccessory();
    }

    @Override
    public void onDestroy()
    {
        unregisterReceiver(mUsbReceiver);
        super.onDestroy();
    }

    private void openAccessory(UsbAccessory accessory)
    {
        addLog("Open accessory");
        mFileDescriptor = mUsbManager.openAccessory(accessory);
        if (mFileDescriptor != null)
        {
            mAccessory = accessory;
            FileDescriptor fd = mFileDescriptor.getFileDescriptor();
//            mInputStream = new FileInputStream(fd);
            mOutputStream = new FileOutputStream(fd);
//            Thread thread = new Thread(null, this, "DemoKit");
//            thread.start();
            Log.d(TAG, "accessory opened");
            addLog("Accessory opened");
            enableControls(true);
        }
        else
        {
            Log.d(TAG, "accessory open fail");
            addLog("Accessory open failed");
        }
    }

    private void closeAccessory()
    {
        addLog("Close accessory");
        enableControls(false);

        try
        {
            if (mFileDescriptor != null)
            {
                mFileDescriptor.close();
            }
        }
        catch (IOException e)
        {
        }
        finally
        {
            mFileDescriptor = null;
            mAccessory = null;
            addLog("Accessory closed");
        }
    }

    protected void enableControls(boolean enable)
    {
    }

    public void onButtonClick(final View view)
    {
        switch (view.getId())
        {
            case R.id.upButton:
                sendCommand((byte)0, (byte)0, 1);
                break;
            case R.id.downButton:
                sendCommand((byte)0, (byte)1, 1);
                break;
            case R.id.directionButton:
                sendCommand((byte)0, (byte)2, 1);
                break;
            case R.id.haltButton:
                sendCommand((byte)0, (byte)3, 1);
                break;
        }
    }

    public void sendCommand(byte command, byte target, int value)
    {
        Log.d(TAG, "sendCommand");
        addLog("Send command");
        byte[] buffer = new byte[3];
        if (value > 255)
        {
            value = 255;
        }

        buffer[0] = command;
        buffer[1] = target;
        buffer[2] = (byte)value;
        if (mOutputStream != null && buffer[1] != -1)
        {
            try
            {
                mOutputStream.write(buffer);
                Log.d(TAG, "command sent");
                addLog("Command sent");
            }
            catch (IOException e)
            {
                Log.e(TAG, "write failed", e);
                addLog("Sending command failed");
            }
        }
        else
        {
            addLog("mOutputStream null");
        }
    }

    public void run()
    {
    }

    public void addLog(final String text)
    {
        final TextView logView = (TextView)findViewById(R.id.logView);
        if (logView != null)
        {
            logView.append(text + "\n");
//            logView.scrollTo(0, logView.getHeight());
        }
    }
}
