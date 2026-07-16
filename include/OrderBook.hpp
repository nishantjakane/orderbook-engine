#pragma once

#include <map>
#include <deque>
#include <functional>
#include <optional>
#include <vector>
#include <memory>

#include "Order.hpp"

class OrderBook{
public:
    void addOrder(const std::shared_ptr<Order> &order);

    std::optional<double> getBestBid() const{
        if(bids.empty()){
            return std::nullopt;
        }
        return bids.begin()->first;
    }

    std::optional<double> getBestAsk() const{
        if(asks.empty()){
            return std::nullopt;
        }
        
        return asks.begin()->first;
    }
  
    std::shared_ptr<Order> getBestBuyOrder() const{
        if(bids.empty()){
            return nullptr;
        }
        return bids.begin()->second[0];
    }

    std::shared_ptr<Order> getBestSellOrder() const{
        if(asks.empty()){
            return nullptr;
        }
        return asks.begin()->second[0];
    }


    void removeFilledOrders();

    std::map<double, long long,std::greater<double>> getBidsLevels() const{
        return bidLevels;
    };

    std::map<double, long long> getAsksLevels() const{
        return askLevels;
    };

    bool isEmptyBids() const{
        return bids.empty();
    }

    bool isEmptyAsks() const{
        return asks.empty();
    }

    bool removeOrder(const std::shared_ptr<Order> &order);

    //Levels update helper funcs
    void updateBidLevel(double price,long long deltaQty);
    void updateAskLevel(double price,long long deltaQty);


private:
    std::map<double,std::deque<std::shared_ptr<Order>>,std::greater<double>> bids;
    std::map<double,std::deque<std::shared_ptr<Order>>> asks;

    std::map<double, long long, std::greater<double>> bidLevels;
    std::map<double, long long> askLevels;

};