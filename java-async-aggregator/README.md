# Async Transaction Aggregator

## Overview
A production-like microservice simulation designed to aggregate and process raw, unstructured transaction logs from multiple cryptocurrency exchanges (e.g., Binance, Kraken, Bybit). This project highlights advanced Java concurrency, thread-safe state management, fault tolerance, and functional data transformations.

## Technical Scope
- **Asynchronous Polling:** Implementation of parallel data retrieval from multiple simulated I/O bound services to minimize latency and maximize throughput.
- **Data Validation & Parsing:** Robust extraction of transaction metadata (status, ID, amount, currency) from unpredictable string logs using regular expressions.
- **Fault Tolerance & Resilience:** Strategic exception handling tailored to ensure that corrupted data logs or isolated service failures (like simulated 502 Bad Gateway errors) do not cascade and compromise the rest of the execution pipeline.
- **Functional Data Pipelines:** Utilization of stream APIs to map, filter, and aggregate transactions, enforcing specific business logic (e.g., tracking only successful "USDT" transactions) and grouping outputs intelligently.
- **Concurrent State Mutation:** Utilization of highly concurrent atomic primitives to maintain global metrics (like total financial profit) without introducing blocking overhead or race conditions across multiple threads.
- **Persistent Storage:** Formatting and flushing the highly structured final financial report out to standard local disk storage.
