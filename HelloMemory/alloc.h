#pragma once

void *operator new(size_t size);


void *operator new[] (size_t size);
void operator delete(void * p);

void *operator new[](size_t size);

void operator delete[](void * p);

void* mem_alloc(size_t size);
void med_free(void* p);