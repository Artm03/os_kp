#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <time.h>
#include <cstddef>
#include <chrono>
#include <ratio>
#include <vector>

#include "allocator.h" // base class allocator

#if 0
#define OPERATIONS (m_nOperations)
#else
#define OPERATIONS (5000)
#endif

using namespace std;

struct BenchmarkResults {
	size_t Operations;
	chrono::milliseconds Milliseconds;
	double OperationsPerSec;
	double TimePerOperation;
    size_t MemoryPeak;
};

class Benchmark {
public:
    Benchmark() = delete;

    Benchmark(const unsigned int nOperations) : m_nOperations { nOperations } { }

	void SingleAllocation(Allocator* allocator, const size_t size);
	void SingleFree(Allocator* allocator, const size_t size);

	void MultipleAllocation(Allocator* allocator, const vector<size_t>& allocationSizes);
	void MultipleFree(Allocator* allocator, const vector<size_t>& allocationSizes);

	void RandomAllocation(Allocator* allocator, const vector<size_t>& allocationSizes);
	void RandomFree(Allocator* allocator, const vector<size_t>& allocationSizes);

private:
	void PrintResults(const BenchmarkResults& results) const;

	void RandomAllocationAttr(const vector<size_t>& allocationSizes, size_t & size);

	const BenchmarkResults buildResults(size_t nOperations, chrono::milliseconds&& ellapsedTime, const size_t memoryUsed) const;
    
    void SetStartTime() noexcept { Start = chrono::high_resolution_clock::now(); }

    void SetFinishTime() noexcept { Finish = chrono::high_resolution_clock::now(); }

    void SetElapsedTime() noexcept { TimeElapsed = chrono::duration_cast<chrono::milliseconds>(Finish - Start); }

    void StartRound() noexcept { SetStartTime(); }

    void FinishRound() noexcept
    {
        SetFinishTime();
        SetElapsedTime();
    }

private:
	size_t m_nOperations; // Число операций

    chrono::time_point<chrono::high_resolution_clock> Start; // Начальный момент времени
    chrono::time_point<chrono::high_resolution_clock> Finish; // Конечный момент времени

    chrono::milliseconds TimeElapsed;
};

#endif /* BENCHMARK_H */