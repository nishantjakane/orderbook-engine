#pragma once

#include <string>

class Trade
{

public:
    Trade(
        const std::string &tradeId,
        const std::string &buyOrderId,
        const std::string &sellOrderId,
        double price,
        long long qty,
        const std::string &time)
        : tradeId(tradeId),
          buyOrderId(buyOrderId),
          sellOrderId(sellOrderId),
          price(price),
          qty(qty),
          time(time)
    {
    }

    //Getters

    const std::string &getTradeId() const
    {
        return tradeId;
    }

    const std::string &getBuyOrderId() const
    {
        return buyOrderId;
    }

    const std::string &getSellOrderId() const
    {
        return sellOrderId;
    }

    double getPrice() const
    {
        return price;
    }

    long long getQty() const
    {
        return qty;
    }

    const std::string &getTime() const
    {
        return time;
    }

private:
    const std::string tradeId;
    const std::string buyOrderId;
    const std::string sellOrderId;
    const double price;
    const long long qty;
    const std::string time;
};