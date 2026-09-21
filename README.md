# Orderbook Engine

A lightweight **C++ limit order book and matching engine** built from scratch to learn how electronic markets, order matching, and market making work under the hood.

## Features

* Limit order matching
* Price-time priority
* Bid / ask order books
* Partial fills
* Multi-level matching
* Order cancellation
* Trade generation
* CMake-based build

## How It Works

The engine maintains separate bid and ask books and matches incoming orders according to **price-time priority**.

```text
          ASK (SELL)
          100.20 × 80
          100.10 × 120
          ───────────
          100.00 × 150
          99.90  × 100
          99.80  × 200
          BID (BUY)
```

The **best ask** is the lowest sell price, while the **best bid** is the highest buy price. Incoming orders that cross the spread are matched against available liquidity.

## Performance

I haven't run a formal benchmark suite yet, but based on the current C++ implementation, I'd expect performance in the **~1–5M orders/sec** range on a modern desktop CPU in a Release build.

Actual performance will depend heavily on CPU, compiler optimizations, order-book size, and workload.

> **Benchmark numbers are estimates, not measured results.**

## Why I Built This

I built this primarily as a learning project to understand:

* Market microstructure
* Price discovery
* Bid-ask spreads
* Order matching
* Liquidity and inventory risk
* How market makers operate
* Low-latency C++ systems

The goal is to eventually build simulations and strategies on top of the matching engine.

## Build

```bash
cmake -S . -B build
cmake --build build --config Release
```


**Built by [Nishant Jakane](https://github.com/nishantjakane)**

*Educational project exploring trading infrastructure and market microstructure.*
