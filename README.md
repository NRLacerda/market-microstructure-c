# market-microstructure-c

High-performance Limit Order Book reconstruction engine written in C.
Replays NASDAQ TotalView-ITCH (via LOBSTER) historical message data to deterministically rebuild full market depth.

All sample datasets are based on the official **NASDAQ Historical TotalView-ITCH sample**, using Amazon (AMZN) as demonstration data.

---

## 🚀 Overview

`market-microstructure-c` is a low-level market data replay engine designed to:

* Process historical NASDAQ ITCH order flow
* Reconstruct a full depth limit order book
* Preserve strict price-time priority
* Operate with predictable memory behavior
* Emphasize cache-friendly and performance-aware design

The project focuses on systems programming and market microstructure modeling.

---

## 📈 Data Source

* NASDAQ TotalView-ITCH sample data
* Processed via LOBSTER format
* Demo dataset: **Amazon (AMZN)**

Two input files:

### Message File (Event Stream)

Represents the atomic market events:

* New order submissions
* Partial cancellations
* Full deletions
* Visible executions
* Hidden executions

This is the **source of truth**.

### Orderbook File (Reference State)

Represents the expected book state after each event.
Used for deterministic validation.

---

## 🏗 Engine Architecture

### Core Design Principles

* Deterministic replay
* O(1) average order lookup
* Strict FIFO within price levels
* Minimal dynamic allocations during replay
* Clear separation between data model and event processor

---

## 🔧 Core Components

### 1️⃣ Order Registry (Hash Table)

Maps:

```
OrderID → Order*
```

Used for:

* Fast cancellation
* Fast execution lookup
* Size updates

Implemented from scratch to control:

* Collision resolution strategy
* Memory layout
* Cache locality

---

### 2️⃣ Price Levels

Each price level contains:

* Aggregated volume
* FIFO queue of orders

Structure:

```
Bid Side  → descending price priority
Ask Side  → ascending price priority
```

Price-time priority is preserved via queue ordering.

---

### 3️⃣ FIFO Queue (Per Price Level)

Maintains strict execution ordering.

Properties:

* O(1) insertion
* O(1) removal from head
* Linked list or intrusive node design

---

### 4️⃣ Order Book Core

Contains:

* Bid structure
* Ask structure
* Order registry
* Event processor

---

## 🧠 Memory Strategy

### Stack

Used for:

* Small local structs
* Temporary parsing buffers
* Fixed-size control structures

Reason:

* Fast allocation
* Automatic cleanup
* No fragmentation

---

### Heap

Used for:

* Orders
* Hash table buckets
* Price level queues

Reason:

* Dynamic growth
* Large dataset handling
* Explicit lifetime control

---

## ⚡ Performance Considerations

* Sequential file processing (cache friendly)
* Avoid unnecessary malloc/free inside hot loop
* Preallocate hash table capacity
* Use integer prices (no floating point)
* Struct packing awareness
* Predictable memory layout

Potential optimizations:

* Custom memory pool allocator
* Arena allocator
* Lock-free structures (for future real-time variant)
* SIMD parsing
* Binary ITCH parser

---

## 📊 Complexity

| Operation              | Expected Complexity |
| ---------------------- | ------------------- |
| Order insertion        | O(1)                |
| Order cancellation     | O(1)                |
| Execution update       | O(1)                |
| Best bid/ask retrieval | O(1)                |

Replay is strictly linear in number of events:

```
O(N)
```

---

## 🧪 Validation

The engine validates reconstructed states against:

LOBSTER orderbook reference file

Ensures deterministic correctness.

---

## 📚 Market Microstructure Concepts

* Limit order book mechanics
* Price-time priority
* Order flow replay
* Visible vs hidden liquidity
* ITCH protocol semantics

---

## 🔮 Future Extensions

* Native ITCH binary parser
* Real-time feed handler
* Matching engine simulation
* Latency benchmarking
* Profiling with perf / valgrind
* Multi-asset support
* Persistent memory allocator

---