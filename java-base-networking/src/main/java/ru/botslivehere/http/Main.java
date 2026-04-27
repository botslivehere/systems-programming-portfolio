package ru.botslivehere.http;

public class Main {
    public static void main(String[] args) {
        HttpServer server = new HttpServer(8082,100);
        server.run();
    }
}
