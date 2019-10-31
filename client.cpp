#include <iostream>
#include <vector>
#include <sys/shm.h>
#include <unistd.h>
#include "sharedmemory.h" // to include sharedmemeory.h
#include <signal.h>

using namespace std;

communicationPacket *cp = connectToSharedMemory();

void getResponse()
{

	int response = 0;
	while (response != 99)
	{
		response = 0;
		//cout<<"Process ID : "<<processID<<endl;
		cout << "1. Get Block\n2. Print Buffer Pool\n99. Exit" << endl;
		cout << "Response : ";
		cin >> response;

		if (response == 1)
		{

			response = 0;
			sendRequestForBlock(cp);
			cout << "1. Release Block\n2. Print Buffer Pool status\n99. Exit" << endl;
			cin >> response;

			if (response == 1)
			{

				sendRequestForReleaseBlock(cp);
			}
			else if (response == 2)
			{

				sendRequestToPrintBufferStatus(cp);
			}
			else if (response == 99)
			{

				//release buffers
				exit(0);
			}
		}
		else if (response == 2)
		{

			sendRequestToPrintBufferStatus(cp);
		}
		else if (response == 99)
		{

			sendRequestForReleaseBlock(cp);
			exit(0);
		}
		else
		{

			cout << "Enter a valid response!" << endl;
		}

		/*
		switch(response)
		{
		
			case 1 : sendRequestForBlock(cp);
					 break;
					 
			case 2 : sendRequestForReleaseBlock(cp);
					 break;
			case 3 : sendRequestToPrintBufferStatus(cp);
					 break;
					 
			case 99 : //detach from memory
					  //release all buffers held by process 
					  sendRequestForReleaseBlock(cp);
					  exit(0);
					 
 
			default : cout<<"Enter a valid response!\n";
			
		 }
		*/
	}
}

void signalHandlerClient(int signalNumber)
{

	if (signalNumber == SIGUSR1)
	{
		cout << "Block is busy | Free list is empty\n";
		sleep(2);
		cout << "Proccess is sleeping." << endl;
		sleep(1);
		pause();
	}
	else if (signalNumber == SIGUSR2)
	{
		cout << "Waking up..." << endl;
		sleep(2);
		cout << "Block request served" << endl;
		sleep(1);
		getResponse(); //TODO : Bad practice, but for now.
		return;
	}
}

int main()
{
	cout << "Process ID : " << getpid() << endl;
	signal(SIGUSR1, signalHandlerClient);
	signal(SIGUSR2, signalHandlerClient);

	//TODO : Add signal handlers

	while (1)
	{

		getResponse();
	}
	exit(0);
}
