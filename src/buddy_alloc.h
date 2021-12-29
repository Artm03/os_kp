#ifndef BUDDY_MEMORY_ALLOCATION_H
#define BUDDY_MEMORY_ALLOCATION_H

#include <vector>
#include <iostream>
#include "allocator.h"

using namespace std;

class buddy_alloc : public Allocator {
public:
    int level; // Число уровней в дереве - степеь двойки
	vector<int> tree; // ДО, в котором хранится информация о блоках памяти
    void _mark_parent(int index);
    void _merge(int index);
    void _dump(int index , int level);
    virtual void reset() override;
public:
    explicit buddy_alloc(size_t size); // Конструктор
    ~buddy_alloc(); // Деструктор
    virtual void* Allocate(size_t size) override; // Выделение памяти
    virtual void Free(void* offset) override; // Освобождение памяти
    virtual void Dumb() override; // Вывод информации о блоках
};

int is_pow_of_2(uint32_t x);
uint32_t next_pow_of_2(uint32_t x);
int _index_offset(int index, int level, int max_level);

#endif
