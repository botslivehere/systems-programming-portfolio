package ru.botslivehere.sockets;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.ServerSocket;

public class SocketServer {
    public static void main(String[] args) {
        try(var serverSocket = new ServerSocket(8081);
            var socket = serverSocket.accept();
            var inputSteam = new DataInputStream(socket.getInputStream());
            var outputStream = new DataOutputStream(socket.getOutputStream());
        ){
            System.out.println("Data from client:"+inputSteam.readUTF());
            outputStream.writeUTF("Hello dear client, pong...");
        }
        catch (Exception e){

        }
    }
}
