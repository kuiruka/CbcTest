#include "CbcEvent.h"
#include <iomanip>

namespace Strasbourg {
	/*
	   CbcEvent implementation
	   fBuf defined in CBC2 specification + GLIB internals + STUB bits
	   Error(00) Address(8) Data(254) GLIB(12) STUB(12)
	 */
	const UInt_t CbcEvent::NSENSOR = 254;
	const UInt_t CbcEvent::NERROR = 2;

	const UInt_t CbcEvent::OFFSET_ERROR = 0;
	const UInt_t CbcEvent::WIDTH_ERROR = 2;
	const UInt_t CbcEvent::OFFSET_PIPELINE_ADDRESS = OFFSET_ERROR + WIDTH_ERROR;
	const UInt_t CbcEvent::WIDTH_PIPELINE_ADDRESS = 8;
	const UInt_t CbcEvent::OFFSET_CBCDATA = OFFSET_PIPELINE_ADDRESS + WIDTH_PIPELINE_ADDRESS;
	const UInt_t CbcEvent::WIDTH_CBCDATA = NSENSOR;
	const UInt_t CbcEvent::OFFSET_GLIBFLAG = OFFSET_CBCDATA + WIDTH_CBCDATA;
	const UInt_t CbcEvent::WIDTH_GLIBFLAG = 12;
	const UInt_t CbcEvent::OFFSET_CBCSTABDATA = OFFSET_GLIBFLAG + WIDTH_GLIBFLAG;
	const UInt_t CbcEvent::WIDTH_CBCSTABDATA = 12;

	CbcEvent::CbcEvent(UInt_t pFeId, UInt_t pId): 
		fFeId(pFeId),
		fCbcId(pId), 
		fBuf(0){ 
		}
	CbcEvent::CbcEvent(const CbcEvent &pE):
		fFeId(pE.fFeId),
		fCbcId(pE.fCbcId), 
		fBuf(0){
		}

	void CbcEvent::Set( char *pEvent ){ fBuf = pEvent; } 

	bool CbcEvent::Error( UInt_t i )const{ 

		return Bit( i+OFFSET_ERROR );	
	}
	UInt_t CbcEvent::Error()const{ 

		UInt_t val(0);
		UInt_t cWidth = WIDTH_ERROR;
		for( UInt_t i=0; i<cWidth; i++ ){
			val = val<<1; val |= Error(i);
		}
		return val; 
	}

	UInt_t CbcEvent::PipelineAddress()const{ 
		UInt_t cOffset = OFFSET_PIPELINE_ADDRESS;
		UInt_t cWidth = WIDTH_PIPELINE_ADDRESS;
		UInt_t val(0);
		for( UInt_t i=0; i<cWidth; i++ ) {
			val = val<<1; val |= Bit( cOffset+i );
		}
		return val;
	}
	bool CbcEvent::DataBit(UInt_t i)const{

		if( i > NSENSOR ) return 0;
		return Bit( i+OFFSET_CBCDATA ); 
	}
	//function to get bit string of CBC data
	std::string CbcEvent::DataBitString()const{

		return BitString( OFFSET_CBCDATA, WIDTH_CBCDATA );
	}
	//function to get bit string in hexadecimal format for CBC data
	std::string CbcEvent::DataHexString()const{

		std::stringbuf tmp;
		std::ostream os(&tmp); 

		os << std::hex;

		UInt_t cFirstByteP = OFFSET_CBCDATA/8;
		UInt_t cFirstBitP = OFFSET_CBCDATA%8;
		UInt_t cLastByteP = (cFirstByteP+WIDTH_CBCDATA-1)/8;
		UInt_t cLastBitP = (cFirstByteP+WIDTH_CBCDATA-1)%8;

		UInt_t cMask(0);
		UInt_t cMaskLastBit(0);
		UInt_t cMaskWidth(0);
		//First byte	
		cMaskLastBit = cFirstByteP < cLastByteP ? 7 : cLastBitP;
		cMaskWidth = cMaskLastBit - cFirstBitP + 1;
		cMask = ( 1 << ( 7 - cMaskLastBit ) );	
		for( UInt_t i=0;i < cMaskWidth; i++ ) { cMask = cMask << 1; cMask |= 1; } 
		os <<std::uppercase<<std::setw(2)<<std::setfill('0')<< (fBuf[cFirstByteP]&cMask);
		if( cFirstByteP == cLastByteP ) return tmp.str();
		//Second to the second last byte	
		if( cFirstByteP != cLastByteP - 1 ){
			for( UInt_t j=cFirstByteP+1; j<cLastByteP; j++ )os <<std::uppercase<<std::setw(2)<<std::setfill('0')<< (fBuf[j] & 0xFF);

		}
		//Last byte	
		cMaskLastBit = cLastBitP;
		cMaskWidth = cMaskLastBit+1;
		cMask = ( 1 << ( 7 - cMaskLastBit ) );	
		for( UInt_t i=0;i < cMaskWidth; i++ ) { cMask = cMask << 1; cMask |= 1; } 
		os <<std::uppercase<<std::setw(2)<<std::setfill('0')<< (fBuf[cFirstByteP]&cMask);
		return tmp.str();
	}
	//function to get GLIB flag string
	std::string CbcEvent::GlibFlagString()const{

		return BitString( OFFSET_GLIBFLAG, WIDTH_GLIBFLAG );
	}
	//function to get bit string for CBC STAB data
	std::string CbcEvent::StubBitString()const{

		return BitString( OFFSET_CBCSTABDATA, WIDTH_CBCSTABDATA );
	}
	//function to get the bit at the global data string position 
	bool CbcEvent::Bit( UInt_t pPosition )const{

		UInt_t cByteP=pPosition/8;
		UInt_t cBitP =pPosition%8;
		return fBuf[cByteP] & (1<<(7-cBitP));
	}
	//function to get bit string from the data offset and width
	std::string CbcEvent::BitString( UInt_t pOffset, UInt_t pWidth )const{

		std::stringbuf tmp;
		std::ostream os(&tmp);
		for( UInt_t i=0; i<pWidth; i++ ) os << Bit( i+pOffset );
		return tmp.str();
	}
}
