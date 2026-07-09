#pragma once
#include <vector>
#include <algorithm>
#include <chrono>

#include "Order.hpp"
#include "OrderBook.hpp"
#include "Trade.hpp"
#include "IdGenerator.hpp"

class MatchingEngine{
public:

    std::string generateTradeId(){
        return "T" + std::to_string(tradeIdGenerator.generate());
    }

    // Getters
    const auto &getOrderBook() const{
        return orderBook;
    }

    const auto &getTrades() const{
        return trades;
    }

    void submitOrder(Order &order){
        auto side = order.getSide();
        auto price = order.getPrice();
        auto orderType = order.getOrderType();


        // Limit order logic
        if(orderType==OrderType::Limit){
            if (side == Side::Buy){
                // handles the trade execution till best price or qty becomes 0

                while(!(orderBook.isEmptyAsks()||order.isFilled())){
                    auto &bestSellOrder = *orderBook.getBestSellOrder();
                    auto remainingQty = order.getRemainingQty();
                    auto sellOrderQty = bestSellOrder.getRemainingQty();
                    auto bestAsk = bestSellOrder.getPrice();
                    auto currentTime = std::chrono::system_clock::now();

                    if((price<bestSellOrder.getPrice())){
                        break;
                    }
                    
                    long long tradeQty = std::min(remainingQty,sellOrderQty);


                    Trade trade(
                        generateTradeId(),
                        order.getId(),
                        bestSellOrder.getId(),
                        bestAsk,
                        tradeQty,
                        sequenceGenerator.generate(),
                        currentTime
                    );
                    trades.push_back(trade);
                    
                    order.execute(tradeQty);
                    bestSellOrder.execute(tradeQty);
                    
                    orderBook.removeFilledOrders();
                }

                // remaining qty which do not have the best price get added to order book
                if(order.getRemainingQty()>0){
                    orderBook.addOrder(order);
                }
            }
            else if(side == Side::Sell){
                // handles the trade execution till best price or qty becomes 0

                while(!(orderBook.isEmptyBids()||order.isFilled())){
                    
                    auto &bestBuyOrder = *orderBook.getBestBuyOrder();
                    auto remainingQty = order.getRemainingQty();
                    auto buyOrderQty = bestBuyOrder.getRemainingQty();
                    auto bestBid = bestBuyOrder.getPrice();
                    auto currentTime = std::chrono::system_clock::now();

                    if((price>bestBuyOrder.getPrice())){
                        break;
                    }
                    
                    long long tradeQty = std::min(remainingQty,buyOrderQty);

                    Trade trade(
                        generateTradeId(),
                        bestBuyOrder.getId(),
                        order.getId(),
                        bestBid,
                        tradeQty,
                        sequenceGenerator.generate(),
                        currentTime
                    );
                
                    trades.push_back(trade);

                    order.execute(tradeQty);
                    bestBuyOrder.execute(tradeQty);                    
                    
                    orderBook.removeFilledOrders();
                }

                // remaining qty which do not have the best price get added to order book

                if(order.getRemainingQty()>0){
                    orderBook.addOrder(order);
                }
            }
            
            return;
        
        }
    

        //Market order logic only change from limit is that remaining qty is not added to orderbook
        else if(orderType==OrderType::Market){
            
            if(side==Side::Buy){
                while(!(orderBook.isEmptyAsks()||order.isFilled())){
                    auto &bestSellOrder = *orderBook.getBestSellOrder();
                    auto remainingQty = order.getRemainingQty();
                    auto sellOrderQty = bestSellOrder.getRemainingQty();
                    auto bestAsk = bestSellOrder.getPrice();
                    auto currentTime = std::chrono::system_clock::now();

                    long long tradeQty = std::min(remainingQty,sellOrderQty);


                    Trade trade(
                        generateTradeId(),
                        order.getId(),
                        bestSellOrder.getId(),
                        bestAsk,
                        tradeQty,
                        sequenceGenerator.generate(),
                        currentTime
                    );
                    trades.push_back(trade);
                    
                    order.execute(tradeQty);
                    bestSellOrder.execute(tradeQty);
                    
                    orderBook.removeFilledOrders();
                }
            }
            
            else if(side == Side::Sell){
                // handles the trade execution till best price or qty becomes 0
                while(!(orderBook.isEmptyBids()||order.isFilled())){
                    auto &bestBuyOrder = *orderBook.getBestBuyOrder();
                    auto remainingQty = order.getRemainingQty();
                    auto buyOrderQty = bestBuyOrder.getRemainingQty();
                    auto bestBid = bestBuyOrder.getPrice();
                    auto currentTime = std::chrono::system_clock::now();
                    
                    long long tradeQty = std::min(remainingQty,buyOrderQty);

                    Trade trade(
                        generateTradeId(),
                        bestBuyOrder.getId(),
                        order.getId(),
                        bestBid,
                        tradeQty,
                        sequenceGenerator.generate(),
                        currentTime
                    );
                
                    trades.push_back(trade);

                    order.execute(tradeQty);
                    bestBuyOrder.execute(tradeQty);                    
                    
                    orderBook.removeFilledOrders();
                }
            }
        }
    }


private:

    OrderBook orderBook;

    std::vector<Trade> trades;

    IdGenerator tradeIdGenerator;
    IdGenerator sequenceGenerator;
};