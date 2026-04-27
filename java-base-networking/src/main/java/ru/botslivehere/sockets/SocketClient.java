package ru.botslivehere.sockets;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.Socket;

public class SocketClient {
    public static void main(String[] args) {
        try(var socket = new Socket("localhost",8081);
            var inputSteam = new DataInputStream(socket.getInputStream());
            var outputStream = new DataOutputStream(socket.getOutputStream());
        ){
            outputStream.writeUTF("Ping!");
            System.out.println(inputSteam.readUTF());
        }
        catch (Exception e){

        }
    }
}
