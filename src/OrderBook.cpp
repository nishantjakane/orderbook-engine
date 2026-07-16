#include "../include/OrderBook.hpp"
#include <algorithm>

void OrderBook::addOrder(const std::shared_ptr<Order> &order)
{
    auto side = order->getSide();
    auto price = order->getPrice();

    if (side == Side::Buy)
    {
        bids[price].push_back(order);
        bidLevels[price]+=order->getRemainingQty();
    }
    else if (side == Side::Sell)
    {
        asks[price].push_back(order);
        askLevels[price]+=order->getRemainingQty();
    }
}

void OrderBook::removeFilledOrders()
{
    for (auto levelit = bids.begin(); levelit != bids.end();)
    {
        auto &level = *levelit;
        auto &orders = level.second;

        for (auto it = orders.begin(); it != orders.end();)
        {
            if ((*it)->isFilled())
            {
                it = orders.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (orders.empty())
        {
            levelit = bids.erase(levelit);
        }
        else
        {
            ++levelit;
        }
    }

    for (auto levelit = asks.begin(); levelit != asks.end();)
    {
        auto &level = *levelit;
        auto &orders = level.second;

        for (auto it = orders.begin(); it != orders.end();)
        {
            if ((*it)->isFilled())
            {
                it = orders.erase(it);
            }
            else
            {
                ++it;
            }
        }

        if (orders.empty())
        {
            levelit = asks.erase(levelit);
        }
        else
        {
            ++levelit;
        }
    }
}

bool OrderBook::removeOrder(const std::shared_ptr<Order> &order)
{
    double price = order->getPrice();
    if (order->getSide() == Side::Buy)
    {
        auto levelIt = bids.find(price);
        if (levelIt == bids.end())
        {
            return false;
        }
        auto &orders = levelIt->second;
        for (auto it = orders.begin(); it != orders.end();)
        {
            if (*it == order)
            {
                it = orders.erase(it);
                updateBidLevel(price,order->getRemainingQty());

                if (orders.empty())
                {
                    bids.erase(levelIt);
                }
                return true;
            }
            else
            {
                ++it;
            }
        }
    }
    else if (order->getSide() == Side::Sell)
    {
        auto levelIt = asks.find(price);
        if (levelIt == asks.end())
        {
            return false;
        }
        auto &orders = levelIt->second;
        for (auto it = orders.begin(); it != orders.end();)
        {
            if (*it == order)
            {
                it = orders.erase(it);

                updateAskLevel(price,order->getRemainingQty());

                if (orders.empty())
                {
                    asks.erase(levelIt);
                }
                return true;
            }
            else
            {
                ++it;
            }
        }
    }

    return false;
}

// Levels helper funcs
void OrderBook::updateBidLevel(double price,long long deltaQty){
    bidLevels[price]-=deltaQty;
    
    
    if (bidLevels[price]==0){
        bidLevels.erase(price);
    }
}

void OrderBook::updateAskLevel(double price,long long deltaQty){
    askLevels[price]-=deltaQty;

    if(askLevels[price]==0){
        askLevels.erase(price);
    }
}