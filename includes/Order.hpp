#pragma once

#include <string>

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
        const std::string &time)
        : id(id),
          qty(qty),
          remainingQty(qty),
          price(price),
          type(type),
          side(side),
          time(time)
    {
    }
    // Getters

    std::string getID() const
    {
        return id;
    }

    long long getQTY() const
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

    std::string getTime() const
    {
        return time;
    }

    double getPrice() const
    {
        return price;
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
    const std::string time;
};