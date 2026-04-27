import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.nio.file.*;
import java.util.*;
import java.util.concurrent.*;
import java.util.concurrent.atomic.DoubleAdder;
import java.util.regex.*;
import java.util.stream.*;

public class AsyncTransactionAggregator {

    // Global lock-free counter for highly concurrent accumulation of total profit
    private static final DoubleAdder TOTAL_PROFIT = new DoubleAdder(); 
    
    // Regex pattern to extract metadata: [STATUS] ID AMOUNT:0.00 CURRENCY
    private static final Pattern LOG_PATTERN = Pattern.compile("\\[(\\w+)\\]\\s(\\S+)\\sAMOUNT:(\\d+.\\d+)\\s(\\w+)$");

    record Transaction(String exchange, String id, double amount, String currency, boolean isSuccess) {}

    public static void main(String[] args) {
        System.out.println("Старт Агрегатора (Thread: " + Thread.currentThread().getName() + ")");
        long startTime = System.currentTimeMillis();

        // Dedicated thread pool for non-blocking I/O operations (Exchange polling)
        ExecutorService ioPool = Executors.newFixedThreadPool(3);

        // We simulate queries to the exchanges:
        CompletableFuture<List<Transaction>> binanceFuture = fetchFromExchange("Binance", ioPool)
                .thenApply(rawLogs -> parseLogs(rawLogs, "Binance"))
                // Fallback structure: prevent whole pipeline crash on isolated exchange failure
                .exceptionally(ex -> Collections.emptyList());

        CompletableFuture<List<Transaction>> krakenFuture = fetchFromExchange("Kraken", ioPool)
                .thenApply(rawLogs -> parseLogs(rawLogs, "Kraken"))
                .exceptionally(ex -> Collections.emptyList());

        CompletableFuture<List<Transaction>> bybitFuture = fetchFromExchange("Bybit", ioPool)
                .thenApply(rawLogs -> parseLogs(rawLogs, "Bybit"))
                .exceptionally(ex -> Collections.emptyList()); 

        // Wait for all non-blocking futures to complete concurrently
        CompletableFuture.allOf(binanceFuture, krakenFuture, bybitFuture).join();
        
        System.out.println("Все биржи отдали данные!");

        // Functional stream pipeline: merge, filter by condition, aggregate profit, and group
        Map<String, List<Transaction>> groupedReport = Stream.of(binanceFuture.join(), krakenFuture.join(), bybitFuture.join())
                .flatMap(List::stream)
                .filter(Transaction::isSuccess)
                .filter(x->x.currency.equals("USDT"))
                .peek(x->TOTAL_PROFIT.add(x.amount))
                .collect(Collectors.groupingBy(Transaction::exchange));


        // Pass aggregated data representation for local storage persistence
        writeReportToDisk(groupedReport, TOTAL_PROFIT.doubleValue());

        // Graceful shutdown of executor to prevent resource leaks
        ioPool.shutdown();
        
        long endTime = System.currentTimeMillis();
        System.out.println("Агрегация завершена за " + (endTime - startTime) + " мс. Проверь report.txt");
    }

    private static CompletableFuture<List<String>> fetchFromExchange(String exchangeName, Executor executor) {
        return CompletableFuture.supplyAsync(() -> {
            System.out.println("Запрос к " + exchangeName + " (Поток: " + Thread.currentThread().getName() + ")");
            try { Thread.sleep((long) (200 + Math.random() * 300)); } catch (InterruptedException e) {}
            
            if (exchangeName.equals("Bybit")) {
                throw new RuntimeException("502 Bad Gateway: Сервер лег!"); 
            }
            
            if (exchangeName.equals("Binance")) {
                return List.of(
                    "[SUCCESS] TX_101 AMOUNT:500.5 USDT",
                    "[FAILED] TX_102 AMOUNT:12.0 BTC",
                    "[SUCCESS] TX_103 AMOUNT:150.0 USDT"
                );
            } else {
                return List.of(
                    "[SUCCESS] TX_201 AMOUNT:999.9 USDT",
                    "CORRUPTED_LOG_) AMOUNT:HACKED", 
                    "[SUCCESS] TX_202 AMOUNT:42.0 ETH"
                );
            }
        }, executor);
    }

    private static List<Transaction> parseLogs(List<String> rawLogs, String exchange) {
        // Stream operations to parse structured logs. Malformed entries will safely return an empty stream.
        return rawLogs.stream().flatMap(
                x->
                {
                    Matcher matcher = LOG_PATTERN.matcher(x);
                    if (matcher.matches()) {
                       try{
                           boolean isSuccess = matcher.group(1).equals("SUCCESS");
                           String id = matcher.group(2);
                           double amount = Double.parseDouble(matcher.group(3));
                           String currency = matcher.group(4);

                           Transaction t = new Transaction(exchange, id, amount, currency, isSuccess);
                           return Stream.of(t);
                       }
                       catch(Exception e){
                           return Stream.empty();
                       }
                    }
                    return Stream.empty();
                }
        ).collect(Collectors.toList());
    }

    private static void writeReportToDisk(Map<String, List<Transaction>> report, double totalSum) {
        Path path = Paths.get("report.txt");
        // Convert logic to form a final transaction report String, printing metrics
        System.out.println(totalSum);
        StringBuilder sb = new StringBuilder(100);

        for(Map.Entry<String, List<Transaction>> entry: report.entrySet()) {
            String exchange = entry.getKey();
            List<Transaction> transactions = entry.getValue();
            sb.append("Биржа ").append(exchange).append(transactions+"\n");
        }

        try {
            Files.write(path, sb.toString().getBytes());
        } catch (IOException e) {}

    }
}
