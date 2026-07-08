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

class Order
{
public:
    Order(
        const std::string &id,
        const long long qty,
        const double price,
        OrderType type,
        Side side,
        const long long sequence,
        const std::chrono::system_clock::time_point timestamp)
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

    // Methods
    bool execute(long long qty)
    {
        if (qty <= 0 || qty > remainingQty)
        {
            return false;
        }

        remainingQty -= qty;
        return true;
    }

    bool isFilled() const {return remainingQty==0;}

private:
    const std::string id;
    const long long qty;
    long long remainingQty;
    const double price;
    OrderType type;
    Side side;
    const long long sequence;
    const std::chrono::system_clock::time_point timestamp;
};