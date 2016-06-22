#ifndef WORK_H
#define WORK_H
#include <stddef.h>

void work_count(work_t * work);
void work_all(work_t * work, const char ** vector, size_t size);
void work_active(work_t * work, const char ** vector, size_t size);
void work_current(work_t * work, const char ** vector, size_t size);

size_t work_format(const char * work, char * buf, size_t size);
#endif
