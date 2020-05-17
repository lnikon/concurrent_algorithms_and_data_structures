#pragma once

#include <thread>
#include <mutex>
#include <vector>

static const auto NUMCPUS = std::thread::hardware_concurrency();

struct counter_t {
    int                     global;
    std::mutex              glock;
    std::vector<int>        local;
    std::vector<std::mutex> llock;
    int                     threashold;

    counter_t(const int _threashold)
        : global(0)
        , local(NUMCPUS)
        , llock(NUMCPUS)
        , threashold(_threashold)
    { }

    void update(const int threadID, int amt)
    {
        int cpu = threadID % NUMCPUS;
        llock[cpu].lock();
        local[cpu] += amt;
        if (local[cpu] >= threashold) {
            glock.lock();
            global += local[cpu];
            glock.unlock();
            local[cpu] = 0;
        }
        llock[cpu].unlock();
    }

    int get() {
        glock.lock();
        int val = global;
        glock.unlock();
        return val;
    }
};


