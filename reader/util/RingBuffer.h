#ifndef RINGBUFFER_H
#define RINGBUFFER_H

class RingBuffer {
protected:
    int _headPos;
    int _tailPos;
    int _bufLen;
    int* _buf;

    void _incPos(int var);
public:
    RingBuffer(int len);
    void bumpTail(int val);
};

#endif