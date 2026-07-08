#pragma once
#include <vector>
#include <algorithm>

#include "Order.hpp"
#include "OrderBook.hpp"
#include "Trade.hpp"
#include "IdGenerator.hpp"

class MatchingEngine{
public:

    std::string tradeIdString(){
        return "T" + std::to_string(tradeIdGenerator.generate());
    }


    void submitOrder(Order &order){
        auto side = order.getSide();
        auto price = order.getPrice();
        auto orderType = order.getOrderType();

        if(orderType==OrderType::Limit){
            if (side == Side::Buy){
                if(orderBook.isEmptyAsks()){
                    orderBook.addOrder(order);
                    return;
                }

                auto &bestSellOrder = *orderBook.getBestSellOrder();
                
                // handles the trade execution till best price or qty becomes 0

                while(!orderBook.isEmptyAsks()&&price>=bestSellOrder.getPrice()&&order.getRemainingQty()>0){
                    auto &bestSellOrder = *orderBook.getBestSellOrder();
                    auto remainingQty = order.getRemainingQty();
                    auto sellOrderQty = bestSellOrder.getRemainingQty();
                    auto bestAsk = bestSellOrder.getPrice();
                    auto currentTime = std::chrono::system_clock::now();
                    
                    if(remainingQty<=sellOrderQty){ 
                        Trade trade(
                            tradeIdString(),
                            order.getId(),
                            bestSellOrder.getId(),
                            bestAsk,
                            remainingQty,
                            sequenceGenerator.generate(),
                            currentTime
                        );

                        trades.push_back(trade);

                        order.execute(remainingQty);
                        bestSellOrder.execute(remainingQty);
                    
                    }else{
                        Trade trade(
                            tradeIdString(),
                            order.getId(),
                            bestSellOrder.getId(),
                            bestAsk,
                            sellOrderQty,
                            sequenceGenerator.generate(),
                            currentTime
                        );

                        trades.push_back(trade);
                    
                        order.execute(sellOrderQty);
                        bestSellOrder.execute(sellOrderQty);
                    
                    }
                    
                    orderBook.removeFilledOrders();
                }

                // remaining qty which do not have the best price get added to order book
                if(order.getRemainingQty()>0){
                    orderBook.addOrder(order);
                }
            }
            else if(side == Side::Sell){
                if(orderBook.isEmptyBids()){
                    orderBook.addOrder(order);
                    return;
                }

                auto &bestBuyOrder= *orderBook.getBestBuyOrder();

                // handles the trade execution till best price or qty becomes 0

                while(!orderBook.isEmptyBids()){
                    
                    auto &bestBuyOrder = *orderBook.getBestBuyOrder();
                    auto remainingQty = order.getRemainingQty();
                    auto buyOrderQty = bestBuyOrder.getRemainingQty();
                    auto bestBid = bestBuyOrder.getPrice();
                    auto currentTime = std::chrono::system_clock::now();

                    if(!(price<=bestBuyOrder.getPrice()||order.getRemainingQty()>0)){
                        break;
                    }
                    
                    if(remainingQty<=buyOrderQty){
                        Trade trade(
                            tradeIdString(),
                            bestBuyOrder.getId(),
                            order.getId(),
                            bestBid,
                            remainingQty,
                            sequenceGenerator.generate(),
                            currentTime
                        );
                    
                        trades.push_back(trade);

                        order.execute(remainingQty);
                        bestBuyOrder.execute(remainingQty);
                    
                    }else{
                        Trade trade(
                            tradeIdString(),
                            bestBuyOrder.getId(),
                            order.getId(),
                            bestBid,
                            buyOrderQty,
                            sequenceGenerator.generate(),
                            currentTime
                        );

                        trades.push_back(trade);
                    
                        order.execute(buyOrderQty);
                        bestBuyOrder.execute(buyOrderQty);
                    
                    }
                    
                    orderBook.removeFilledOrders();
                }

                // remaining qty which do not have the best price get added to order book

                if(order.getRemainingQty()>0){
                    orderBook.addOrder(order);
                }
            }
            
            return;
        
        }
    }

private:

    OrderBook orderBook;

    std::vector<Trade> trades;

    IdGenerator orderIdGenerator;
    IdGenerator tradeIdGenerator;
    IdGenerator sequenceGenerator;
};