#include "Data.h"
#include <string.h>
#include <uhal/uhal.hpp>

namespace Strasbourg {

	void swapByteOrder( const char *org, char *swapped, unsigned int nbyte ){
	
		for( unsigned int i=0; i<nbyte; i++ ){
			swapped[i] = org[nbyte-1-i];
		}
	}

	Data::Data( Data &pD ){
		fBuf = 0;
		Initialise( pD.fNevents);
		fEvent = pD.fEvent;
		fBufSize = pD.fBufSize;
		fNevents = pD.fNevents;
		fCurrentEvent = pD.fCurrentEvent;
	}
	const Event *Data::GetNextEvent(){
		if( fCurrentEvent >= fNevents ) return 0; 
		fEvent.SetEvent( &fBuf[ fCurrentEvent * fEvent.Size32() * 4 ] );
		fCurrentEvent++;
		return &fEvent;
	}
	std::string Data::HexString()const{ //printout hex serial data

		std::stringbuf tmp;
		std::ostream os(&tmp);

		os <<std::hex;
		for( UInt_t j=0; j<fNevents; j++){
			for( UInt_t i=0; i< fEvent.Size32() * 4; i++ ) 
				os <<std::uppercase<<std::setw(2)<<std::setfill('0') << (fBuf[ j * fEvent.Size32() * 4 + i ]&0xFF );
			os << std::endl;
		}
		return tmp.str();
	}
	void Data::Initialise( UInt_t pNevents ){ 

		fNevents = pNevents;
		fBufSize = ( fNevents + 1 ) * fEvent.Size32() * 4;  
		if( fBuf ) free( fBuf );
		fBuf = (char *)malloc( fBufSize );

		fEvent.Clear(); 
#ifdef __CBCDAQ_DEV__
		std::cout << "Data::Initialise done." << std::endl;
#endif
	}
	void Data::Reset(){ fCurrentEvent = 0; for( UInt_t i=0; i<fBufSize; i++ ) fBuf[i]=0; }

	void Data::Set( void *pData ){

		Reset();

		uhal::ValVector<uint32_t> *cUhalData = (uhal::ValVector<uint32_t>*)pData; 

		for( unsigned int i=0; i<cUhalData->size(); i++ ){

			char cSwapped[4];
			uint32_t cVal = cUhalData->at(i);

			swapByteOrder( (const char *) &cVal, cSwapped, 4 );

			for( int j=0; j < 4; j++ ){
				fBuf[i*4+j] = cSwapped[j];
			}
		}
	}

	void Data::CopyBuffer( Data &pD ){
		memcpy( fBuf, pD.fBuf, pD.fBufSize );
	}
}


