# Market Microstructure Engine in C

High-performance Limit Order Book (LOB) reconstruction engine written in **C**.
Replays NASDAQ TotalView-ITCH (via LOBSTER) historical message data to deterministically rebuild full market depth.

This project demonstrates **efficient order book reconstruction** with a focus on speed, memory predictability, and price-time priority.

---

## 🚀 Overview

`market-microstructure-c` is a low-level engine designed to:

* Replay historical NASDAQ ITCH order flow
* Reconstruct full depth **limit order books**
* Maintain strict **price-time priority**
* Operate with **predictable memory usage**
* Maximize **performance**, achieving hundreds of thousands of events per second

The engine is **fully deterministic**, ideal for backtesting, research, and educational purposes in market microstructure.

---

## 📈 Data Source

### Message File (Event Stream)

Represents the atomic market events:

* New order submissions
* Partial cancellations
* Full deletions
* Visible executions
* Hidden executions

The message file is the **source of truth** for book reconstruction.

### Orderbook Reference File

Contains expected book states for validation:

* After each event
* Used to check deterministic reconstruction

Demo dataset: **Amazon (AMZN)**

---

## 🏗 Engine Architecture

### Core Principles

* Deterministic replay
* O(1) order lookup
* Strict FIFO within price levels
* Minimal dynamic allocations inside hot loop
* Clear separation between **data model** and **event processor**

---

### 🔧 Components

#### 1️⃣ Order Registry (Hash Table)

Maps:

```
OrderID → Order*
```

Purpose:

* Fast cancellation & execution
* Constant-time lookup
* Cache-friendly memory layout

#### 2️⃣ Price Levels

Each price level maintains:

* Total aggregated volume
* FIFO queue of orders (strict price-time priority)
* Bid side → descending prices
* Ask side → ascending prices

#### 3️⃣ Order Pool

* Orders preallocated in a **memory pool**
* Avoids repeated malloc/free during replay
* Reduces fragmentation
* Improves throughput

#### 4️⃣ Event Processor

* Reads event stream line-by-line
* Updates orders and price levels
* Matches incoming orders immediately
* Supports **fast replay loop**

---

## 🧠 Memory Strategy

* **Stack**: Local parsing buffers, small structs
* **Heap**: Preallocated order pool, hash table buckets, price level queues

All critical structures are designed for **cache-friendly access**.

---

## ⚡ Performance

* Sequential file processing → cache-friendly
* Preallocated memory → no hot-loop malloc
* Integer prices → avoids floating point operations
* Minimal print/logging → can reach **over 1 million events/sec**

### Example Performance on AMZN Dataset

```
Processed: 57,515 events
Replay time: 0.05 seconds
Throughput: 1.173.775 events/sec 
```

Adding per-event logs drastically reduces throughput (~441 events/sec).

---

## 📊 Complexity

| Operation              | Complexity |
| ---------------------- | ---------- |
| Order insertion        | O(1)       |
| Order cancellation     | O(1)       |
| Execution update       | O(1)       |
| Best bid/ask retrieval | O(1)       |
| Event processing       | O(1)       |

Total replay is **O(N)** in number of events.

---

## 🧪 Usage

```c
#include "engine/engine.h"

int main() {
    FILE *file = fopen("res/AMZN_2012-06-21.csv", "r");
    if (!file) return 1;

    Engine *engine = engine_create(100000);

    engine_run(engine, file);

    engine_destroy(engine);
    fclose(file);

    return 0;
}
```

* Build with GCC/Make
* Supports preallocated memory pools for orders
* Minimal external dependencies

---

## 🔮 Future Improvements

* Native binary ITCH parser
* Real-time feed handler
* Multi-asset support
* Profiling & latency measurement
* SIMD parsing optimizations
* Lock-free or multi-threaded version
