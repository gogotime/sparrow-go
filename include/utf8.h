//
// Created by Galamo on 2022/5/4.
//

#ifndef SPARROW_GO_UTF8_H
#define SPARROW_GO_UTF8_H

#include "common.h"

uint32 getByteNumOfEncodeUtf8(int value);

uint8 encodeUtf8(uint8* buf, int value);

uint32 getByteNumOfDecodeUtf8(uint8 byte);

int decodeUtf8(const char* bytePtr, uint32 length);

#endif //SPARROW_GO_UTF8_H
