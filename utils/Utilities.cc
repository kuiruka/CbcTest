#include "Utilities.h"
#include <iostream>

namespace CbcDaq{

	long getTimeTook( struct timeval &pStart, bool pMili ){ 

		struct timeval end;
		long seconds(0), useconds(0);

		gettimeofday( &end, 0 );
		seconds = end.tv_sec - pStart.tv_sec;
		useconds = end.tv_usec - pStart.tv_usec;
		if( pMili ){
			return (long) ( seconds * 1e3 + useconds/1000 );
		}
		else{
			return (long) (seconds * 1e6 + useconds );
		}
	}
}

