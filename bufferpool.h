#include "headers.h"
#include <vector>
using namespace std;

class BufferPool
{

  private:
    bufferHeader *freeListHead;
    bufferHeader *freeListTail;
    bufferHeader *hashQueueHead[HASHQUEUECOUNT];
    bufferHeader *hashQueueTail[HASHQUEUECOUNT];

    std::vector<struct blockRequest> waitingForAnyBuffer;
    std::vector<struct blockRequest> waitingForABuffer;
    std::vector<struct allocationLog> allocationLogs;

    //int disk[DISKSIZE];

    int scenarioFlag;

  public:
    BufferPool(); //Constructor
    //---------------------HASH QUEUE FUNCTIONS---------------------
    void initializeHashQueueHeadToNull();
    void addToHashQueue(bufferHeader *&b);
    void addToHQHead(bufferHeader *&H, bufferHeader *&T, bufferHeader *&B);
    void printHQ(bufferHeader *, bufferHeader *);
    void addToHQTail(bufferHeader *&H, bufferHeader *&T, bufferHeader *&B);
    void removeFromHashQueue(bufferHeader *&);
    void removeFromHQ(bufferHeader *&H, bufferHeader *&T, bufferHeader *&B);
    void printHashQueues(); //wrapper for printHQ()
    bufferHeader *findBufferInHashQueue(int, int);

    //---------------------FREE LIST FUNCTIONS---------------------
    void addToFreeListTail(bufferHeader *&b);
    void printFreeList();
    bufferHeader *removeFromFreeList(bufferHeader *&);
    bool isFreeListEmpty();
    bufferHeader *getBufferFromFreeListHead();
    void addToFreeListHead(bufferHeader *&);

    //------------------------------------------
    unsigned int hashFunction(bufferHeader *);
    unsigned int hashFunction(int);
    void printBuffer(bufferHeader *);
    void printBufferPoolStatus();

    //---------------------DEBUGGING FUNCTIONS---------------------
    void createFreeList();
    void populateHashQueues();
    void printFreeListReverse();
    void printHQReverse(bufferHeader *, bufferHeader *);
    void printHashQueuesReverse();
    //TODO : Move this to appropriate category.

    //---------------------BOOK KEEPING--------------------------
    void printAllocationLogs();

    //---------------------ALGORITHMS---------------------
    bufferHeader *getblk(int, int);
    void getBlock(blockRequest); //wrapper for getblk()
    void brelse(bufferHeader *&);
    void releaseBlock(releaseRequest); //wrapper for brelse()

    void wakeWaitingForAnyBuffer();
    void wakeWaitingForThisBuffer(int, int);

    void printWaitingForAnyBuffer();
    void printWaitingForABuffer();

    //---------------------USER INTERACTION---------------------
    void getResponseLocally();
    blockRequest getBlockRequest();
    releaseRequest getReleaseBlockRequest();
    void getResponseFromSharedMemory();

    //---------------------DISK RELATED---------------------
    //void randomlyFillDisk();	//fills random values in disk, only used for simulation.
};