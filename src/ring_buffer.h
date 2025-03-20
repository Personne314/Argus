#pragma once


struct RingBuffer {
	float *buffer;
	int cap;
	int start; 
	int end;
};
typedef struct RingBuffer RingBuffer;
