#include "RingBuffer.h"

void RingBuffer::_incPos(int var) {
    var += 1;
    if (var >= _bufLen) {
        var = 0;
    }
    return var;
};

void RingBuffer::RingBuffer(int len) {
    _buf = new int[len];
    _headPos = 0;
    _tailPos = 0;
    _bufLen = len
};

void RingBuffer::bumpTail(int val) {};