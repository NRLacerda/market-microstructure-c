---

# 📘 Order Book Reconstruction – Technical Design (C Implementation)

## 1. Overview

This document describes the **technical design decisions** for reconstructing a LOBSTER limit order book in C.

The system replays historical order events (message file) and reconstructs the evolving state of the order book from scratch.

Focus areas:

* Data structures to implement
* Memory management strategy
* Stack vs Heap decisions
* Ownership model
* Design tradeoffs

---

# 2. High-Level Architecture

```
Message File  →  Event Processor  →  OrderBook Engine  →  Internal State
```

We do **not simulate matching**.

We replay events sequentially and update internal structures accordingly.

---

# 3. Core Data Structures (To Be Implemented From Scratch)

We must implement the following:

* Hash Table (Order ID lookup)
* FIFO Queue (per price level)
* Price Level structure
* Order structure
* OrderBook structure

---

# 4. Data Structures Design

---

## 4.1 Order

Represents a single limit order.

```c
typedef struct Order {
    long id;
    int size;
    int price;
    int direction;   // 1 = buy, -1 = sell
    struct Order* next;   // FIFO linkage
} Order;
```

### Why heap allocation?

* Orders are created dynamically.
* Orders are deleted in arbitrary order.
* Lifetime does not follow stack LIFO.
* Quantity unknown at compile time.

---

## 4.2 FIFO Queue (Per Price Level)

Each price level maintains FIFO priority.

```c
typedef struct {
    Order* head;
    Order* tail;
} Queue;
```

Operations:

* enqueue
* remove specific order
* adjust size

Implemented manually using linked list.

---

## 4.3 Price Level

Represents one price bucket.

```c
typedef struct PriceLevel {
    int price;
    int total_volume;
    Queue orders;
    struct PriceLevel* next;
} PriceLevel;
```

Two separate linked lists:

* Bids (descending prices)
* Asks (ascending prices)

---

## 4.4 Hash Table (Order ID → Order*)

Purpose:

* O(1) lookup for cancel/execute/delete events.

Must implement:

* Hash function
* Collision resolution (chaining recommended)
* Insert
* Lookup
* Remove

Why needed:
Message events reference Order ID directly.
We must find the exact order quickly.

---

## 4.5 OrderBook (Controller Struct)

```c
typedef struct {
    PriceLevel* bids;
    PriceLevel* asks;
    HashTable order_map;
} OrderBook;
```

### Why stack allocation is appropriate:

* Small struct (mostly pointers)
* Only one instance needed
* Lifetime equals program lifetime
* No dynamic resizing required

---

# 5. Memory Management Strategy

## 5.1 Stack Usage

Used for:

* OrderBook struct
* Temporary variables
* Function-local logic
* Counters

Why:

* Fast
* Automatically freed
* Small and predictable size

---

## 5.2 Heap Usage

Used for:

* Orders
* Price levels
* Hash table entries

Why:

* Quantity unknown at runtime
* Arbitrary creation/deletion
* Non-LIFO lifetime
* Linked data structures

---

# 6. Allocation Rules

## Order Creation

```c
Order* o = malloc(sizeof(Order));
```

Must check for NULL.

---

## Order Deletion

When:

* Type 3 (delete)
* Size becomes zero after execution

Actions:

* Remove from queue
* Remove from hash table
* free(order)

---

## Price Level Deletion

If total_volume == 0:

* Remove level from list
* free(level)

---

# 7. Ownership Model

| Object     | Owner      | Responsibility        |
| ---------- | ---------- | --------------------- |
| OrderBook  | main()     | Cleanup all internals |
| PriceLevel | OrderBook  | Free when empty       |
| Order      | PriceLevel | Free when deleted     |
| Hash Entry | HashTable  | Free on removal       |

---

# 8. Initialization & Cleanup

## Initialization

```c
void init_orderbook(OrderBook* book);
```

* Set bids/asks to NULL
* Initialize hash table

---

## Cleanup

```c
void destroy_orderbook(OrderBook* book);
```

Must:

* Free all orders
* Free all price levels
* Destroy hash table
* Leave no memory leaks

---

# 9. Event Handling Logic

| Type | Action                                        |
| ---- | --------------------------------------------- |
| 1    | Create order + insert into price level + hash |
| 2    | Reduce order size                             |
| 3    | Remove order completely                       |
| 4    | Reduce order size (execution)                 |
| 5    | Ignore (hidden, no visible impact)            |
| 7    | Optional: halt handling                       |

---

# 10. Design Tradeoffs

## Linked List vs Array for Price Levels

Linked List:

* Simpler
* Good for learning
* Slower lookup

Array by price index:

* Faster
* More complex
* Requires bounded price range

Initial implementation: linked list.

---

## Hash Table Strategy

Chaining (linked list buckets) recommended:

* Simpler to implement
* Easier memory handling

Avoid open addressing for first version.

---

# 11. Performance Considerations (Future Optimization)

Once working:

* Memory pool for orders
* Replace linked list with balanced tree
* Replace hash with faster implementation
* Reduce malloc frequency

But correctness first.

---

# 12. Summary of Memory Decisions

| Component    | Stack | Heap | Why               |
| ------------ | ----- | ---- | ----------------- |
| OrderBook    | ✅     | ❌    | Small, fixed size |
| Orders       | ❌     | ✅    | Dynamic count     |
| Price Levels | ❌     | ✅    | Dynamic count     |
| Hash Table   | ❌     | ✅    | Dynamic storage   |
| Temp Vars    | ✅     | ❌    | Short lifetime    |

---

# 13. Core Design Principle

Stack = control structures
Heap = dynamic market data

Stack handles predictable lifetime.
Heap handles unpredictable growth and deletion.

---

# 14. Final Goal

After replaying all messages:

* OrderBook reflects final state
* No memory leaks
* No dangling pointers
* All allocations freed

System behaves like a deterministic replay engine.

---

If you want, I can next generate:

* A project folder structure
* A minimal compilable skeleton
* A memory-safe destroy implementation example
* Or a visual memory layout diagram
