#include "stm32f407xx.h"

#ifndef array_queue_FILE
#define array_queue_FILE

typedef struct Queue Queue;
Queue* qConstructor(void);
void qDestructor(Queue* q);
void qAdd(Queue* q, uint32_t value);
void qRemove(Queue* q, uint32_t* value);
void qDisplay(Queue* q);

#endif
