#include <vector>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include "bufferpool.h"
#include "headers.h"
#include "sharedmemory.h"
#include <signal.h>

using namespace std;

BufferPool::BufferPool()
{
    scenarioFlag = 0;
    //cout << "-System started" << endl;
    freeListHead = NULL;
    freeListTail = NULL;
    //memset(&disk, 0x00, sizeof(disk));	//initializing memory to zero
    initializeHashQueueHeadToNull();
    //cout << "-Pointers set to NULL" << endl;
    //cout << "-Creating Free List" << endl;
    createFreeList();
    //printFreeList();
    //printFreeListReverse();

    //cout << "-Creating Hash Queues" << endl;

    //populateHashQueues();
    //printHashQueues();
    //printHashQueuesReverse();
    printBufferPoolStatus();
}

//_______________________________HASH QUEUE FUNCTIONS__________________________________//

void BufferPool::initializeHashQueueHeadToNull()
{

    for (int i = 0; i < HASHQUEUECOUNT; i++)
    {

        hashQueueHead[i] = NULL;
        hashQueueTail[i] = NULL;
    }
}

void BufferPool::populateHashQueues()
{

    if (freeListHead == NULL)
    {

        cout << "Freelist empty | nothing to populate \n";
    }
    else
    {

        bufferHeader *temp = freeListHead;

        do
        {

            addToHashQueue(temp);
            temp = temp->nextOnFreeList;

        } while (temp != freeListHead);
    }
}

void BufferPool::addToHashQueue(bufferHeader *&buffer)
{

    int hashQueueNumber = hashFunction(buffer);

    addToHQHead(hashQueueHead[hashQueueNumber], hashQueueTail[hashQueueNumber], buffer);
}

void BufferPool::addToHQHead(bufferHeader *&HQHead, bufferHeader *&HQTail, bufferHeader *&buffer)
{

    if (HQHead == NULL)
    {

        HQHead = buffer;
        HQTail = buffer;
        buffer->nextOnHashQueue = buffer;
        buffer->prevOnHashQueue = buffer;
    }
    else
    {

        buffer->nextOnHashQueue = HQHead;
        buffer->prevOnHashQueue = HQTail;

        HQHead->prevOnHashQueue = buffer;
        HQTail->nextOnHashQueue = buffer;

        HQHead = buffer;
    }
}

void BufferPool::printHQ(bufferHeader *HQHead, bufferHeader *HQTail)
{

    if (HQHead == NULL)
    {

        cout << "Empty";
    }
    else
    {
        bufferHeader *iterator = HQHead;

        do
        {

            //cout << iterator->blockNumber << " ";
            printBuffer(iterator);
            iterator = iterator->nextOnHashQueue;

        } while (iterator != HQHead);
    }
}

void BufferPool::addToHQTail(bufferHeader *&HQHead, bufferHeader *&HQTail, bufferHeader *&buffer)
{

    if (HQHead == NULL)
    {

        HQHead = buffer;
        HQTail = buffer;

        buffer->nextOnHashQueue = buffer;
        buffer->prevOnHashQueue = buffer;
    }

    else
    {

        buffer->prevOnHashQueue = HQTail;
        HQTail->nextOnHashQueue = buffer;

        buffer->nextOnHashQueue = HQHead;
        HQHead->prevOnHashQueue = buffer;

        HQTail = buffer;
    }
}

void BufferPool::removeFromHashQueue(bufferHeader *&buffer)
{

    int hashQueueNumber = hashFunction(buffer);

    removeFromHQ(hashQueueHead[hashQueueNumber], hashQueueTail[hashQueueNumber], buffer);
}

void BufferPool::removeFromHQ(bufferHeader *&HQHead, bufferHeader *&HQTail, bufferHeader *&buffer)
{

    if (HQHead == NULL)
    {

        return;
    }
    else if (buffer->nextOnHashQueue == buffer) //Only one node in the list
    {
        HQHead = NULL;
        HQTail = NULL;
        buffer->nextOnHashQueue = NULL;
        buffer->prevOnHashQueue = NULL;
        return;
    }
    else if (buffer == HQHead)
    {

        HQHead = HQHead->nextOnHashQueue;
    }
    else if (buffer == HQTail)
    {

        HQTail = HQTail->nextOnHashQueue;
    }

    bufferHeader *previous = buffer->prevOnHashQueue;
    bufferHeader *next = buffer->nextOnHashQueue;

    previous->nextOnHashQueue = next;
    next->prevOnHashQueue = previous;

    buffer->nextOnHashQueue = NULL;
    buffer->prevOnHashQueue = NULL;
}

void BufferPool::printHashQueues()
{

    cout << "_________________________________Hash Queues_______________________________" << endl;
    cout << "-------[BlockNumber,DeviceNumber,Data,BUSY,VALID,DELAYWRITE,DEMAND]--------" << endl;

    for (int i = 0; i < HASHQUEUECOUNT; i++)
    {

        cout << "HQ " << i << " : ";
        printHQ(hashQueueHead[i], hashQueueTail[i]);
        cout << endl;
    }

    cout << "____________________________________________________________________________" << endl;
}

bufferHeader *BufferPool::findBufferInHashQueue(int deviceNo, int blockNo)
{

    int hashQueueNumber = hashFunction(blockNo);

    bufferHeader *HQHead = hashQueueHead[hashQueueNumber];
    bufferHeader *iterator = HQHead;

    if (HQHead == NULL)
    {

        return NULL;
    }
    else
    {
        do
        {
            if (iterator->blockNumber == blockNo && iterator->deviceNumber == deviceNo)
            {

                return iterator; //Found, return buffer
                break;           //TODO : Do I need this?
            }
            iterator = iterator->nextOnHashQueue;

        } while (iterator != HQHead);

        return NULL; //Not Found, return NULL;
    }
}

//_______________________________FREE LIST FUNCTIONS__________________________________//

void BufferPool::addToFreeListTail(bufferHeader *&buffer)
{

    buffer->busy = false;

    if (freeListHead == NULL)
    {

        freeListHead = buffer;
        freeListTail = buffer;

        buffer->nextOnFreeList = buffer;
        buffer->prevOnFreeList = buffer;
    }

    else
    {

        buffer->prevOnFreeList = freeListTail;
        freeListTail->nextOnFreeList = buffer;

        buffer->nextOnFreeList = freeListHead;
        freeListHead->prevOnFreeList = buffer;

        freeListTail = buffer;
    }
}

void BufferPool::printFreeList()
{

    cout << "_________________________________Freelist__________________________________" << endl;
    cout << "-------[BlockNumber,DeviceNumber,Data,BUSY,VALID,DELAYWRITE,DEMAND]--------" << endl;

    if (freeListHead == NULL)
    {

        cout << "Empty" << endl;
    }
    else
    {
        bufferHeader *iterator = freeListHead;
        int nextLine = 0;

        do
        {

            nextLine++;
            printBuffer(iterator);

            if (nextLine % 5 == 0)
                cout << endl;

            iterator = iterator->nextOnFreeList;

        } while (iterator != freeListHead);
    }

    cout << "\n____________________________________________________________________________" << endl;
}

bufferHeader *BufferPool::removeFromFreeList(bufferHeader *&buffer)
{

    //just to double check
    buffer->busy = true;

    if (freeListHead == NULL)
    {
        return NULL;
    }
    else if (buffer->nextOnFreeList == buffer) //Only one node in the list
    {
        //TODO : I hope this doesn't bug out...
        buffer->nextOnFreeList = NULL;
        buffer->prevOnFreeList = NULL;
        freeListHead = NULL;
        freeListTail = NULL;
        return buffer;
    }
    else if (buffer == freeListHead)
    {

        freeListHead = freeListHead->nextOnFreeList;
    }
    else if (buffer == freeListTail)
    {

        freeListTail = freeListTail->nextOnFreeList;
    }

    bufferHeader *previous = buffer->prevOnFreeList;
    bufferHeader *next = buffer->nextOnFreeList;

    previous->nextOnFreeList = next;
    next->prevOnFreeList = previous;

    buffer->nextOnFreeList = NULL;
    buffer->prevOnFreeList = NULL;

    return buffer;
}

bool BufferPool::isFreeListEmpty()
{

    return (freeListHead == NULL ? true : false);
}

bufferHeader *BufferPool::getBufferFromFreeListHead()
{

    bufferHeader *temp = freeListHead;

    if (temp == NULL)
    {
        return NULL;
    }
    else
    {

        return (removeFromFreeList(temp));
    }
}

void BufferPool::addToFreeListHead(bufferHeader *&buffer)
{

    buffer->busy = false;

    if (freeListHead == NULL)
    {

        freeListHead = buffer;
        freeListTail = buffer;
        buffer->nextOnFreeList = buffer;
        buffer->prevOnFreeList = buffer;
    }
    else
    {
        buffer->nextOnFreeList = freeListHead;
        freeListHead->prevOnFreeList = buffer;

        freeListTail->nextOnFreeList = buffer;
        buffer->prevOnFreeList = freeListTail;

        freeListHead = buffer;
    }
}

//_______________________________HASHING FUNCTIONS__________________________________//

unsigned int BufferPool::hashFunction(bufferHeader *buffer)
{

    return (buffer->blockNumber % HASHQUEUECOUNT);
}

unsigned int BufferPool::hashFunction(int blockNo)
{

    return (blockNo % HASHQUEUECOUNT);
}

void BufferPool::printBufferPoolStatus()
{

    printFreeList();
    cout << endl;
    printHashQueues();
    cout << endl;
    printAllocationLogs();
}

void BufferPool::printWaitingForAnyBuffer()
{

    cout << "Waiting for Any Buffer [ProcessID] : ";

    if (waitingForAnyBuffer.empty())
    {
        cout << "None" << endl;
    }
    else
    {

        for (unsigned int i = 0; i < waitingForAnyBuffer.size(); i++)
        {

            blockRequest br = waitingForAnyBuffer[i];
            cout << "[" << br.processID << "]";
        }

        cout << endl;
    }
}

void BufferPool::printWaitingForABuffer()
{

    cout << "Waiting for A Buffer [Process,DevNo,BlkNo] : ";
    if (waitingForABuffer.empty())
    {
        cout << "None" << endl;
    }
    else
    {

        for (unsigned int i = 0; i < waitingForABuffer.size(); i++)
        {

            blockRequest br = waitingForABuffer[i];
            cout << "[" << br.processID << ","
                 << br.deviceNumber << ","
                 << br.blockNumber << "]";
        }

        cout << endl;
    }
}

void BufferPool::printAllocationLogs()
{
    cout << "____________________________________Logs____________________________________" << endl;
    //cout<<"---------------------------[ProcessID,BlockNo]------------------------------"<<endl;
    cout << "Allocated [ProcessID,BlockNo] : ";
    if (!allocationLogs.empty())
    {
        for (unsigned int i = 0; i < allocationLogs.size(); i++)
        {

            cout << "[" << allocationLogs[i].processID
                 << "," << allocationLogs[i].allocatedBuffer->blockNumber
                 << "]";
        }

        cout << endl;
    }
    else
    {
        cout << "None" << endl;
    }
    printWaitingForAnyBuffer();
    printWaitingForABuffer();

    cout << "____________________________________________________________________________" << endl;
}

//_______________________________DEBUGGING FUNCTIONS__________________________________//

void BufferPool::createFreeList()
{

    for (int i = 0; i < MAXIMUMBUFFERCOUNT; i++)
    {

        bufferHeader *newBufferHeader = new bufferHeader;

        //initially buffer are not allocated any memory block / device no
        newBufferHeader->deviceNumber = 0; //TODO : Take care of device no, for now 1 by default.
        newBufferHeader->blockNumber = i;  //TODO : explicity setting block number here, undo this.

        //TODO : Don't initialize data, this is for debugging purpose only.
        newBufferHeader->data = 0;

        //TODO : check if status bits are correctly Set/Initialized
        //Setting bits
        newBufferHeader->busy = false;
        newBufferHeader->valid = true;
        newBufferHeader->delayedWrite = false;
        newBufferHeader->demand = false;

        //Setting pointers to NULL
        newBufferHeader->nextOnFreeList = NULL;
        newBufferHeader->prevOnFreeList = NULL;
        newBufferHeader->nextOnHashQueue = NULL;
        newBufferHeader->prevOnHashQueue = NULL;

        addToFreeListTail(newBufferHeader);
    }
}

void BufferPool::printFreeListReverse()
{

    cout << "-----------------------Freelist in reverse -----------------------" << endl;

    if (freeListHead == NULL)
    {

        cout << "Empty";
    }
    else
    {

        bufferHeader *iterator = freeListTail;

        do
        {

            cout << iterator->blockNumber << " ";
            iterator = iterator->prevOnFreeList;

        } while (iterator != freeListTail);
    }

    cout << endl;

    cout << "------------------------------------------------------" << endl;
}

void BufferPool::printHQReverse(bufferHeader *HQHead, bufferHeader *HQTail)
{

    if (HQHead == NULL)
    {

        cout << "Empty";
    }
    else
    {
        bufferHeader *iterator = HQTail;

        do
        {

            //cout << iterator->blockNumber <<" ";
            printBuffer(iterator);
            iterator = iterator->prevOnHashQueue;

        } while (iterator != HQTail);
    }
}

void BufferPool::printHashQueuesReverse()
{

    cout << "------------------------Hash Queues in reverse----------------------" << endl;

    for (int i = 0; i < HASHQUEUECOUNT; i++)
    {

        cout << "HQ " << i << " : ";
        printHQReverse(hashQueueHead[i], hashQueueTail[i]);
        cout << endl;
    }

    cout << "---------------------------------------------------------------------" << endl;
}

void BufferPool::printBuffer(bufferHeader *buffer)
{

    cout << "[" << buffer->blockNumber << ","
         << buffer->deviceNumber << ","
         << buffer->data << ","
         << (buffer->busy ? "B" : "F") << ","
         << (buffer->valid ? "V" : "INV") << ","
         << (buffer->delayedWrite ? "DW" : "N") << ","
         << (buffer->demand ? "Y" : "N")
         << "]";
}

//_______________________________ALGORITHMS__________________________________//

void BufferPool::getBlock(blockRequest r)
{

    scenarioFlag = 0; //just to be sure

    bufferHeader *buffer = getblk(r.deviceNumber, r.blockNumber);

    if (scenarioFlag == 1 || scenarioFlag == 2)
    {
        buffer->demand = false;
        buffer->data = r.data;
        buffer->valid = r.isValid;
        buffer->delayedWrite = r.isDelayedWrite;
        allocationLogs.push_back({r.processID, buffer});
    }
    else if (scenarioFlag == 5)
    { //buffer found but is busy

        waitingForABuffer.push_back(r);
        kill(r.processID, SIGUSR1); //TODO : Comment out for local run
    }
    else if (scenarioFlag == 4)
    { //buffer not found + freelist empty

        waitingForAnyBuffer.push_back(r);
        kill(r.processID, SIGUSR1); //TODO : Comment out for local run
    }
}

bufferHeader *BufferPool::getblk(int deviceNumber, int blockNumber)
{

    bufferHeader *buffer = NULL;

    while (1)
    {

        buffer = findBufferInHashQueue(deviceNumber, blockNumber);

        if (buffer != NULL)
        {

            if (buffer->busy) //Scenario 5 : Kernel finds the block on hash queue, but its buffer is currently busy.
            {
                scenarioFlag = 5;
                buffer->demand = true;
                return NULL;
            }
            //Scenario 1 : Kernel finds the block on hash queue, and its buffer is free.

            scenarioFlag = 1;
            buffer->busy = true;        //mark buffer busy
            removeFromFreeList(buffer); //remove buffer from freelist
            return buffer;
        }
        else
        { //block not found on hash queue

        freeListEmptyJump: //If delayed write case does not find a freebuffer.

            if (isFreeListEmpty()) //Scenario 4 : The kernel cannot find the block on hash queue and the freelist is empty.
            {
                //sleep(event any buffer becomes free)
                scenarioFlag = 4;
                return NULL;
            }

            bufferHeader *freeBuffer = getBufferFromFreeListHead(); //remove buffer from free list

        delayWriteJump:

            if (freeBuffer != NULL) //Scenario 3 : The kernel cannot find the block on hash queue and, in attempting to allocate a buffer from the freelist (as in scenario 2), finds a buffer on freelist that has been marked "delayed write". The kernel must write the buffer to disk and allocate another buffer.
            {
                if (freeBuffer->delayedWrite)
                {

                    scenarioFlag = 3;
                    cout << "-Scenario 3" << endl;
                    cout << "Asynchronously writing to disk.\n";
                    sleep(3);
                    bufferHeader *delayedWriteBuffer = freeBuffer;
                    freeBuffer->delayedWrite = false;
                    cout << "Write complete.\n";
                    sleep(1);
                    cout << "Trying next freebuffer.\n";
                    freeBuffer = getBufferFromFreeListHead(); //Get new buffer from freelist head
                    addToFreeListHead(delayedWriteBuffer);    //Release delayed write buffer to freelist head
                    goto delayWriteJump;
                }
            }

            if (freeBuffer == NULL)
                goto freeListEmptyJump;

            //Scenario 2 : The kernel cannot find the block on hash queue, so it allocates a buffer from freelist.
            scenarioFlag = 2;
            removeFromHashQueue(freeBuffer); //need to check
            freeBuffer->blockNumber = blockNumber;
            freeBuffer->deviceNumber = deviceNumber;
            addToHashQueue(freeBuffer);
            return freeBuffer;
        }
    }
}

void BufferPool::releaseBlock(releaseRequest r)
{

    cout << "Releasing all buffers held by process : " << r.processID << endl;

    for (unsigned int i = 0; i < allocationLogs.size(); i++)
    {

        if (allocationLogs[i].processID == r.processID)
        {
            brelse(allocationLogs[i].allocatedBuffer);
            allocationLogs.erase(allocationLogs.begin() + i);
            --i;
        }
    }

    cout << "-Released-" << endl;
}

void BufferPool::brelse(bufferHeader *&buffer)
{

    if (buffer->valid)
        addToFreeListTail(buffer);
    else
        addToFreeListHead(buffer);

    buffer->busy = false;

    wakeWaitingForThisBuffer(buffer->deviceNumber, buffer->blockNumber);
    wakeWaitingForAnyBuffer();
}

void BufferPool::wakeWaitingForAnyBuffer()
{

    if (!waitingForAnyBuffer.empty())
    {
        srand(time(0));
        int randomRequestIndex = rand() % waitingForAnyBuffer.size();
        blockRequest r = waitingForAnyBuffer[randomRequestIndex];
        waitingForAnyBuffer.erase(waitingForAnyBuffer.begin() + randomRequestIndex);
        getBlock(r);
        kill(r.processID, SIGUSR2); // TODO :  Comment out for local run
    }
}

void BufferPool::wakeWaitingForThisBuffer(int devNo, int blkNo)
{

    if (!waitingForABuffer.empty())
    {

        for (unsigned int i = 0; i < waitingForABuffer.size(); i++)
        {
            blockRequest r = waitingForABuffer[i];

            if (r.deviceNumber == devNo && r.blockNumber == blkNo)
            {

                waitingForABuffer.erase(waitingForABuffer.begin() + i);
                getBlock(r);
                i--;
                kill(r.processID, SIGUSR2); // TODO :  Comment out for local run
            }
        }
    }
}

//_______________________________USER INTERACTION__________________________________//

blockRequest BufferPool::getBlockRequest()
{

    int blockNumber, deviceNumber, data, processID;
    bool valid, delayWrite;
    cout << "----------Block Request----------" << endl;
    cout << "Enter processID : ";
    cin >> processID;
    cout << "Enter Device No : ";
    cin >> deviceNumber;
    cout << "Enter Block No : ";
    cin >> blockNumber;
    cout << "Data : ";
    cin >> data;
    cout << "Data valid? [0/1] : ";
    cin >> valid;
    cout << "Delay write? [0/1] : ";
    cin >> delayWrite;

    blockRequest r;

    r.processID = processID;
    r.deviceNumber = deviceNumber;
    r.blockNumber = blockNumber;
    r.data = data;
    r.isValid = valid;
    r.isDelayedWrite = delayWrite;

    return r;
}
releaseRequest BufferPool::getReleaseBlockRequest()
{

    int processID;
    cout << "Enter processID : ";
    cin >> processID;

    releaseRequest r; //TODO : Make use of other two fields.
    r.processID = processID;

    return r;
}

void BufferPool::getResponseLocally()
{

    int response = 0;

    while (response != 99)
    {
        //cout<<"Process ID : "<<processID<<endl;
        cout << "1. Get Block\n2. Release Block\n3. Print Buffer Pool\n99. Exit" << endl;
        cout << "Response : ";
        cin >> response;

        switch (response)
        {

        case 1:
        {
            blockRequest br = getBlockRequest();
            getBlock(br);
            break;
        }

        case 2:
        {
            releaseRequest rr = getReleaseBlockRequest();
            releaseBlock(rr);
            break;
        }
        case 3:
            printBufferPoolStatus();
            break;

        case 99:
            break; // need to do this otherwise it will print "Enter a valid response!"

        default:
            cout << "Enter a valid response!\n";
        }
    }
}

void BufferPool::getResponseFromSharedMemory()
{

    communicationPacket *cp = createSharedMemory();
    cout << "-Waiting for requests" << endl;
    while (1)
    {

        if (cp->hasData)
        {

            if (cp->packetType == 0) //getblk request
            {
                cout << "-Get Block Request recieved\nDetails : \n";
                int processID = cp->clientProcessID;
                cout << "ProcessID : " << processID << endl;
                int deviceNumber = cp->deviceNumber;
                cout << "Device Number : " << deviceNumber << endl;
                int blockNumber = cp->blockNumber;
                cout << "Block Number : " << blockNumber << endl;
                int data = cp->data;
                cout << "Data : " << data << endl;
                bool valid = cp->valid;
                cout << "Valid? : " << (valid ? "1" : "0") << endl;
                bool delayWrite = cp->delayWrite;
                cout << "Delay Write? : " << (delayWrite ? "1" : "0") << endl;

                blockRequest br;
                br.processID = processID;
                br.deviceNumber = deviceNumber;
                br.blockNumber = blockNumber;
                br.data = data;
                br.isValid = valid;
                br.isDelayedWrite = delayWrite;
                getBlock(br);
                resetMemory(cp);
                printBufferPoolStatus();
                cout << "-Get Block Request served" << endl;
            }
            else if (cp->packetType == 1) //brelse request
            {
                cout << "-Block release request recieved" << endl;
                int processID = cp->clientProcessID;
                cout << "Requesting process : " << processID << endl;
                releaseRequest rr;
                rr.processID = processID;
                releaseBlock(rr);
                resetMemory(cp);
                printBufferPoolStatus();
                cout << "-Block release request served" << endl;
            }
            else if (cp->packetType == 2) //print buffer status
            {
                printBufferPoolStatus();
                resetMemory(cp);
            }
            else if (cp->packetType == 99)
            { //exit request, free all buffers held by process
                int processID = cp->clientProcessID;
                releaseRequest rr;
                rr.processID = processID;
                releaseBlock(rr);
                resetMemory(cp);
                printBufferPoolStatus();
            }
        }
    }
}

/*
//_______________________________DISK RELATED__________________________________//
void BufferPool::randomlyFillDisk()
{	
	int randomValue = 0;
	
	for(int i = 0; i < DISKSIZE ; i++)
		{
				randomValue = DISKVALUERANGE % srand(time(0));	//generates a value between 0 and DISKVALUERANGE
				disk[i] = randomValue;
			
		}
	
}

*/