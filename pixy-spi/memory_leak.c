#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

struct memory_info {
	void* addr;
	size_t size;
	const char* file;
	int line;
	struct memory_info *prev, *next;
}; 
typedef struct memory_info memory_info_t;

size_t max_memory_allocated = 0;
size_t memory_allocated = 0;
memory_info_t *mem_info = 0;

void add_memory_info(memory_info_t* info) {
	if(!mem_info){
		info->prev = info->next = NULL;
		mem_info = info;
	}
	else{
		mem_info->prev = info;
		info->prev = NULL;
		info->next = mem_info;
		mem_info = info;
	}
	memory_allocated += info->size;
	if(memory_allocated > max_memory_allocated){
		max_memory_allocated = memory_allocated;
	}
}

memory_info_t* find_memory(void*p) {
	memory_info_t *this = mem_info;
	while(this){
		if (this->addr == p){
			return this;
		}
		this = this->next;
	}
	return NULL;
}

void remove_memory(memory_info_t* info){
	if(info == mem_info){
		mem_info = info->next;
		if(info->next){
			info->next->prev = NULL;
		}
	}else{
		info->prev->next = info->next;
		if(info->next){
			info->next->prev = info->prev;
		}
	}

	memory_allocated -= info->size;
}

void *debug_malloc(size_t size, const char* file, int line) {
	void *p = malloc(size);

	if (p == NULL) {
		return NULL;
	}

	memory_info_t * info = (memory_info_t*)malloc(sizeof(memory_info_t));
	info->addr = p;
	info->size = size;
	info->file = file;
	info->line = line;
	info->prev = info->next = 0;
	add_memory_info(info);

	return p;
}

void *debug_calloc(size_t count, size_t size, const char* file, int line) {
	void *p = calloc(count, size);

	if (p == NULL) {
		return NULL;
	}

	memory_info_t * info = (memory_info_t*)malloc(sizeof(memory_info_t));
	info->addr = p;
	info->size = size;
	info->file = file;
	info->line = line;
	info->prev = info->next = 0;
	add_memory_info(info);

	return p;
}

void *debug_realloc(void *ptr, size_t size, const char* file, int line) {
	void *p = realloc(ptr, size);

	if (p == NULL) {
		return NULL;
	}

	memory_info_t * info = find_memory(ptr);
	if(!info){
		printf("Memory Debug: trying to realloc a memory that does not exist: %p, %s:%d\n", p, file, line);
	}else{
		remove_memory(info);
		memory_info_t * info = (memory_info_t*)malloc(sizeof(memory_info_t));
		info->addr = p;
		info->size = size;
		info->file = file;
		info->line = line;
		info->prev = info->next = 0;
		add_memory_info(info);
	}

	return p;
}

void debug_free(void *p, const char* file, int line) {
	memory_info_t * info = find_memory(p);
	if(!info && p){
		printf("Memory Debug: trying to free a memory that does not exist: %p, %s:%d\n", p, file, line);
	}

	free(p);

	if(info){
		remove_memory(info);
		free(info);
	}
}

void debug_mem_finalize(){
	while(mem_info){
		printf("Memory Debug: memory leak: %p, %s:%d\n", mem_info->addr, mem_info->file, mem_info->line);
		memory_info_t * t = mem_info->next;
		free(mem_info);
		mem_info = t;
	}
	printf("Maximum memory used: %ld\n", max_memory_allocated);
}