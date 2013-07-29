package com.example.Teichroboter;

import java.io.*;
import java.net.Socket;

/**
 * Created with IntelliJ IDEA.
 * User: mirko
 * Date: 18.02.13
 * Time: 16:21
 * To change this template use File | Settings | File Templates.
 */
public class TelnetConnection implements Runnable
{
    private Socket socket=null;
    private Thread runner=null;

    TelnetConnection(Socket socket)
    {
        this.socket = socket;
        synchronized (this) {
            if (runner == null) {
                runner = new Thread(this);
                runner.start();
            }
        }
    }

    public void run() {
        try {
            BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
            PrintWriter writer = new PrintWriter(socket.getOutputStream());

            writer.append("Willkommen auf dem Teichroboter!");

            while (socket!=null)
            {
               writer.append("\r\n>"); // Prompt
               writer.flush();
               String line = reader.readLine();
               {
                   writer.append(line);
               }
            }
        } catch (IOException e) {
            e.printStackTrace();  //To change body of catch statement use File | Settings | File Templates.
        }
    }


}
