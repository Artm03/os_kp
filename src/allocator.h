#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <cstddef> // size_t
#include <iostream>

using namespace std;

class Allocator {
protected:
    size_t m_total_size; // Выделенная аллокатору память
    size_t m_used; // Используемая память
    size_t m_peak; // Максимально используемая память
public:
    Allocator() = default;
    Allocator(const size_t total_size) : m_total_size { total_size }, m_used { 0 }, m_peak { 0 } { }
    virtual ~Allocator() { m_total_size = 0; }
    virtual void* Allocate(const size_t size) = 0; // Выделение памяти
    virtual void Free(void* ptr) = 0; // Освобождение памяти
    virtual void Dumb(){    
        cout << "Total size: " << m_total_size << "\n";
        cout << "Used size: " << m_used << "\n";
        cout << "Peak size: " << m_peak << "\n";
    }
    friend class Benchmark;
    virtual void reset() = 0;
};

#endif /* ALLOCATOR_H */
