#include "Exception.h"
#include "TROOT.h"

namespace CbcDaq{
	const char *Exception::what() const throw(){ return Form( "BeController %s", fStrError.c_str() ); }
}

