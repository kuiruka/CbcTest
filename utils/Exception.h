#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__
#include <exception>
#include <string>

namespace CbcDaq{

	class Exception : public std::exception {
		public:
			Exception( const char *pStrError ){ fStrError = pStrError; }
			~Exception()throw(){}
			const char *what() const throw();
		private:
			std::string fStrError;
	};

}
#endif

