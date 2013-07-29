package com.example.Teichroboter;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created with IntelliJ IDEA.
 * User: mirko
 * Date: 18.02.13
 * Time: 16:14
 * To change this template use File | Settings | File Templates.
 */
public class TelnetSocket implements Runnable {
    private ServerSocket telnetSocket = null;
    private Thread runner = null;

    TelnetSocket() {
        if (telnetSocket == null) {
            try {
                telnetSocket = new ServerSocket(8888);
                synchronized (this) {
                    if (runner == null) {
                        runner = new Thread(this);
                        runner.start();
                    }
                }
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    // this code is executed when a routine starts the thread
    public void run() {
        while (telnetSocket != null) {
            //wait until a connection is established
            try {
                new TelnetConnection(telnetSocket.accept());
            } catch (IOException e) {
                e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
            }
        }
    }

}
