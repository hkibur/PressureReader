#ifndef RAVGRINGBUFFER_H
#define RAVGRINGBUFFER_H

#include "RingBuffer.h"

class RAvgRingBuffer: public RingBuffer {
protected:
    float _curAvg;

    float _compAvg();
public:
    RAvgRingBuffer(int len);
    float getAvg();
};

#endif