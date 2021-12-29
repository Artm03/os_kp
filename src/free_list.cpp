#include <stdlib.h>
#include <cassert>
#include <limits>
#include <algorithm>
#include "free_list.h"

using namespace std;

free_list_alloc::free_list_alloc(size_t size) {
    if (buffer_begin != nullptr) {
        free(buffer_begin);
        buffer_begin = nullptr;
    }
    buffer_begin = malloc(size);
    if (buffer_begin == nullptr) {
        cerr << "Can't allocate memory\n";
        return;
    }
    m_total_size = size;
    buffer_end = (void*)((char*)(buffer_begin) + m_total_size);
    auto *header = (Header *) buffer_begin;
    header->is_free = true;
    header->size = (m_total_size - sizeof(Header));
    header->prev_size = 0;
    m_used = sizeof(Header);
    m_peak = m_used;
}

void* free_list_alloc::Allocate(std::size_t new_size) {
    if (new_size == 0) throw runtime_error("Size must be bigger than 0.");
    if (new_size > m_total_size - m_used) throw runtime_error("Insufficient memory");
    auto *header = _find_block(new_size);
    if (header == nullptr) throw runtime_error("No required blocks.");
    _split(header, new_size);
    m_peak += new_size;
    return header + 1; //find_block returns header*
}

void free_list_alloc::Free(void* ptr) {
    if (!_validate(ptr)) return; // if address is not valid 
    auto *header = (Header*)ptr - 1;
    header->is_free = true;
    m_used -= header->size;
    _merge(header);
}

void free_list_alloc::reset() {
    if (buffer_begin != nullptr) {
        free(buffer_begin);
        buffer_begin = nullptr;
    }
    buffer_begin = malloc(m_total_size);
    if (buffer_begin == nullptr) {
        cerr << "Can't allocate memory\n";
        return;
    }
    buffer_end = (void*)((char*)(buffer_begin) + m_total_size);
    auto *header = (Header *) buffer_begin;
    header->is_free = true;
    header->size = (m_total_size - sizeof(Header));
    header->prev_size = 0;
    m_used = sizeof(Header);
    m_peak = m_used;
}

void free_list_alloc::_merge(Header *header) {
    auto *previous = header->previous();
    if ((header != buffer_begin) && (previous->is_free)) { //if prev free
        if (header->next() < buffer_end) {
            header->next()->prev_size += previous->size + sizeof(Header);
        }
        previous->size += header->size + sizeof(Header);
        m_used -= sizeof(Header);
        header = previous;
    }
    auto *next = header->next();
    if (header != buffer_end && next->is_free) { // if next free
        header->size += sizeof(Header) + header->next()->size;
        m_used -= sizeof(Header);
        if (next != buffer_end) { 
            next->prev_size = header->size;
        }
    }
}

void free_list_alloc::Dumb() {
    cout << "Total size: " << m_total_size << "\n";
    cout << "Used size: " << m_used << "\n";
    cout << "Peak size: " << m_peak << "\n";
}
