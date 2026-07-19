#include "../include/MatchingEngine.hpp"

// Public

void MatchingEngine::submitOrder(const std::shared_ptr<Order> &order)
{
    auto side = order->getSide();
    auto orderType = order->getOrderType();

    orderHistory.push_back(order);
    // Limit order logic
    if (orderType == OrderType::Limit)
    {
        if (side == Side::Buy)
        {
            // handles the trade execution till best price or qty becomes 0
            matchBuy(order);
            // remaining qty which do not have the best price get added to order book
            if (order->getRemainingQty() > 0)
            {
                orderBook.addOrder(order);
            }
        }
        else if (side == Side::Sell)
        {
            // handles the trade execution till best price or qty becomes 0

            matchSell(order);

            // remaining qty which do not have the best price get added to order book

            if (order->getRemainingQty() > 0)
            {
                orderBook.addOrder(order);
            }
        }

        return;
    }

    // Market order logic only change from limit is that remaining qty is not added to orderbook
    else if (orderType == OrderType::Market)
    {
        if (side == Side::Buy)
        {
            matchBuy(order);
        }
        else if (side == Side::Sell)
        {
            matchSell(order);
        }
    }

    //Same as Limit but remaining unfilled qty is not added to orderbook

    else if (orderType == OrderType::IOC)
    {
        if (side == Side::Buy)
        {
            matchBuy(order);
        }
        else if (side == Side::Sell)
        {
            matchSell(order);
        }
    }
    else if(orderType == OrderType::FillOrKill)
    {
        double price = order->getPrice();

        if(side == Side::Buy)
        {
            auto askLevels = orderBook.getAsksLevels();
            auto qty = order->getQty();
            long long fillingQty=0;

            auto it = askLevels.begin();

            while(it != askLevels.end()&&!orderBook.isEmptyAsks()&&fillingQty<qty&&it->first<=price)
            {
                fillingQty+=it->second;
                ++it;
            }

            if(fillingQty>=qty){
                matchBuy(order);
            }else{
                order->killOrder();
            }

        }
        else if(side == Side::Sell)
        {
            auto bidLevels = orderBook.getBidsLevels();
            auto qty = order->getQty();
            long long fillingQty=0;

            auto it = bidLevels.begin();

            while(it != bidLevels.end()&&!orderBook.isEmptyBids()&&fillingQty<qty&&it->first>=price)
            {
                fillingQty+=it->second;
                ++it;
            }

            if(fillingQty>=qty){
                matchSell(order);
            }else{
                order->killOrder();
            }

        }
    }

}

bool MatchingEngine::cancelOrder(const std::shared_ptr<Order> &order)
{
    if (order->isFilled())
    {
        return false;
    }

    if (order->getStatus() == OrderStatus::Cancelled)
    {
        return false;
    }

    if (!orderBook.removeOrder(order))
    {
        return false;
    }

    order->cancelOrder();
    return true;
}

std::shared_ptr<Order> MatchingEngine::modifyOrder(const std::shared_ptr<Order> &order, long long newQty, double newPrice)
{
    if (!order || order->isFilled() || order->isCancelled())
    {
        return nullptr;
    }

    if (!cancelOrder(order))
    {
        return nullptr;
    }

    auto newOrder = std::make_shared<Order>(
        generateOrderId(),
        newQty,
        newPrice,
        order->getOrderType(),
        order->getSide(),
        generateSequence(),
        std::chrono::system_clock::now());
    submitOrder(newOrder);

    return newOrder;
}

// Private

void MatchingEngine::executeTrade(const std::shared_ptr<Order> &order, const std::shared_ptr<Order> &bestMatchingOrder, Side orderSide)
{
    long long tradeQty = std::min(order->getRemainingQty(), bestMatchingOrder->getRemainingQty());

    if (orderSide == Side::Buy)
    {
        Trade trade(
            generateTradeId(),
            order->getId(),
            bestMatchingOrder->getId(),
            bestMatchingOrder->getPrice(),
            tradeQty,
            sequenceGenerator.generate(),
            std::chrono::system_clock::now());
        trades.push_back(trade);

        orderBook.updateAskLevel(bestMatchingOrder->getPrice(),tradeQty);
        order->execute(tradeQty);
        bestMatchingOrder->execute(tradeQty);

    }
    else if (orderSide == Side::Sell)
    {
        Trade trade(
            generateTradeId(),
            bestMatchingOrder->getId(),
            order->getId(),
            bestMatchingOrder->getPrice(),
            tradeQty,
            sequenceGenerator.generate(),
            std::chrono::system_clock::now());
        trades.push_back(trade);
        
        orderBook.updateBidLevel(bestMatchingOrder->getPrice(),tradeQty);

        order->execute(tradeQty);
        bestMatchingOrder->execute(tradeQty);
    }
}

void MatchingEngine::matchBuy(const std::shared_ptr<Order> &order)
{
    auto orderType = order->getOrderType();

    while (!(orderBook.isEmptyAsks() || order->isFilled()))
    {
        auto bestSellOrder = orderBook.getBestSellOrder();
        if ((order->getPrice() < bestSellOrder->getPrice()) && (orderType == OrderType::Limit || orderType == OrderType::IOC || orderType == OrderType::FillOrKill))
        {
            break;
        }

        executeTrade(order, bestSellOrder, Side::Buy);

        orderBook.removeFilledOrders();
    }
}

void MatchingEngine::matchSell(const std::shared_ptr<Order> &order)
{
    auto orderType = order->getOrderType();

    while (!(orderBook.isEmptyBids() || order->isFilled()))
    {
        auto bestBuyOrder = orderBook.getBestBuyOrder();

        if ((order->getPrice() > bestBuyOrder->getPrice()) && (orderType == OrderType::Limit || orderType == OrderType::IOC|| orderType == OrderType::FillOrKill))
        {
            break;
        }

        executeTrade(order, bestBuyOrder, Side::Sell);

        orderBook.removeFilledOrders();
    }
}