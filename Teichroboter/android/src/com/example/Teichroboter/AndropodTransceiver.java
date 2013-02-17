package com.example.Teichroboter;

import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created with IntelliJ IDEA.
 * User: mirko
 * Date: 16.02.13
 * Time: 20:13
 * To change this template use File | Settings | File Templates.
 */
public class AndropodTransceiver extends Thread {
    private ServerSocket AndropodSocket = null;
    private Socket AndropodInterface = null;
    private BufferedInputStream AndropodReader = null;
    private BufferedOutputStream AndropodWriter = null;

    // this code is executed when a routine starts the thread
    public void run() {
        while (true) {
            // as long the ServerSocket (Port 1337) is not closed (from a routine that calls .Suspend(), see below )
            while (this.IsOpen()) {
                try {
                    //wait until a connection is established
                    this.AndropodInterface = this.AndropodSocket.accept();

                    // we need these objects to receive and transmit data over the ServerSocket
                    this.AndropodReader = new BufferedInputStream(this.AndropodInterface.getInputStream());
                    this.AndropodWriter = new BufferedOutputStream(this.AndropodInterface.getOutputStream());

                } catch (Exception e) {
                }
            }

            // .Suspend() was called, so ServerSocket has been closed
            // then close the other objects
            try {
                this.AndropodReader.close();
                this.AndropodWriter.close();
                this.AndropodInterface.close();
            } catch (Exception e) {
            }

            synchronized (this) {
                try {
                    // and wait until a notification with "this.notify"
                    // that means .Resume() was called, see below
                    this.wait();
                } catch (Exception e) {
                }
            }
        }

    }

    /**
     * Suspends the Andropod Interface
     */
    // this routine is called when we stop the Browser-Activity (main-form)= stopping the app
    // or when we go to the settings menue
    public void Suspend() {
        try {
            // close the ServerSocket(Port 1337)
            this.AndropodSocket.close();
        } catch (Exception e) {
        }
    }

    /**
     * Resumes the Andropod Interface from suspend
     */
    // this routine is called when we start the Browser-Activity (main-form)= starting the app
    // or when we resume this activity (after change of settings)
    public void Resume() {
        // isn't the ServerSocket(Port 1337) still open? just for safety reasons
        if (!this.IsOpen()) {
            try {

                // new ServerSocket(Port 1337)
                this.AndropodSocket = new ServerSocket(1337);

                // Thread wasn't started before, this means it is the start of the App
                if (this.getState() == Thread.State.NEW) {

                    // start the Thread (that means the code in the .run()-routine, see above)
                    this.start();
                }
                // Thread was started before, this means we resumed the main-activity
                else {
                    synchronized (this) {
                        // wake up the Thread (waiting in the .run-routine)
                        this.notify();
                    }
                }

            } catch (Exception e) {
                Log.e("info", e.toString());
            }
        }
    }

    /**
     * Checks if the AndropodInterface is connected
     *
     * @return Interface is connected?
     */
    // Connection to ServerSocket (Port 1337) was established
    public boolean IsConnected() {
        if (this.AndropodInterface != null && !this.AndropodInterface.isClosed()) {
            return true;
        }
        return false;
    }

    /**
     * Checks if the port to the AndropodInterface is open
     *
     * @return Port is open?
     */
    // ServerSocket (Port 1337) is opened and is still open (was not closed by .Suspend() )
    public boolean IsOpen() {
        if (this.AndropodSocket != null && !this.AndropodSocket.isClosed()) {
            return true;
        }
        return false;
    }

    /**
     * Transmit a packet to the AndrodpodInterface
     *
     * @param buffer to be transmitted
     * @param len length to send
     */
    public void Write(byte[] buffer, int len) {
        if (this.IsConnected()) {
            try {
                // send out 16 bytes of the rawdata buffer of the message object
                this.AndropodWriter.write(buffer,0,len);
                this.AndropodWriter.flush();
            } catch (Exception e) {
            }
        }
    }

    /**
     * Receive a packet from the AndropodInterface
     * Waits until a packet has been received successfully
     *
     * @return Received packet
     */

    public synchronized byte Receive() {
        // a message-object to be filled with data
        // Connection to ServerSocket was established (for safety reasons)
        byte b=0;
        if (this.IsConnected()) {
            try {
                int c = this.AndropodReader.read();
                if (c != -1) b=(byte)c;
            } catch (Exception e) {
                try {
                    this.AndropodInterface.close();
                } catch (Exception e2) {
                }
                this.AndropodInterface = null;
            }
        }
        return b;
    }


}
