//
// Created by gogotime on 2022/5/4.
//

#include "utils.h"
#include "utf8.h"

uint32 getByteNumOfEncodeUtf8(int value) {
    ASSERT(value > 0, "value must >0")
    if (value <= 0x7f) {
        return 1;
    }
    if (value <= 0x7ff) {
        return 2;
    }
    if (value <= 0xffff) {
        return 3;
    }
    if (value <= 0x10ffff) {
        return 4;
    }
    return 0;
}

uint8 encodeUtf8(uint8* buf, int value) {
    ASSERT(value > 0, "value must >0")
    if (value <= 0x7f) {
        *buf = value & 0x7f;
        return 1;
    }
    if (value <= 0x7ff) {
        *buf = 0b11000000 | (value >> 6);
        buf++;
        *buf = 0b10000000 | (value & 0x3f);
        return 2;
    }
    if (value <= 0xffff) {
        *buf = 0xe0 | (value >> 12);
        buf++;
        *buf = 0x80 | ((value >> 6) & 0x3f);
        buf++;
        *buf = 0x80 | (value & 0x3f);
        return 3;
    }
    if (value <= 0x10ffff) {
        *buf = 0xf0 | (value >> 18);
        buf++;
        *buf = 0x80 | ((value >> 12) & 0x3f);
        buf++;
        *buf = 0x80 | ((value >> 6) & 0x3f);
        buf++;
        *buf = 0x80 | (value & 0x3f);
        return 4;
    }
    NOT_REACHED();
    return 0;
}

uint32 getByteNumOfDecodeUtf8(uint8 byte) {
    if ((byte & 0xc0) == 0x80) {
        return 0;
    }
    if ((byte & 0xf8) == 0xf0) {
        return 4;
    }
    if ((byte & 0xf0) == 0xe0) {
        return 3;
    }
    if ((byte & 0xe0) == 0xc0) {
        return 2;
    }
    return 1;
}

int decodeUtf8(const char* bytePtr, uint32 length) {
    if (*bytePtr <= 0x7f) {
        return *bytePtr;
    }

    int value = 0;
    uint8 remainByte = 0;

    if ((*bytePtr & 0xf8) == 0xf0) {
        value = *bytePtr & 0x07;
        remainByte = 3;
    } else if ((*bytePtr & 0xf0) == 0xe0) {
        value = *bytePtr & 0x0f;
        remainByte = 2;
    } else if ((*bytePtr & 0xe0) == 0xc0) {
        value = *bytePtr & 0x1f;
        remainByte = 1;
    } else {
        return -1;
    }

    if (remainByte > length - 1) {
        return -1;
    }

    while (remainByte > 0) {
        bytePtr++;
        remainByte--;
        if ((*bytePtr & 0xc0) != 0x80) {
            return -1;
        }
        value = value << 6 | (*bytePtr & 0x3f);
    }

    return value;
}

