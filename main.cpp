#include <iostream>
#include "bufferpool.h"
#include "headers.h"
#include "sharedmemory.h"

using namespace std;

int main()
{	
	BufferPool b;
	
	while(1){
	
	b.getResponseFromSharedMemory();
	
	}	
	
}
