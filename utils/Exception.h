#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__
#include <exception>
#include <string>

namespace CbcDaq{

	class Exception : public std::exception {
		public:
			Exception( const char *pStrError, int pIntError = -1 ){ fStrError = pStrError; fIntError = pIntError; }
			~Exception()throw(){}
			const char *what() const throw();
			int			type() const throw();
		private:
			std::string fStrError;
			int 		fIntError;
	};

}
#endif

