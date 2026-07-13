#pragma once
#include <vector>
#include <algorithm>
#include <chrono>
#include <memory>

#include "Order.hpp"
#include "OrderBook.hpp"
#include "Trade.hpp"
#include "IdGenerator.hpp"

class MatchingEngine{
public:

    long long generateSequence(){
        return sequenceGenerator.generate();
    }


    // Getters
    const OrderBook &getOrderBook() const{
        return orderBook;
    }

    const std::vector<Trade> &getTrades() const{
        return trades;
    }
    
    const std::vector<std::shared_ptr<Order>>& getOrderHistory() const{ 
        return orderHistory;
    }

    std::string generateOrderId(){
        return "O" + std::to_string(orderIdGenerator.generate());
    }

    void submitOrder(const std::shared_ptr<Order> &order);

    bool cancelOrder(const std::shared_ptr<Order>& order);

    std::shared_ptr<Order> modifyOrder(const std::shared_ptr<Order>& order,long long newQty,double newPrice);

private:

    OrderBook orderBook;
    std::vector<std::shared_ptr<Order>> orderHistory;

    std::vector<Trade> trades;

    IdGenerator tradeIdGenerator;
    IdGenerator sequenceGenerator;
    IdGenerator orderIdGenerator;

    std::string generateTradeId(){
        return "T" + std::to_string(tradeIdGenerator.generate());
    }

    void executeTrade(const std::shared_ptr<Order> &order,const std::shared_ptr<Order> &bestMatchingOrder,Side orderSide);

    void matchBuy(const std::shared_ptr<Order> &order);

    void matchSell(const std::shared_ptr<Order> &order);

};