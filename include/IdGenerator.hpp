#pragma once

#include <atomic>

class IdGenerator{
public:
    long long generate(){
        return counter++;
    }


private:
    std::atomic<long long> counter{1};
};