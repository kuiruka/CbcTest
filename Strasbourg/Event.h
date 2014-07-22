#ifndef __EVENT_H__
#define __EVENT_H__

#include "FeEvent.h"
#include "CbcEvent.h"

namespace Strasbourg {

	class Event {


		/*
		   id of FeEvent should be the order of FeEvents in data stream starting from 0
		   id of CbcEvent also should be the order of CBCEvents in data stream starting from 0
		 */
		public:
//			static const UInt_t EVENT_SIZE_32;
			static const UInt_t OFFSET_BUNCH; 
			static const UInt_t WIDTH_BUNCH; 
			static const UInt_t OFFSET_ORBIT; 
			static const UInt_t WIDTH_ORBIT; 
			static const UInt_t OFFSET_LUMI; 
			static const UInt_t WIDTH_LUMI; 
			static const UInt_t OFFSET_EVENT_COUNT; 
			static const UInt_t WIDTH_EVENT_COUNT; 
			static const UInt_t OFFSET_EVENT_COUNT_CBC; 	
			static const UInt_t WIDTH_EVENT_COUNT_CBC; 
			static const UInt_t OFFSET_FE_EVENT; 
			static const UInt_t WIDTH_FE_EVENT;
			static const UInt_t OFFSET_TDC;
			static const UInt_t WIDTH_TDC;
			Event(): fSize32(6){}
			Event(Event &pE): fBuf(0), fSize32(pE.fSize32), fBunch(pE.fBunch), fOrbit(pE.fOrbit), fLumi(pE.fLumi), 
			fEventCount(pE.fEventCount), fEventCountCBC(pE.fEventCountCBC),fTDC(pE.fTDC){}
			~Event(){}
			void Clear(){ fEvent.clear(); fSize32 = 6; }
			void AddFe( UInt_t pFE, bool pDummy = false ); 
			void AddCbc( UInt_t pFE, UInt_t pCBC ); 
			UInt_t Size32()const{ return fSize32; }
			int SetEvent( char *pEvent );
			//user interface
			const FeEvent *GetFeEvent( UInt_t pFE )const;
			UInt_t GetBunch()const{ return fBunch; }
			UInt_t GetOrbit()const{ return fOrbit; }
			UInt_t GetLumi()const{ return fLumi; }
			UInt_t GetEventCount()const{ return fEventCount; }
			UInt_t GetEventCountCBC()const{ return fEventCountCBC; }
			UInt_t GetTDC()const{ return fTDC; }
			std::string HexString()const;

		private:

			FeEvent *findFeEvent( UInt_t pFE );

			char *fBuf;
			UInt_t fSize32;
			FeEventMap fEvent;
			UInt_t fBunch;
			UInt_t fOrbit;
			UInt_t fLumi;
			UInt_t fEventCount;
			UInt_t fEventCountCBC;
			UInt_t fTDC;
	};
}
#endif


