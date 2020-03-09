#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

#include <Arduino.h>

/************************ Cyclic Buffer **************************/
template <typename T>
class CyclicBuffer
{
private:
    byte begin, end, pointer, size;
    byte SIZE;
    T *p_array;

public:
    CyclicBuffer(byte bufferSize)
    {
        begin, end, pointer, size = 0;
        SIZE = bufferSize;
        p_array = new T[SIZE];
    }

    ~CyclicBuffer() { delete[] p_array; }

    void push(T data)
    {
        if (isFull())
        {
            begin = (begin + 1) % SIZE;
            --size;
            pointer = begin;
        }

        p_array[end] = data;
        end = (end + 1) % SIZE;
        ++size;
    }

    void pull(T &target)
    {
        if (isEmpty())
            return;

        target = p_array[begin];
        begin = (begin + 1) % SIZE;
        pointer = begin;
        --size;
    }

    CyclicBuffer &next()
    {
        if (!isEmpty())
        {
            pointer = (pointer + 1) % SIZE;
            if (pointer == end)
                pointer = begin;
        }

        return *this;
    }

    CyclicBuffer &prev()
    {
        if (!isEmpty())
        {
            if (pointer == begin)
                pointer = (end == 0) ? SIZE - 1 : end - 1;
            else
                pointer = (pointer == 0) ? SIZE - 1 : pointer - 1;
        }

        return *this;
    }

    void get(T &target)
    {
        if (isEmpty())
            return;

        target = p_array[pointer];
    }

    CyclicBuffer &head()
    {
        if (!isEmpty())
            pointer = begin;

        return *this;
    }

    CyclicBuffer &tail()
    {
        if (!isEmpty())
            pointer = (end == 0) ? SIZE - 1 : end - 1;

        return *this;
    }

    byte getPointer() { return pointer; }
    byte getHeadPosition() { return begin; }
    byte getTailPosition() { return (end == 0) ? SIZE - 1 : end - 1; }
    void clear() { begin = end = pointer = this->size = 0; }
    byte getSize() { return size; }
    bool isEmpty() { return end == begin; }
    bool isFull() { return (end + 1) % SIZE == begin; }
};
/********************* End of Cyclic Buffer ***********************/

#endif