#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H
#include <sys/shm.h>
#include <unistd.h>
#include <iostream>
#define SHAREDMEMORYNAME "buffer_pool_shm"

// used by client to request a block 
struct communicationPacket {
	
	bool hasData = false;
	int serverProcessID;
	int packetType;	
	
	//0 : getBlock() request
	//1 : bufferRelease() request
	//2 : bufferPoolPrint() request
	
	
	//client fields
	int clientProcessID;
	int deviceNumber;
	int blockNumber;
	int data;
	bool valid;
	bool delayWrite;

	
	//server fields

};

void destroySharedMemory();
void detachFromSharedMemory(communicationPacket*);
void resetMemory(communicationPacket*);
void sendRequest(communicationPacket* cp, int devNo, int blkNo, int data, bool valid, bool delayWrite);
communicationPacket* createSharedMemory();
void sendResponse();
communicationPacket* connectToSharedMemory();

//------------------NEW ADDITIONS--------------------------

void sendRequestForBlock(communicationPacket*);
void sendRequestForReleaseBlock(communicationPacket*);
void sendRequestToPrintBufferStatus(communicationPacket*);

#endif
