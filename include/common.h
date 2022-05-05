//
// Created by Galamo on 2022/5/4.
//

#ifndef SPARROW_GO_COMMON_H
#define SPARROW_GO_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define uint32 uint32_t
#define uint8 uint8_t
#define nil NULL
#define NULL ((void *)0)


typedef struct _parser Parser;
typedef struct _class Class;

#define bool char
#define true 1
#define false 0

#define UNUSED __attribute__ ((unused))

#ifdef DEBUG
#define ASSERT(condition,errMsg)                                                          \
do {                                                                                      \
    if (!condition){                                                                      \
        fprintf(stderr,"ASSERT FAILED: %s:%d:%s %s\n",__FILE__,__LINE__,__func__,errMsg); \
        abort();                                                                          \
    }                                                                                     \
}while(0);                                                                                \

#else
#define ASSERT(condition, errMsg) ((void)0);
#endif

#define NOT_REACHED() \
do {                                                                                      \
    if (!condition){                                                                      \
        fprintf(stderr,"NOT_REACHED: %s:%d:%s %s\n",__FILE__,__LINE__,__func__,errMsg); \
        while(1);                                                                      \
    }                                                                                     \
}while(0);\

#endif //SPARROW_GO_COMMON_H
