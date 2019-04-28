#ifndef HEADERS_H
#define HEADERS_H
#define HASHQUEUECOUNT 4
#define MAXIMUMBUFFERCOUNT 2
//#define DISKSIZE	100
//#define DISKVALUERANGE	100

struct bufferHeader
{

    int deviceNumber; //Logical device number
    int blockNumber;  //Block number on the device
    int data;         //Pointer to data area

    //status

    bool busy; // if the buffer is Busy/Locked or Free/Unlocked
    bool valid;
    bool delayedWrite;
    // TODO : bool reading writing contents of buffer to disk
    bool demand; //if any process is waiting for buffer to become free

    // pointers
    bufferHeader *nextOnFreeList;
    bufferHeader *prevOnFreeList;
    bufferHeader *nextOnHashQueue;
    bufferHeader *prevOnHashQueue;
};

struct allocationLog
{

    int processID;
    bufferHeader *allocatedBuffer;
};

struct waitingLog
{

    int processID;
    int blockNumber;
    int deviceNumber;
};

struct blockRequest
{

    int processID;
    int blockNumber;
    int deviceNumber;
    int isValid;
    int isDelayedWrite;
    int data;
};

struct releaseRequest
{

    int processID;
    int deviceNumber; //Not used as of now
    int blockNumber;  //Not used as of now
};

#endif