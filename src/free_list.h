#ifndef FREEBLOCKSALLOCATOR_H
#define FREEBLOCKSALLOCATOR_H

#include <iostream>
#include "allocator.h"

class free_list_alloc : public Allocator{
public:
    void* buffer_begin = nullptr; // Указатель на начало буффера
    void* buffer_end = nullptr; // Указатель на конец буффера
    explicit free_list_alloc(size_t total_size);
    ~free_list_alloc() {
        ::free(buffer_begin);
    }
    struct Header {
    public:
        size_t size; // Размер блока
        size_t prev_size; // Размер предыдущего блока
        bool is_free;
        inline Header *next() {
            return (Header *) ((char *) (this + 1) + size);
        }
        inline Header *previous() {
            return (Header *) ((char *) this - prev_size) - 1;
        }
    };
    Header *_find_block(size_t size) {
        auto *header = (Header*)(buffer_begin);
        while (!header->is_free || header->size < size) {
            header = header->next();
            if (header >= buffer_end) { return nullptr; }
        }
        return header;
    }
    void _split(Header *header, size_t chunk) {
        size_t block_size = header->size;
        header->size = chunk;
        header->is_free = false;
        if (block_size - chunk >= sizeof(Header)) {
            auto *next = header->next();
            next->prev_size = chunk;
            next->size = block_size - chunk - sizeof(Header);
            next->is_free = true;
            m_used += chunk + sizeof(Header);
            auto *next_next = next->next();
            if (next_next < buffer_end) {
                next_next->prev_size = next->size;
            }
        } else {
            header->size = block_size;
            m_used += block_size;
        }
    }
    bool _validate(void *ptr) {
        auto *header = (Header*)buffer_begin;
        while (header < buffer_end) {
            if (header + 1 == ptr){ return true; }
            header = header->next();
        }
        return false;
    }
public:
    virtual void* Allocate(size_t new_size) override;
    virtual void Free(void* ptr) override;
    virtual void Dumb() override;
    virtual void reset() override;
    void _merge(Header *header);
};

#endif
