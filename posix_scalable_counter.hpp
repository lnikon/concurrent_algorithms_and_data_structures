#pragma once

#include "wrapped_pthread.hpp"

#define NUMCPUS 4

typedef struct __counter_t {
    int             global;
    pthread_mutex_t glock;
    int             local[NUMCPUS];
    pthread_mutex_t llock[NUMCPUS];
    int             threashold;
} counter_t;

void init(counter_t* c, int threashold) {
    c->threashold = threashold;
    c->global = 0;
    Pthread_mutex_init(&c->glock);
    int i;
    for (i = 0; i < NUMCPUS; i++) {
        c->local[i] = 0;
        Pthread_mutex_init(&c->llock[i]);
    }
}

void update(counter_t* c, int threadID, int amt) {
    int cpu = threadID % NUMCPUS;
    Pthread_mutex_lock(&c->llock[cpu]);
    c->local[cpu] += amt;
    if (c->local[cpu] >= c->threashold) {
        Pthread_mutex_lock(&c->glock);
        c->global += c->local[cpu];
        Pthread_mutex_unlock(&c->glock);
        c->local[cpu] = 0;
    }
    Pthread_mutex_unlock(&c->llock[cpu]);
}

int get(counter_t* c) {
    Pthread_mutex_lock(&c->glock);
    int val = c->global;
    Pthread_mutex_unlock(&c->glock);
    return val;
}
