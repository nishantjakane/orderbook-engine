#include "../include/OrderBook.hpp"
#include <algorithm>

void OrderBook::addOrder(const std::shared_ptr<Order> &order)
{
    auto side = order->getSide();
    auto price = order->getPrice();

    if (side == Side::Buy)
    {
        bids[price].push_back(order);
    }
    else if (side == Side::Sell)
    {
        asks[price].push_back(order);
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

std::vector<std::pair<double, long long>> OrderBook::getBidsLevels() const
{
    std::vector<std::pair<double, long long>> levels;
    for (const auto &level : bids)
    {
        auto &orders = level.second;
        auto &levelPrice = level.first;
        long long levelQty = 0;
        for (auto &order : orders)
        {
            levelQty += order->getRemainingQty();
        }

        levels.push_back({levelPrice, levelQty});
    }

    return levels;
}

std::vector<std::pair<double, long long>> OrderBook::getAsksLevels() const
{
    std::vector<std::pair<double, long long>> levels;
    for (const auto &level : asks)
    {
        auto &orders = level.second;
        auto &levelPrice = level.first;
        long long levelQty = 0;
        for (auto &order : orders)
        {
            levelQty += order->getRemainingQty();
        }

        levels.push_back({levelPrice, levelQty});
    }

    return levels;
}

bool OrderBook::removeOrder(const std::shared_ptr<Order> &order)
{
    if (order->getSide() == Side::Buy)
    {
        auto levelIt = bids.find(order->getPrice());
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
        auto levelIt = asks.find(order->getPrice());
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
