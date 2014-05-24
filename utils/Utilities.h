#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <sys/time.h>

namespace CbcDaq{

long getTimeTook( struct timeval &pStart, bool pMili ); 
}
#endif

