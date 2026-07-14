#pragma once

#include <string>
#include <chrono>

enum class OrderType
{
    Market,
    Limit
};

enum class Side
{
    Buy,
    Sell
};

enum class OrderStatus
{
    Active,
    Filled,
    Cancelled
};

class Order
{
public:
    Order(
        std::string id,
        long long qty,
        double price,
        OrderType type,
        Side side,
        long long sequence,
        std::chrono::system_clock::time_point timestamp)
        : id(id),
          qty(qty),
          remainingQty(qty),
          price(price),
          type(type),
          side(side),
          sequence(sequence),
          timestamp(timestamp)
    {
    }
    // Getters

    std::string getId() const
    {
        return id;
    }

    long long getQty() const
    {
        return qty;
    }

    long long getRemainingQty() const
    {
        return remainingQty;
    }

    OrderType getOrderType() const
    {
        return type;
    }

    Side getSide() const
    {
        return side;
    }

    auto getTime() const
    {
        return timestamp;
    }

    double getPrice() const
    {
        return price;
    }

    auto getSequence() const {
        return sequence;
    }

    auto getStatus() const { return status;}

    void cancelOrder() {
        status = OrderStatus::Cancelled;
    }

    // Methods
    bool execute(long long qty)
    {
        if (qty <= 0 || qty > remainingQty)
        {
            return false;
        }

        remainingQty -= qty;
        if(remainingQty ==0){
            status = OrderStatus::Filled;
        }
        return true;
    }

    bool isFilled() const {return remainingQty==0;}
    bool isCancelled() const { return status==OrderStatus::Cancelled;}

private:
    std::string id;
    long long qty;
    long long remainingQty;
    double price;
    OrderType type;
    Side side;
    OrderStatus status = OrderStatus::Active;
    long long sequence;
    std::chrono::system_clock::time_point timestamp;
};