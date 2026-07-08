#pragma once

#include <string>
#include <chrono>

class Trade
{

public:
    Trade(
        const std::string &tradeId,
        const std::string &buyOrderId,
        const std::string &sellOrderId,
        double price,
        long long qty,
        long long sequence,
        const std::chrono::system_clock::time_point &timestamp)
        : tradeId(tradeId),
          buyOrderId(buyOrderId),
          sellOrderId(sellOrderId),
          price(price),
          qty(qty),
          sequence(sequence),
          timestamp(timestamp)
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

    auto &getTime() const
    {
        return timestamp;
    }

private:
    const std::string tradeId;
    const std::string buyOrderId;
    const std::string sellOrderId;
    const double price;
    const long long qty;
    const long long sequence;
    const std::chrono::system_clock::time_point timestamp;
};