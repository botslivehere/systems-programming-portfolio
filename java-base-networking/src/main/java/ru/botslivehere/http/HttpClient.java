package ru.botslivehere.http;

import java.net.URI;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.nio.file.Path;

import static java.net.http.HttpRequest.BodyPublishers.ofFile;

public class HttpClient {
    public static void main(String[] args) {
        try {
            var httpClient = java.net.http.HttpClient.newBuilder()
                    .version(java.net.http.HttpClient.Version.HTTP_1_1)
                    .build();

           var request = HttpRequest.newBuilder()
                            .uri(URI.create("http://localhost:8082"))
                            .header("Content-Type","application/json")
                            .POST(ofFile(Path.of("src/main/resources/input.json")))
                    .build();

            for (int i = 0; i < 3; i++) {
                var response = httpClient.sendAsync(request, HttpResponse.BodyHandlers.ofString());
                System.out.println(response.get().headers());
                System.out.println(response.get().body());
            }

        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
