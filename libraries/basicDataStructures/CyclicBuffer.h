#ifndef CYCLIC_BUFFER_H
#define CYCLIC_BUFFER_H

#include <Arduino.h>

/************************ Cyclic Buffer **************************/
template <typename T>
class CyclicBuffer
{
private:
    unsigned int begin, end, pointer, size, SIZE;
    T *p_array;

public:
    CyclicBuffer(int bufferSize)
    {
        begin, end, pointer, size = 0;
        SIZE = bufferSize + 1;
        p_array = new T[SIZE];
    }

    ~CyclicBuffer() { delete[] p_array; }

    CyclicBuffer &push(T data)
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

        return *this;
    }

    T pull()
    {
        if (isEmpty())
            return;

        begin = (begin + 1) % SIZE;
        pointer = begin;
        --size;

        return p_array[begin];
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

    T get()
    {
        return p_array[pointer];
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

    unsigned int getPointer() { return pointer; }
    /*sets element pointer on given index
    if pointer excedes buffer size it will be set on head position
    
    !!! Be aware that you can point on garbage memory out of buffer scope !!!*/
    void setPointer(int newPointerPos)
    {
        if (newPointerPos >= SIZE || newPointerPos == end)
            head();
        else
            pointer = newPointerPos;
    }
    unsigned int getHeadPosition() { return begin; }
    unsigned int getTailPosition() { return (end == 0) ? SIZE - 1 : end - 1; }
    void clear() { begin = end = pointer = this->size = 0; }
    unsigned int getSize() { return size; }
    bool isEmpty() { return end == begin; }
    bool isFull() { return (end + 1) % SIZE == begin; }
};
/********************* End of Cyclic Buffer ***********************/

#endif