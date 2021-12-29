#include "benchmark.h"
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <cassert>

void Benchmark::SingleAllocation(Allocator* allocator, const size_t size) {
    cout << "BENCHMARK: ALLOCATION" << "\n";
    cout << "\tSize:     \t" << size << "\n";

    StartRound();

    allocator->reset();

    auto operations = 0u;

    while (operations < m_nOperations) {
        allocator->Allocate(size);
        ++operations;
    }
    FinishRound();
    BenchmarkResults results = buildResults(m_nOperations, move(TimeElapsed), allocator->m_peak);
    PrintResults(results);
}

void Benchmark::SingleFree(Allocator* allocator, const size_t size) {
    cout << "BENCHMARK: ALLOCATION/FREE" << "\n";
    cout << "\tSize:     \t" << size << "\n";

    void* addresses[OPERATIONS];

    StartRound();

    allocator->reset();
    auto operations = 0u;

    while (operations < m_nOperations) {
        addresses[operations] = allocator->Allocate(size);
        operations++;
    }
    operations--;
    while (operations > -1) {
        allocator->Free(addresses[operations]);
        operations--;
    }
    FinishRound();
    BenchmarkResults results = buildResults(m_nOperations, move(TimeElapsed), allocator->m_peak);
    PrintResults(results);
}

void Benchmark::MultipleAllocation(Allocator* allocator, const vector<size_t>& allocationSizes) {
    for (auto i = 0u; i < allocationSizes.size(); ++i) {
        SingleAllocation(allocator, allocationSizes[i]);
    }
}

void Benchmark::MultipleFree(Allocator* allocator, const vector<size_t>& allocationSizes) {
    for (auto i = 0u; i < allocationSizes.size(); ++i) {
        SingleFree(allocator, allocationSizes[i]);
    }
}

void Benchmark::RandomAllocation(Allocator* allocator, const vector<size_t>& allocationSizes) {
    
    // NOTE: Is this actually initializing the RNG? Jose Fernando Lopez Fernandez 11/07/2018 @ 12:54am (UTC)
    srand(1);

    cout << "\tBENCHMARK: ALLOCATION" << "\n";

    StartRound();

    size_t allocation_size;

    allocator->reset();

    auto operations = 0u;

    while (operations < m_nOperations) {
        this->RandomAllocationAttr(allocationSizes, allocation_size);
        allocator->Allocate(allocation_size);
        ++operations;
    }
    
    FinishRound();

    BenchmarkResults results = buildResults(m_nOperations, move(TimeElapsed), allocator->m_peak);
    
    PrintResults(results);
}

void Benchmark::RandomFree(Allocator* allocator, const vector<size_t>& allocationSizes) {
    
    // NOTE: Is this actually initializing the RNG? Jose Fernando Lopez Fernandez 11/07/2018 @ 1:51am (UTC)
    srand(1);

    cout << "\tBENCHMARK: ALLOCATION/FREE" << "\n";

    StartRound();

    void* addresses[OPERATIONS];

    size_t allocation_size;

    allocator->reset();

    auto operations = 0u;

    while (operations < m_nOperations) {
        this->RandomAllocationAttr(allocationSizes, allocation_size);
        addresses[operations] = allocator->Allocate(allocation_size);
        ++operations;
    }

    while (operations) {
        allocator->Free(addresses[--operations]);
    }

    FinishRound();

    BenchmarkResults results = buildResults(m_nOperations, move(TimeElapsed), allocator->m_peak);

    PrintResults(results);

}

void Benchmark::PrintResults(const BenchmarkResults& results) const {
    cout << "\tRESULTS:" << "\n";
    cout << "\t\tOperations:    \t" << results.Operations << "\n";
    cout << "\t\tTime elapsed: \t" << results.Milliseconds.count() << " ms" << "\n";
    cout << "\t\tOp per sec:    \t" << results.OperationsPerSec << " ops/ms" << "\n";
    cout << "\t\tTimer per op:  \t" << results.TimePerOperation << " ms/ops" << "\n";
    cout << "\t\tMemory peak:   \t" << results.MemoryPeak << " bytes" << "\n";

    cout << "\n";
}

const BenchmarkResults Benchmark::buildResults(size_t nOperations, chrono::milliseconds&& elapsedTime, const size_t memoryPeak) const {
    BenchmarkResults results;

    results.Operations = nOperations;
    results.Milliseconds = move(elapsedTime);
    results.OperationsPerSec = results.Operations / static_cast<double>(results.Milliseconds.count());
    results.TimePerOperation = static_cast<double>(results.Milliseconds.count()) / static_cast<double>(results.Operations);
    results.MemoryPeak = memoryPeak;

    return results;
}

void Benchmark::RandomAllocationAttr(const vector<size_t>& allocationSizes, size_t & size) {
    const int r = rand() % allocationSizes.size();
    size = allocationSizes[r];
}