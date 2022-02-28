//struct queue *queue_new(size_t elem_size);
//Создать новую очередь, которая способна хранить элементы размера elem_size, вернуть указатель на неё.
//int queue_push(struct queue *q, const void *elem);
//Добавить в голову очереди q элемент elem. Обратите внимание, что из elem должно быть извлечено для хранения ровно то количество байт, которой было специфицировано при создании данного стека. Функция должна вернуть 0 при успехе и 1 при неуспехе любого рода. Амортизированная сложность этой операции должна быть O(1). После успешного исполнения данной операции в очереди должно оказаться на один элемент больше.
//int queue_pop (struct queue *q, void *elem);
//Извлечь из хвоста очереди q элемент elem. Обратите внимание, что в elem должно быть положено ровно то количество байт, которой было специфицировано при создании данного стека. Функция должна вернуть 0 при успехе и 1 при неуспехе любого рода. Амортизированная сложность этой операции должна быть O(1).
//int queue_empty(struct queue const *q);
//Функция-предикат. Она должна вернуть логическую истину (1), если очередь пуста и логический ноль (0) в противном случае.
//struct queue *queue_delete(struct queue *q);
//Удалить объект q и вернуть NULL.
//void queue_print(struct queue const *q, void (*pf)(void const *data));
//Так как очередь может содержать произвольные данные, но выводить информацию из неё необходимо, вторым аргументом функции является указатель на функцию, которая должна принимать указатель на данные, располагаемые в очереди и выводить их в соответствующем виде на стандартный вывод.

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct queue
{
	void *data;
	size_t start;
	size_t end;
	size_t elem_size;
	size_t size;
	size_t capacity;
};

struct queue *queue_new(size_t elem_size)
{
	assert(elem_size);
	struct queue *new = calloc(1, sizeof(struct queue));
	new->data = calloc(8, elem_size);
	new->elem_size = elem_size;
	new->start = new->end = new->size = 0;
	new->capacity = 8;
	return new;
}

int queue_push(struct queue *q, const void *elem)
{
	assert(q);
	assert(elem);
	if (q->size + 1 == q->capacity)
	{
		size_t count_real_num = q->capacity - q->start;
		q->capacity *= 2;
		struct queue *new = realloc(q->data, q->capacity * q->elem_size);
		if (new != NULL)
			q->data = new;
		else
			return 1;
		if (q->start > q->end)
		{
			memcpy(q->data + (q->capacity - count_real_num) * q->elem_size, q->data + q->start * q->elem_size,
			       count_real_num * q->elem_size);
			q->start = q->capacity - count_real_num;
		}
	}
	memcpy(q->data + q->end * q->elem_size, elem, q->elem_size);
	q->end = (q->end + 1) % q->capacity;
	q->size++;
	return 0;
}

int queue_pop(struct queue *q, void *elem)
{
	assert(q);
	assert(elem);
	if (q->size == 0)
		return 1;
	else
	{
		memcpy(elem, q->data + q->start * q->elem_size, q->elem_size);
		q->start = (q->start + 1) % q->capacity;
		q->size--;
		return 0;
	}
}

int queue_empty(struct queue const *q)
{
	assert(q);
	return (q->size == 0);
}

struct queue *queue_delete(struct queue *q)
{
	assert(q);
	free(q->data);
	free(q);
	return NULL;
}

void queue_print(struct queue const *q, void (*pf)(void const *data))
{
	assert(q);
	assert(pf);
	printf("[");
	if (q->size > 0)
		pf(q->data + (q->end - 1) * q->elem_size);
	for (int i = 1; i < q->size; i++)
	{
		printf(", ");
		pf(q->data + q->elem_size * ((q->end - 1 - i) % q->capacity));
		//pf(q->data + ((q->start + i) % q->capacity) * q->elem_size);
	}
	printf("]\n");
}