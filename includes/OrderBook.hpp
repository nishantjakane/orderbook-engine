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
    void addOrder(const std::shared_ptr<Order> &order){
        auto side = order->getSide();
        auto price = order->getPrice();

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


    void removeFilledOrders(){
        for(auto levelit = bids.begin();levelit!=bids.end();){
            auto &level =*levelit;
            auto &orders = level.second;

            for(auto it=orders.begin();it!=orders.end();){
                if((*it)->isFilled()){
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
                if((*it)->isFilled()){
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

    auto getBidsLevels() const{
        std::vector<std::pair<double,long long>> levels;
        for(auto level : bids){
            auto &orders = level.second;
            auto &levelPrice = level.first;
            long long levelQty = 0;
            for(auto &order : orders){
                levelQty+=order->getRemainingQty();
            }

            levels.push_back({levelPrice , levelQty});
        }

        return levels;
    }

    auto getAsksLevels() const{
        std::vector<std::pair<double,long long>> levels;
        for(auto level : asks){
            auto &orders = level.second;
            auto &levelPrice = level.first;
            long long levelQty = 0;
            for(auto &order : orders){
                levelQty+=order->getRemainingQty();
            }

            levels.push_back({levelPrice , levelQty});
        }

        return levels;
    }


    bool isEmptyBids() const{
        return bids.empty();
    }

    bool isEmptyAsks() const{
        return asks.empty();
    }

    bool removeOrder(const std::shared_ptr<Order> &order) {
        if(order->getSide()==Side::Buy){
            auto levelIt = bids.find(order->getPrice());
            if(levelIt==bids.end()){
                return false;
            }
            auto &orders = levelIt->second;
            for(auto it = orders.begin();it!=orders.end();){
                if(*it == order){
                    it = orders.erase(it);
                    if(orders.empty()){
                        bids.erase(levelIt);
                    }
                    return true;
                }else{
                    ++it;
                }
            }
        }else if(order->getSide()==Side::Sell){
            auto levelIt = asks.find(order->getPrice()); 
            if(levelIt==asks.end()){
                return false;
            }
            auto &orders = levelIt->second;
            for(auto it = orders.begin();it!=orders.end();){
                if(*it == order){
                    it = orders.erase(it);
                    if(orders.empty()){
                        asks.erase(levelIt);
                    }
                    return true;
                }else{
                    ++it;
                }
            }
        }


        return false;
    }


private:
    std::map<double,std::deque<std::shared_ptr<Order>>,std::greater<double>> bids;
    std::map<double,std::deque<std::shared_ptr<Order>>> asks;
};