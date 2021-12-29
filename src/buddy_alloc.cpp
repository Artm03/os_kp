#include "buddy_alloc.h"

#define NODE_UNUSED 0  // Свободный блок
#define NODE_USED 1	// Занятый блок
#define NODE_SPLIT 2 // Разделённый блок
#define NODE_FULL 3 // Заполненный блок

buddy_alloc::buddy_alloc(size_t sz) { // Конструктор, выделяет память на ДО
	int lvl = 0;
	sz = next_pow_of_2(sz);
	m_total_size = sz;
	while(sz > 1) {
		sz /= 2;
		lvl++;
	}
	level = lvl;
    m_used = 0;
    m_peak = 0;
    tree.resize(m_total_size * 2 - 1, NODE_UNUSED); // size * 2 - 1 - общее величина ДО
	//cout << "Total memory allocated is: " << size << "\n";
}

void buddy_alloc::reset() {
    m_used = 0;
    m_peak = 0;
    tree.resize(m_total_size * 2 - 1, NODE_UNUSED); // size * 2 - 1 - общее величина ДО
}

buddy_alloc::~buddy_alloc() {
    level = 0;
    tree.clear();
}

int is_pow_of_2(uint32_t x) {
	return !(x & (x-1));
}

int _index_offset(int index, int level, int max_level) { // Смещение от начала блока 
	return ((index + 1) - (1 << level)) << (max_level - level);
}

uint32_t next_pow_of_2(uint32_t x) {
	if (is_pow_of_2(x)) return x;
	int pow_of_2 = 1;
    while (pow_of_2 < x) {
        pow_of_2 *= 2;
    }
	return pow_of_2;
}

void buddy_alloc::_mark_parent(int index) {  // Обновить информацию о родителе, если это необходимо
	for (;;) {
		int buddy = index - 1 + (index & 1) * 2; // Индекс "приятеля"
		if (buddy > 0 && (tree[buddy] == NODE_USED || tree[buddy] == NODE_FULL)) {
			index = (index + 1) / 2 - 1; // Индекс предка
			tree[index] = NODE_FULL;
		} else {
			return;
		}
	}
}

void* buddy_alloc::Allocate(size_t s) { // Выделение запрошенной памяти
	size_t size;
	if (s == 0) {
		throw runtime_error("Size should be more then 0");
	} 
	size = (int)next_pow_of_2(s);
	size_t length = m_total_size;
	if (size > length) { // Если запрашивается слишком много памяти
		throw runtime_error("Insufficient memory");
    }
	int index = 0;
	int lvl = 0;
	while (index >= 0) { // Происходит поиск наиболее подходящего блока
		if (size == length) { // Блок найден
			if (tree[index] == NODE_UNUSED) { // Блок свободен
				tree[index] = NODE_USED;
				_mark_parent(index);
            	m_used += length;
				m_peak = max(m_used, m_peak);
				return &tree[index];
			}
		} else { // Если блок не был найден, т.е. size < length
			switch (tree[index]) {
                case NODE_USED:
                    break;
                case NODE_FULL:
                    break;
                case NODE_UNUSED:
                    tree[index] = NODE_SPLIT;
                    tree[index * 2 + 1] = NODE_UNUSED;
                    tree[index * 2 + 2] = NODE_UNUSED;
                default:
                    index = index * 2 + 1; // Аналогично спуску на уровень ниже в дереве
                    length /= 2;
                    lvl++;
                    continue;
			}
		}
		if (index & 1) {
			index++;
			continue;
		}
		for (;;) {
			lvl--;
			length *= 2;
			index = (index + 1) / 2 - 1; // Аналогично возврату на уровень выше в дереве
			if (index < 0)
				throw runtime_error("Insufficient memory");
			if (index & 1) {
				index++;
				break;
			}
		}
	}
	throw runtime_error("Insufficient memory");
}

void buddy_alloc::_merge(int index) { // Объединение "приятелей" в блоки большего размера
	for (;;) {
		int buddy = index - 1 + (index & 1) * 2;
		if (buddy < 0 || tree[buddy] != NODE_UNUSED) {
			tree[index] = NODE_UNUSED;
			while (((index = (index + 1) / 2 - 1) >= 0) && tree[index] == NODE_FULL){
				tree[index] = NODE_SPLIT;
			}
			return;
		}
		index = (index + 1) / 2 - 1;
	}
}

void buddy_alloc::Free(void* offset) { // Очищает блок по адресу (задаётся смещение от начала блоков)
	int index = ((int*)offset - &tree[0]);
	if (index >= (1 << level)) {
        throw out_of_range("Wrong ptr");
    }
	switch (*((int*)offset)) {
		case NODE_USED:
			_merge(index);
			int lvl;
			lvl = 0;
			for (;;) {
				if ((1 << lvl) <= index + 1 && (1 << (lvl + 1)) > index + 1) {
					break;
				}
				lvl++;
			}
			int length;
			length = 1 << (level - lvl);
			m_used -= length;
			return;
		case NODE_UNUSED:
			throw runtime_error("Memory already is cleared");
		default:
			throw runtime_error("Can't free this memory");
	}
}

void buddy_alloc::_dump(int index, int lvl) { 
	switch (tree[index]) { 
        // В каждой скобке находятся два параметра: смещение и размер блока
        case NODE_UNUSED: // Если память не используется, то записывается в круглых скобках
            cout << " ( " << _index_offset(index, lvl, level) << " : " << (1 << (level - lvl)) << " ) ";
            break;
        case NODE_USED: // Если память используется, то записывается в квадратных скобках
            cout << " [" << _index_offset(index, lvl, level) << " : " << (1 << (level - lvl)) << "] "; 
            break;
        case NODE_FULL: // Рекурсивный вызов для заполненных блоков
            cout << " { ";
            _dump(index * 2 + 1 , lvl + 1);
            _dump(index * 2 + 2 , lvl + 1);
            cout << " } ";
            break;
        default: // Рекурсивный вызов для свободных блоков
            cout << " ( ";
            _dump(index * 2 + 1 , lvl + 1);
            _dump(index * 2 + 2 , lvl + 1);
            cout << " ) ";
            break;
	}
}

void buddy_alloc::Dumb() { // Вывод информации о блоках памяти
	cout << "Total size: " << (1 << level) << "\n";
    cout << "Used size: " << m_used << "\n";
    cout << "Peak size: " << m_peak << "\n";
	//_dump(0, 0);
	//cout << "\n";
}
