#include <iostream>
#include <vector>
#include "buddy_alloc.h"
#include "free_list.h"
#include "benchmark.h"

using namespace std;

int main() {
    size_t B = static_cast<size_t>(1e8);
    const vector<size_t> ALLOCATION_SIZES {32, 64, 256, 512, 1024, 2048, 4096};
    Benchmark benchmark(OPERATIONS);
    try {
        Allocator * Freelist = new free_list_alloc(B);
        cout << "FREE LIST" << "\n";
        benchmark.MultipleAllocation(Freelist, ALLOCATION_SIZES);
        benchmark.MultipleFree(Freelist, ALLOCATION_SIZES);
    } catch(runtime_error& err) {
        cout << err.what() << "\n";
    } catch(invalid_argument& err) {
        cout << err.what() << "\n";
    }
    try {
        Allocator * Buddy = new buddy_alloc(B);
        cout << "BUDDY" << "\n";
        benchmark.MultipleAllocation(Buddy, ALLOCATION_SIZES);
        benchmark.MultipleFree(Buddy, ALLOCATION_SIZES);
    } catch(runtime_error& err) {
        cout << err.what() << "\n";
    } catch(invalid_argument& err) {
        cout << err.what() << "\n";
    }

	return 0;
}
