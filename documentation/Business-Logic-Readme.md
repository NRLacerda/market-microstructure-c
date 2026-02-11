# Event-Driven Market Microstructure Trading Architecture

## 1. Core Concept

Modern exchanges do NOT send "prices".
They send EVENTS.

Example event types (ITCH model):
- Add Order
- Cancel Order
- Execute Order
- Delete Order
- Trade

You reconstruct the order book yourself.

Exchange = deterministic state machine  
Market data feed = log of state transitions  
Order book = reconstructed state  

Price is a derived value, not a primary one.

---

## 2. Layered Architecture

[Market Data Feed (ITCH / Binary)]
                ↓
[Order Book Engine]  → stores CURRENT STATE ONLY
                ↓
[Microstructure Analytics Layer] → bounded memory
                ↓
[Strategy Layer] → computes derived value / signal
                ↓
[Execution Layer / Broker API / Simulator]

---

## 3. Order Book (State Only)

The order book stores:

- Active orders (by Order ID)
- Price levels (FIFO per price)
- Bid side (sorted descending)
- Ask side (sorted ascending)
- Best Bid
- Best Ask

It DOES NOT store:
- History of all events
- Candles
- Indicators
- Old canceled orders

### Invariants

1. Every order ID exists in exactly one place
2. Quantity never negative
3. Executions reduce quantity
4. Zero quantity removes order
5. BestBid = highest buy price
6. BestAsk = lowest sell price

---

## 4. Derived Values (Deterministic)

From the order book:

- Best Bid
- Best Ask
- Spread
- Mid Price
- Depth at top N levels

Example:

mid_price = (best_bid + best_ask) / 2

---

## 5. Microstructure Analytics (Bounded Memory)

This layer stores recent behavior, not full history.

Typical metrics:

- Order Flow Imbalance
- Aggressive Buy vs Sell Volume
- Liquidity Add Rate
- Liquidity Cancel Rate
- Spread Changes
- Best Bid/Ask movement frequency

Memory model:
- Sliding window (time-based or count-based)
- Ring buffer
- Fixed-size arrays

Never unbounded storage.

---

## 6. Strategy Layer

Strategy defines what "value" means.

Examples:

### A) Mid-Price Strategy
value = (best_bid + best_ask) / 2

### B) Volume-Weighted Top of Book
value = (bid_price * bid_vol + ask_price * ask_vol) / total_vol

### C) Imbalance-Adjusted Price
imbalance = (bid_vol - ask_vol) / (bid_vol + ask_vol)
value = mid_price + imbalance * spread * factor

### D) Flow-Based Estimation
Adjust mid-price based on recent aggressive executions.

Important:
Price is always DERIVED.
It is never stored as absolute truth.

---

## 7. Execution Layer

Signal != Order.

Workflow:

[Signal]
   ↓
[Risk Controls]
   ↓
[Order Management System]
   ↓
[Broker API or FIX]
   ↓
[Exchange]

Options for execution:
- Broker REST/WebSocket
- FIX Protocol
- Direct Market Access (requires regulation)
- Simulator (recommended during development)

---

## 8. Protocols

### Market Data

Recommended learning protocol:
- NASDAQ TotalView-ITCH 5.0
https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHspecification.pdf

- Binary
- UDP multicast (in production)
- Market-by-order

Other exchanges follow similar models (e.g., B3 PUMA feed).

### Order Entry

- FIX 4.2 / 4.4
- Broker-specific APIs
- Proprietary exchange gateways (institutional)

---

## 9. System Design Principles

- Event-driven (no polling loops with sleep)
- Deterministic state transitions
- No dynamic allocation in hot path (for C implementation)
- Separate state from analytics
- Separate analytics from strategy
- Separate strategy from execution

---

## 10. Minimal Conceptual Data Flow

while (event = read_feed()) {
    apply_to_order_book(event);
    update_analytics(event);
    signal = strategy.evaluate(book, analytics);

    if (signal != NONE)
        execution.send(signal);
}

---

## 11. Mental Models

Order Book = Current constraints  
Analytics = Recent behavior  
Strategy = Interpretation  
Execution = Action  

Exchange ≈ Git commit log  
Order book ≈ Repository state  
Price ≈ Derived view  

---

## 12. Key Takeaways

- Exchanges send state changes, not prices
- Price is emergent from the book
- Store current state only
- Store bounded recent behavior
- Derive value from structure + flow
- Never mix book logic with strategy logic
- Always insert a risk layer before execution

---

## 13. Next Expansion Paths

- Implement ITCH binary parser
- Build deterministic order book in C
- Add replay engine
- Add matching engine simulator
- Add FIX client
- Measure latency per stage
- Implement queue position tracking
- Add multi-symbol support