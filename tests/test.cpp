#include <iostream>
#include <memory>
#include <chrono>

#include "../includes/Order.hpp"
#include "../includes/MatchingEngine.hpp"
#include "../includes/IdGenerator.hpp"

std::shared_ptr<Order> makeOrder(
    MatchingEngine& engine,
    long long qty,
    double price,
    OrderType type,
    Side side)
{
    return std::make_shared<Order>(
        engine.generateOrderId(),
        qty,
        price,
        type,
        side,
        engine.generateSequence(),
        std::chrono::system_clock::now()
    );
}

void printBook(const MatchingEngine& engine)
{
    const auto& book = engine.getOrderBook();

    std::cout << "\nASKS\n";
    for (auto level : book.getAsksLevels())
        std::cout << level.first << " -> " << level.second << '\n';

    std::cout << "\nBIDS\n";
    for (auto level : book.getBidsLevels())
        std::cout << level.first << " -> " << level.second << '\n';

    std::cout << "---------------------------\n";
}

int main()
{
    //=========================================================
    std::cout << "\n===== TEST 1 : Cancel Only Order =====\n";

    {
        MatchingEngine engine;

        auto s1 = makeOrder(engine,100,100,OrderType::Limit,Side::Sell);

        engine.submitOrder(s1);

        printBook(engine);

        std::cout << "Cancel = " << engine.cancelOrder(s1) << '\n';

        printBook(engine);

        std::cout << "Status = "
                  << (s1->getStatus()==OrderStatus::Cancelled)
                  << "\n";
    }

    //=========================================================
    std::cout << "\n===== TEST 2 : Cancel Middle FIFO Order =====\n";

    {
        MatchingEngine engine;

        auto s1 = makeOrder(engine,10,100,OrderType::Limit,Side::Sell);
        auto s2 = makeOrder(engine,20,100,OrderType::Limit,Side::Sell);
        auto s3 = makeOrder(engine,30,100,OrderType::Limit,Side::Sell);
        std::cout << s1->getId() << '\n';
        std::cout << s2->getId() << '\n';
        std::cout << s3->getId() << '\n';

        engine.submitOrder(s1);
        engine.submitOrder(s2);
        engine.submitOrder(s3);

        engine.cancelOrder(s2);

        printBook(engine);

        std::cout << "Best Sell = "
                  << engine.getOrderBook().getBestSellOrder()->getId()
                  << '\n';
    }

    //=========================================================
    std::cout << "\n===== TEST 3 : Cancel Last Order =====\n";

    {
        MatchingEngine engine;

        auto s1 = makeOrder(engine,50,100,OrderType::Limit,Side::Sell);

        engine.submitOrder(s1);

        engine.cancelOrder(s1);

        printBook(engine);
    }

    //=========================================================
    std::cout << "\n===== TEST 4 : Cancel Buy Order =====\n";

    {
        MatchingEngine engine;

        auto b1 = makeOrder(engine,40,100,OrderType::Limit,Side::Buy);
        auto b2 = makeOrder(engine,30,100,OrderType::Limit,Side::Buy);

        engine.submitOrder(b1);
        engine.submitOrder(b2);

        engine.cancelOrder(b1);

        printBook(engine);
    }

    //=========================================================
    std::cout << "\n===== TEST 5 : Cancel Twice =====\n";

    {
        MatchingEngine engine;

        auto s1 = makeOrder(engine,50,100,OrderType::Limit,Side::Sell);

        engine.submitOrder(s1);

        std::cout << engine.cancelOrder(s1) << '\n';
        std::cout << engine.cancelOrder(s1) << '\n';
    }

    //=========================================================
    std::cout << "\n===== TEST 6 : Cancel Filled Order =====\n";

    {
        MatchingEngine engine;

        auto sell = makeOrder(engine,100,100,OrderType::Limit,Side::Sell);
        auto buy  = makeOrder(engine,100,100,OrderType::Limit,Side::Buy);

        engine.submitOrder(sell);
        engine.submitOrder(buy);

        std::cout << "Cancel Filled = "
                  << engine.cancelOrder(sell)
                  << '\n';

        std::cout << "Filled = "
                  << sell->isFilled()
                  << '\n';
    }

    //=========================================================
    std::cout << "\n===== TEST 7 : Partial Fill then Cancel =====\n";

    {
        MatchingEngine engine;

        auto sell = makeOrder(engine,100,100,OrderType::Limit,Side::Sell);
        auto buy  = makeOrder(engine,40,100,OrderType::Limit,Side::Buy);

        engine.submitOrder(sell);
        engine.submitOrder(buy);

        std::cout << "Remaining = "
                  << sell->getRemainingQty()
                  << '\n';

        std::cout << "Cancel = "
                  << engine.cancelOrder(sell)
                  << '\n';

        printBook(engine);

        std::cout << "Status = "
                  << (sell->getStatus()==OrderStatus::Cancelled)
                  << '\n';
    }

    return 0;
}