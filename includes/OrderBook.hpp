#pragma once

#include <map>
#include <deque>
#include <functional>
#include<optional>

#include "Order.hpp"

class OrderBook{
public:
    void addOrder(const Order &order){
        auto side = order.getSide();
        auto price = order.getPrice();

        if(side==Side::Buy){
            bids[price].push_back(order);
        }else if(side==Side::Sell){
            asks[price].push_back(order);
        }
    }

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


    // Returns a order pointer so we need to dereference
    Order* getBestBuyOrder(){
        if(bids.empty()){
            return nullptr;
        }
        return &bids.begin()->second[0];
    }

    Order* getBestSellOrder(){
        if(asks.empty()){
            return nullptr;
        }
        return &asks.begin()->second[0];
    }

    void removeFilledOrders(){
        for(auto levelit = bids.begin();levelit!=bids.end();){
            auto &level =*levelit;
            auto &orders = level.second;

            for(auto it=orders.begin();it!=orders.end();){
                if(it->isFilled()){
                    it=orders.erase(it);
                }else{
                    ++it;
                }
            }

            if(orders.empty()){
                levelit = bids.erase(levelit);
            }else{
                ++levelit;
            }
        }

        for(auto levelit = asks.begin();levelit!=asks.end();){
            auto &level =*levelit;
            auto &orders = level.second;


            for(auto it=orders.begin();it!=orders.end();){
                if(it->isFilled()){
                    it=orders.erase(it);
                }else{
                    ++it;
                }
            }

            if(orders.empty()){
                levelit = asks.erase(levelit);
            }else{
                ++levelit;
            }
        }
    }


    bool isEmptyBids(){
        return bids.empty();
    }

    bool isEmptyAsks(){
        return asks.empty();
    }


private:
    std::map<double,std::deque<Order>,std::greater<double>> bids;
    std::map<double,std::deque<Order>> asks;
};