#include "Event.h"


void swap_byte_order( const void *org, void *swapped, unsigned int nbyte ){

	for( unsigned int i=0; i<nbyte; i++ ){
		((char *)swapped)[i] = ((char *)org)[nbyte-1-i];
	}
}
namespace Strasbourg {

	//Event implementation
	/*
	   single event data size(/uint32_t) from GLIB board for strusburg BE firmware.
	   fixed size for 2 FE(FMC) x 2CBC (4*9) and  header (5) + 1
	 */
//	const UInt_t Event::EVENT_SIZE_32 = 4*9+6;
	const UInt_t Event::OFFSET_BUNCH = 8; 
	const UInt_t Event::WIDTH_BUNCH = 24; 
	const UInt_t Event::OFFSET_ORBIT = 1 * 32 + 8; 
	const UInt_t Event::WIDTH_ORBIT = 24; 
	const UInt_t Event::OFFSET_LUMI = 2 * 32 + 8; 
	const UInt_t Event::WIDTH_LUMI = 24; 
	const UInt_t Event::OFFSET_EVENT_COUNT = 3 * 32 + 8; 
	const UInt_t Event::WIDTH_EVENT_COUNT = 24; 
	const UInt_t Event::OFFSET_EVENT_COUNT_CBC = 4 * 32 + 8; 	
	const UInt_t Event::WIDTH_EVENT_COUNT_CBC = 3 * 8; 
	const UInt_t Event::OFFSET_FE_EVENT = 5 * 4; 
	const UInt_t Event::WIDTH_FE_EVENT = CbcEvent::NCHAR * 2;
	const UInt_t Event::OFFSET_TDC = 5 * 32 + WIDTH_FE_EVENT;
	const UInt_t Event::WIDTH_TDC = 32;

	void Event::AddFe( UInt_t pFE, bool pDummy ){ 

		if( pDummy == 0 ){ 
			AddCbc( pFE, 0 );
			AddCbc( pFE, 1 );
		}
		fSize32 += ( 2 * 9 ); 
	}
	void Event::AddCbc( UInt_t pFE, UInt_t pCBC ){ 
		FeEvent *cFeEvent = findFeEvent( pFE );
		if( cFeEvent == 0 ){
			fEvent.insert( FeEventIdPair( pFE, FeEvent(pFE) ) );
			cFeEvent = &(fEvent.find(pFE)->second);
		}
		cFeEvent->AddCbc( pCBC );
	}
	int Event::SetEvent( char *pEvent ){
		fBuf = pEvent;
		int vsize = sizeof(UInt_t);
		UInt_t swapped(0);
		swap_byte_order( &pEvent[0*vsize], &swapped, vsize ); 
		fBunch = swapped & 0xFFFFFF ;
		swap_byte_order( &pEvent[1*vsize], &swapped, vsize ); 
		fOrbit = swapped & 0xFFFFFF ;
		swap_byte_order( &pEvent[2*vsize], &swapped, vsize ); 
		fLumi = swapped & 0xFFFFFF ;
		swap_byte_order( &pEvent[3*vsize], &swapped, vsize ); 
		fEventCount = swapped & 0xFFFFFF ;
		swap_byte_order( &pEvent[4*vsize], &swapped, vsize ); 
		fEventCountCBC = swapped & 0xFFFFFF ;
		swap_byte_order( &pEvent[OFFSET_TDC], &swapped, vsize ); 
		fTDC = swapped & 0xFFFFFF ;

		FeEventMap::iterator it = fEvent.begin();
		for( ; it != fEvent.end(); it++ ){
			UInt_t id = it->first;
			it->second.SetEvent( &pEvent[OFFSET_FE_EVENT + id * FeEvent::NCHAR ] );
		}
		return 0;
	}
	const FeEvent *Event::GetFeEvent( UInt_t pFE )const{
		FeEventMap::const_iterator cIt = fEvent.find(pFE);
		if( cIt == fEvent.end() ){
			std::cout << "Event: FE " << pFE << " is not found." << std::endl;
			return 0;
		}
		return &(cIt->second);
	}
	std::string Event::HexString()const{

		std::stringbuf tmp;
		std::ostream os(&tmp);

		os <<std::hex;
		for( UInt_t i=0; i< fSize32; i++){
			os <<std::uppercase<<std::setw(2)<<std::setfill('0')<< (fBuf[i]&0xFF);
		}
		os << std::endl;
		return tmp.str();
	}
	FeEvent *Event::findFeEvent( UInt_t pFE ){

		FeEventMap::iterator cIt = fEvent.find(pFE);
		if( cIt == fEvent.end() ) return 0;

		return &(cIt->second);
	}
}

