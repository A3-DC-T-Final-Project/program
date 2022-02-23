#include <stdio.h>
#include <stdlib.h>

#include "array-queue.h"
#include "stm32f407xx.h"

#define ARRAY_QUEUE_CAPACITY 10

struct Queue {
    int capacity;
    int numEntities;
    uint32_t* entities;
    int head;
    int tail;
};

// Creates Array Queue
Queue* qConstructor(void) {
    Queue* arrayQueue;

    arrayQueue = malloc(sizeof(Queue));

    arrayQueue->entities = malloc(sizeof(uint32_t) * ARRAY_QUEUE_CAPACITY);
    arrayQueue->capacity = ARRAY_QUEUE_CAPACITY;
    arrayQueue->numEntities = 0;
    arrayQueue->head = -1;
    arrayQueue->tail = 0;

    return arrayQueue;
}

void qDestructor(Queue* q) {
    free(q->entities);
    free(q);
}

void qAdd(Queue* q, uint32_t value) {
    if(q->head == -1)
        q->head = 0;

    if(q->head < (ARRAY_QUEUE_CAPACITY-1)) {
        if((q->tail + 1) != q->head) {
            if(q->tail == (ARRAY_QUEUE_CAPACITY-1)) {
								// Queue is full
                if(q->head == 0) {
                } else {
                    // Adds value at index
                    (q->entities)[q->tail] = value;
                    q->tail = 0;
                }
            } else {
                (q->entities)[q->tail] = value;
                q->tail++;
            }
        } else {
            // Queue is full
        }
    }
}

void qRemove(Queue* q, uint32_t* value) {
    int index = q->head;
    *value = (q->entities)[index];

    if(q->head != (ARRAY_QUEUE_CAPACITY - 1))
        q->head++;
    else {
        q->head = 0;
        if(q->tail == 0)
            q->tail++;
    }
}

void qDisplay(Queue* q) {
    int index = q->head;
    while(index != q->tail) {
        printf("qDisplay: Item %d at index %d\n", (q->entities)[index], index);
        if(q->tail < q->head) {
            if(index == (ARRAY_QUEUE_CAPACITY - 1)) {
                index = 0;
            } else {
                index++;
            }
        } else {
            index++;
        }
    }
}













