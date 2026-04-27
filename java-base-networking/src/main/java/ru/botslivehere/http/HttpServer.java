package ru.botslivehere.http;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Arrays;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class HttpServer implements Runnable{
    private final int port;
    private final ExecutorService executor;

    public HttpServer(int port,int threads) {
        this.port = port;
        executor = Executors.newFixedThreadPool(threads);
    }

    @Override
    public void run() {
            try {
                ServerSocket serverSocket = new ServerSocket(port);
                while(true) {
                    var socket = serverSocket.accept();
                    System.out.println("Socket Accepted");
                    executor.submit(() -> processSocket(socket));
                }
            } catch (IOException e) {
                e.printStackTrace();
            }

    }

    private void processSocket(Socket socket) {
        try(socket;
            var inputSteam = new DataInputStream(socket.getInputStream());
            var outputStream = new DataOutputStream(socket.getOutputStream());
        ) {
            System.out.println(Arrays.toString(inputSteam.readNBytes(40)));
            byte[] bytes = Files.readAllBytes(Path.of("src/main/resources/output.html"));


            outputStream.write("""
            HTTP/1.1 200 OK
            content-type: text/html
            content-length: %s
            """.formatted(bytes.length).getBytes());

            outputStream.write(System.lineSeparator().getBytes());
            outputStream.write(bytes);

        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
