#include "sharedmemory.h"
#include <iostream>
using namespace std;
void destroySharedMemory(){
	
	//Used by server to destroy shared memory
	//TODO : DO THIS! I have to do this
	
	};
void detachFromSharedMemory(communicationPacket* cp){
	//Used by clients to detach from shared memory.	
	shmdt(cp);
};
void resetMemory(communicationPacket* cp){
	
	//CONFIGURED TO BE USED BY SERVER i.e. setting packetType = 1 and serverProcessID;
	cp->hasData = false;
    int pid = getpid();
	cp->serverProcessID = pid;
	cp->hasData = false;
	cp->packetType = 1;
	
	cp->clientProcessID = 0;
	cp->deviceNumber = 0;
	cp->blockNumber = 0;
	cp->data = 0;
	cp->valid = false;
	cp->delayWrite = false;
	
}
void sendRequest(communicationPacket* cp, int devNo, int blkNo, int data, bool valid, bool delayWrite){	
	cp->hasData = true;
	int pid = getpid();
	cp->hasData = true;
	cp->packetType = 0;
	cp->clientProcessID = pid;
	cp->deviceNumber = devNo;
	cp->blockNumber = blkNo;
	cp->valid = valid;
	cp->delayWrite = delayWrite;

}
communicationPacket* createSharedMemory(){	
	
	//Called by server, creates memory.
	communicationPacket* cp;
	key_t key = ftok(SHAREDMEMORYNAME,65);
	int shmid = shmget(key,sizeof(communicationPacket),0666|IPC_CREAT); 
	cp = (communicationPacket*) shmat(shmid,(void*)0,0);
	std::cout<<"-Shared memory created"<<std::endl;
	//initialize memory with serverProcessID;
	resetMemory(cp);
	return cp;
	
}	

communicationPacket* connectToSharedMemory(){	
	//Used by client to connect to shared memory.
	communicationPacket* cp;
	key_t key = ftok(SHAREDMEMORYNAME,65);
	int shmid = shmget(key,sizeof(communicationPacket),0666);
	
	cp = (communicationPacket*) shmat(shmid,(void*)0,0);
	std::cout<<"-Shared memory created"<<std::endl;
	return cp;
	
}
//--------------NEW ADDITIONS--------------------

void sendRequestForBlock(communicationPacket* cp){
    cp->hasData = false;    //Just in case
    int blockNumber, deviceNumber, data, processID;
	bool valid, delayWrite, demand;

    cout<<"----------Block Request----------"<<endl;
	
    cp->packetType = 0;         // 0 : getblock request 
    
    processID = getpid();
    cp->clientProcessID = processID;
	
	cout<<"Enter Device No : ";
	cin>>deviceNumber;
    cp->deviceNumber = deviceNumber;
    
    cout<<"Enter Block No : ";
	cin>>blockNumber;
	cp->blockNumber = blockNumber;
    
    cout<<"Data : ";
	cin>>data;
	cp->data = data;
    
    cout<<"Data valid? [0/1] : ";
	cin>>valid;
	cp->valid = valid;

    cout<<"Delay write? [0/1] : ";
	cin>>delayWrite;
    cp->delayWrite = delayWrite;
    cp->hasData = true; //This should always be done at last
    cout<<"----------Request Served----------"<<endl;
}

void sendRequestForReleaseBlock(communicationPacket* cp){
    
    bool response = 0;
    cout<<"Release all blocks held by process? [0/1] : ";
    cin>>response;
    
    if(response){

        cp->hasData = false; //just to be sure
        int processID = getpid();
        cp->packetType = 1;     //Release block request
        cp->clientProcessID = processID;
        cp->hasData = true;
    }
    
    
}

void sendRequestToPrintBufferStatus(communicationPacket* cp){
    
    int processID = getpid();
    cp->hasData = false;
    cp->clientProcessID = processID;
    cp->packetType = 2; // printBufferStatus() request 
    cp->hasData = true;

}
