#ifndef __DATA_H__
#define __DATA_H__

#include <cstring>
#include "Event.h"
#include <string>
#include <sstream>
#include <iomanip>
#include <TROOT.h>

namespace Strasbourg {

	class DataIterator;
	class DataConstIterator;

	/*
	   data serialiser and decoder for strusburg BE board.
	   Once in the beginning,	
	   Call Initialise() to allocate buffer.
	   Call AddFe() to add FE board.
	   For each acquisition,
	   Call Set() to set new data. 
	   Call GetNextEvent() to set next event to fEvent object. This function will return 0 if there is no more event. 
	 */
	class Data {

		public:

			Data(): fBuf(0){;}
			Data( Data &pD );
			~Data(){ if( fBuf) free(fBuf); }

			const Event *GetNextEvent();
			std::string HexString()const; //printout hex serial data

			void Initialise( UInt_t pNevents ); //pNevents : # of events in one acquisition. Memory is allocated to fBuf for pNevents. 
			void AddFe( UInt_t pFe ){ fEvent.AddFe( pFe ); } //Add one FE with id pFe with two CBC.
			void Set( void *Data ); 

			void Reset();  //reset the components of the char array fBuf to be 0.

			const Event *GetEvent()const{ return &fEvent; }
			void AddCbc( UInt_t pFe, UInt_t pCbc ){ fEvent.AddCbc( pFe, pCbc ); }
			void CopyBuffer( Data &pD );
			const char * GetBuffer( UInt_t &pBufSize )const{ pBufSize = fBufSize; return fBuf; }
		private:
			char *fBuf;
			Event fEvent;
			UInt_t fBufSize;
			UInt_t fNevents;
			UInt_t fCurrentEvent;

	};

}
#endif
