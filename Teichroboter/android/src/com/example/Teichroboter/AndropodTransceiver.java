package com.example.Teichroboter;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created with IntelliJ IDEA. User: mirko Date: 16.02.13 Time: 20:13
 */
public class AndropodTransceiver implements Runnable {

    private final Handler receiveHandler;

    public AndropodTransceiver(Handler receiveHandler) {

        this.receiveHandler = receiveHandler;
    }

    public static enum Type {
        APOD_TELTYPE_DATANACK(1),
        APOD_TELTYPE_DATA(2),
        APOD_TELTYPE_ACK(3),
        APOD_TELTYPE_NACK(4),
        APOD_TELTYPE_PING(5),
        APOD_TELTYPE_PINGACK(6),
        APOD_TELTYPE_FRAGMENT(7);

        private final byte value;

        private Type(final int value) {
            this.value = (byte) value;
        }
    }

    private short sourceAddress = 2;
    private byte counter = 0;

    private ServerSocket andropodSocket = null;
    private Socket andropodInterface = null;
    private BufferedInputStream andropodReader = null;
    private BufferedOutputStream andropodWriter = null;

    private Thread runner = null;

    @SuppressWarnings("UnusedDeclaration")
    public short getSourceAddress() {
        return sourceAddress;
    }

    @SuppressWarnings("UnusedDeclaration")
    public void setSourceAddress(final short sourceAddress) {
        this.sourceAddress = sourceAddress;
    }

    // this code is executed when a routine starts the thread
    public void run() {
        while (isOpen()) {
            try {
                // as long the ServerSocket (Port 1337) is not closed (from a routine that calls .suspendTransceiver(), see below )
                //wait until a connection is established
                andropodInterface = andropodSocket.accept();

                // we need these objects to receive and transmit data over the ServerSocket
                andropodReader = new BufferedInputStream(andropodInterface.getInputStream());
                andropodWriter = new BufferedOutputStream(andropodInterface.getOutputStream());

                while (isOpen()) {
                    try {
                        if (andropodReader.available() > 0) {
                            byte[] bytes = new byte[andropodReader.available()];
                            andropodReader.read(bytes);
                            final StringBuilder sb = new StringBuilder();
                            for (byte b : bytes) {
                              sb.append(String.format("%02X",b)).append(' ');
                            }
                            log(sb.toString());
                        }
                    } catch (IOException e) {
                        // Shit happens, do someting - later
                        log("Lesefehler: "+e.getMessage());
                    }
                }
                andropodReader.close();
                andropodWriter.close();
                andropodInterface.close();
            } catch (IOException e) {
                // Ignore
                log("Verbindungsfehler: "+e.getMessage());
            }
        }

        runner = null;
//            synchronized (this) {
//                try {
//                    // and wait until a notification with "this.notify"
//                    // that means .resumeTransceiver() was called, see below
//                    this.wait();
//                } catch (InterruptedException e) {
//                    // Ignore
//                }
//        }
//    }
    }

    private void log(String s) {
        Message msg = new Message();
        Bundle bundle = new Bundle();
        bundle.putString("text", s);
        msg.setData(bundle);
        receiveHandler.sendMessage(msg);
    }

    /**
     * Suspends the Andropod Interface
     */
    // this routine is called when we stop the Browser-Activity (main-form)= stopping the app
    // or when we go to the settings menue
    public void suspendTransceiver() {
        try {
            // close the ServerSocket(Port 1337)
            this.andropodSocket.close();
        } catch (IOException e) {
            // Ignore
        }
    }

    /**
     * Resumes the Andropod Interface from suspend
     */
    // this routine is called when we start the Activity (main-form)= starting the app
    // or when we resume this activity (after change of settings)
    public void resumeTransceiver() {
        // isn't the ServerSocket(Port 1337) still open? just for safety reasons
        if (!isOpen()) {
            try {

                // new ServerSocket(Port 1337)
                andropodSocket = new ServerSocket(1337);

                synchronized (this) {
                    if (runner == null) {
                        runner = new Thread(this);
                        runner.start();
                    }
                }

//                // Thread wasn't started before, this means it is the start of the App
//                if (this.getState() == Thread.State.NEW) {
//
//                    // start the Thread (that means the code in the .run()-routine, see above)
//                    this.start();
//                }
//                // Thread was started before, this means we resumed the main-activity
//                else {
//                    synchronized (this) {
//                        // wake up the Thread (waiting in the .run-routine)
//                        this.notify();
//                    }
//                }

            } catch (Exception e) {
                log("Fehler bei Open:" + e.toString());
            }
        }
    }

    /**
     * Checks if the AndropodInterface is connected
     *
     * @return Interface is connected?
     */
    // Connection to ServerSocket (Port 1337) was established
    public boolean isConnected() {
        return this.andropodInterface != null && !this.andropodInterface.isClosed();
    }

    /**
     * Checks if the port to the AndropodInterface is open
     *
     * @return Port is open?
     */
    // ServerSocket (Port 1337) is opened and is still open (was not closed by .suspendTransceiver() )
    public boolean isOpen() {
        return this.andropodSocket != null && !this.andropodSocket.isClosed();
    }

    /**
     * Receive a packet from the AndropodInterface Waits until a packet has been received successfully
     *
     * @return Received packet
     */

    public synchronized byte read() {
        // a message-object to be filled with data
        // Connection to ServerSocket was established (for safety reasons)
        byte b = 0;
        if (this.isConnected()) {
            try {
                int c = this.andropodReader.read();
                if (c != -1) {
                    b = (byte) c;
                }
            } catch (Exception e) {
                try {
                    this.andropodInterface.close();
                } catch (Exception e2) {
                    // Ignore
                }
                this.andropodInterface = null;
            }
        }
        return b;
    }

    public void write(final Type type, final short targetAddress, final byte[] bytes) throws IOException {
        final AndropodTelegram telegram = new AndropodTelegram();
        telegram.type = type.value;
        telegram.sourceAddress = sourceAddress;
        telegram.targetAddress = targetAddress;
        telegram.counter = counter++;
        telegram.length = (bytes.length <= AndropodTelegram.ANDROPOD_BUFFER_SIZE) ? (byte) bytes.length
                : AndropodTelegram.ANDROPOD_BUFFER_SIZE;
        System.arraycopy(bytes, 0, telegram.data, 0, telegram.length);
        telegram.computeCheckSum();
        telegram.writeBytes(andropodWriter);
    }

    public void write_raw(final byte[] bytes) throws IOException {
        andropodWriter.write(bytes, 0, bytes.length);
        andropodWriter.flush();
    }

    private class AndropodTelegram {
        public static final int ANDROPOD_BUFFER_SIZE = 20;
        private final int HEADER_SIZE = 8;

        byte checkSum;
        byte type;
        short sourceAddress;
        short targetAddress;
        byte counter;
        byte length;
        final byte[] data = new byte[ANDROPOD_BUFFER_SIZE];

        public void computeCheckSum() {
            byte crc = processByte((byte) 0, type);
            crc = processByte(crc, (byte) (sourceAddress & 0xFF));
            crc = processByte(crc, (byte) ((sourceAddress >> 8) & 0xFF));
            crc = processByte(crc, (byte) (targetAddress & 0xFF));
            crc = processByte(crc, (byte) ((targetAddress >> 8) & 0xFF));
            crc = processByte(crc, counter);
            crc = processByte(crc, length);
            for (int i = 0; i < length; i++) {
                crc = processByte(crc, data[i]);
            }
            checkSum = crc;
        }

        private byte processByte(byte crc, byte data) {
            for (int i = 0; i < 8; i++) {
                final boolean feedback = ((crc ^ data) & 0x01) == 1;
                if (feedback) {
                    crc ^= 0x18;
                }
                crc = (byte) ((crc >> 1) & 0x7F);
                if (feedback) {
                    crc |= 0x80;
                }
                data >>= 1;
            }
            return crc;
        }

        @SuppressWarnings("UnusedDeclaration")
        public void writeBytes(final OutputStream os) throws IOException {
            final byte[] buffer = new byte[HEADER_SIZE + ANDROPOD_BUFFER_SIZE];
            buffer[0] = checkSum;
            buffer[1] = type;
            buffer[2] = (byte) (sourceAddress & 0xFF);
            buffer[3] = (byte) ((sourceAddress >> 8) & 0xFF);
            buffer[4] = (byte) (targetAddress & 0xFF);
            buffer[5] = (byte) ((targetAddress >> 8) & 0xFF);
            buffer[6] = counter;
            buffer[7] = length;
            System.arraycopy(data, 0, buffer, HEADER_SIZE, length);
            os.write((byte) 0xAA);
            os.write(buffer, 0, HEADER_SIZE + length);
            os.flush();
            final StringBuilder sb = new StringBuilder();
            for (int i=0; i<HEADER_SIZE+length; i++) {
              sb.append(String.format("%02x",buffer[i])).append(' ');
            }
            log(sb.toString());
        }

        @SuppressWarnings("UnusedDeclaration")
        public void readBytes(final InputStream is) {
            final byte[] buffer = new byte[HEADER_SIZE + ANDROPOD_BUFFER_SIZE];
            int index = 0;
            int i = 0;
        }
    }
}
