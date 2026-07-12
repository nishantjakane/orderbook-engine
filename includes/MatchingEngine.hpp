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


    void submitOrder(const std::shared_ptr<Order> &order){
        auto side = order->getSide();
        auto orderType = order->getOrderType();

        orderHistory.push_back(order);
        // Limit order logic
        if(orderType==OrderType::Limit){
            if (side == Side::Buy){
                // handles the trade execution till best price or qty becomes 0
                matchBuy(order);
                // remaining qty which do not have the best price get added to order book
                if(order->getRemainingQty()>0){
                    orderBook.addOrder(order);
                }
            }
            else if(side == Side::Sell){
                // handles the trade execution till best price or qty becomes 0

                matchSell(order);

                // remaining qty which do not have the best price get added to order book

                if(order->getRemainingQty()>0){
                    orderBook.addOrder(order);
                }
            }
            
            return;
        
        }
    

        //Market order logic only change from limit is that remaining qty is not added to orderbook
        else if(orderType==OrderType::Market){
            if(side==Side::Buy){
                matchBuy(order);
            }else if(side == Side::Sell){
                matchSell(order);
            }
        }
    }

    bool cancelOrder(const std::shared_ptr<Order>& order) {
        if (order->isFilled()){
            return false;
        }

        if (order->getStatus() == OrderStatus::Cancelled){
            return false;
        }

        if (!orderBook.removeOrder(order)){
            return false;
        }

        order->cancelOrder();
        return true;
    }

    std::shared_ptr<Order> modifyOrder(const std::shared_ptr<Order>& order,long long newQty,double newPrice,Side newSide){
        if(!order||order->isFilled()||order->isCancelled()){
            return nullptr;
        }

        if (!cancelOrder(order)){
            return nullptr;
        }
        
        auto newOrder = std::make_shared<Order>(
            generateOrderId(),
            newQty,
            newPrice,
            order->getOrderType(),
            newSide,
            generateSequence(),
            std::chrono::system_clock::now()
        );
        submitOrder(newOrder);

        return newOrder;
    }


private:

    OrderBook orderBook;
    std::vector<std::shared_ptr<Order>> orderHistory;

    std::vector<Trade> trades;

    IdGenerator tradeIdGenerator;
    IdGenerator sequenceGenerator;

    std::string generateTradeId(){
        return "T" + std::to_string(tradeIdGenerator.generate());
    }

    void executeTrade(const std::shared_ptr<Order> &order,const std::shared_ptr<Order> &bestMatchingOrder,Side orderSide){
        long long tradeQty = std::min(order->getRemainingQty(),bestMatchingOrder->getRemainingQty());


        if(orderSide==Side::Buy){
            Trade trade(
                generateTradeId(),
                order->getId(),
                bestMatchingOrder->getId(),
                bestMatchingOrder->getPrice(),
                tradeQty,
                sequenceGenerator.generate(),
                std::chrono::system_clock::now()
            );
            trades.push_back(trade);
                                
            order->execute(tradeQty);
            bestMatchingOrder->execute(tradeQty);
        }else if(orderSide==Side::Sell){
            Trade trade(
                generateTradeId(),
                bestMatchingOrder->getId(),
                order->getId(),
                bestMatchingOrder->getPrice(),
                tradeQty,
                sequenceGenerator.generate(),
                std::chrono::system_clock::now()
            );
            trades.push_back(trade);
                                
            order->execute(tradeQty);
            bestMatchingOrder->execute(tradeQty);
        }
    }

    void matchBuy(const std::shared_ptr<Order> &order){
        auto orderType = order->getOrderType();
        
        while(!(orderBook.isEmptyAsks()||order->isFilled())){
            auto bestSellOrder = orderBook.getBestSellOrder();
            if((order->getPrice()<bestSellOrder->getPrice())&&orderType==OrderType::Limit){
                break;
            }
                    
            executeTrade(order , bestSellOrder,Side::Buy);
            
            orderBook.removeFilledOrders();
        }
    }

    void matchSell(const std::shared_ptr<Order> &order){
        auto orderType = order->getOrderType();

        while(!(orderBook.isEmptyBids()||order->isFilled())){        
            auto bestBuyOrder = orderBook.getBestBuyOrder();

            if((order->getPrice()>bestBuyOrder->getPrice())&&orderType==OrderType::Limit){
                break;
            }

            executeTrade(order,bestBuyOrder,Side::Sell);

            orderBook.removeFilledOrders();
        }
    }

};