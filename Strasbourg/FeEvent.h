#ifndef __FEEVENT_H__
#define __FEEVENT_H__
#include "CbcEvent.h"
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>
#include <TROOT.h>

namespace Strasbourg {

	class FeEvent {

		/* 
		single FE event
		The Id should corresponds to the order of in the stream data starting from 0
		 */

		public:
			static const UInt_t NCHAR = CbcEvent::NCHAR * 2;
			FeEvent( UInt_t pId){ fFeId = pId; }
			FeEvent( const FeEvent &pE ):fFeId(pE.fFeId),fBuf(0), fCbcEvents(pE.fCbcEvents){}
			~FeEvent(){}

			void AddCbc( UInt_t pCBC );
			void SetEvent( char *pEvent ); 

			//user interface
			const CbcEvent *GetCbcEvent( UInt_t pCBC )const; 
			std::string HexString();
			UInt_t Id()const{ return fFeId; }
		private:
			UInt_t fFeId;
			char *fBuf;
			std::vector<CbcEvent> fCbcEvents;

	};

	typedef std::pair<UInt_t, FeEvent> FeEventIdPair;
	typedef std::map<UInt_t, FeEvent> FeEventMap;
}
#endif

