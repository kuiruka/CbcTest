#include "FeEvent.h"

namespace Strasbourg {

	//FeEvent implementation

	//	const UInt_t FeEvent::NCHAR = CbcEvent::NCHAR * 2;
	const CbcEvent *FeEvent::GetCbcEvent( UInt_t pCBC )const{ 
		for( UInt_t i=0; i < fCbcEvents.size(); i++ ){
			if( fCbcEvents[i].Id() == pCBC ) {
				return &fCbcEvents[i];
			}
		}
		std::cout << "The CBC " << pCBC << " is not found for this FE " << fFeId << std::endl;
		return 0;
	}
	void FeEvent::AddCbc( UInt_t pCBC ){ fCbcEvents.push_back( CbcEvent( fFeId, pCBC ) );  }
	void FeEvent::SetEvent( char *pEvent ){

		fBuf = pEvent;
		for( unsigned int i=0; i<fCbcEvents.size(); i++ ){
			fCbcEvents[i].Set( &pEvent[i*CbcEvent::NCHAR] );

		}
		return;
	}
	std::string FeEvent::HexString(){

		std::stringbuf tmp;
		std::ostream os(&tmp);
		os << "FeEvent buf : ";
		os <<std::hex;
		for( UInt_t i=0; i<NCHAR; i++){
			os <<std::uppercase<<std::setw(2)<<std::setfill('0')<< (fBuf[i]&0xFF);
		}
		os << std::endl;
		return tmp.str();
	}

}

