#ifndef __CBCEVENT_H__
#define __CBCEVENT_H__

#include <string>
#include <sstream>
#include <cstring>
#include <TROOT.h>

namespace Strasbourg {

	class CbcEvent {

		/* 
		single CBC event
		The Id should corresponds to the order of in the stream data starting from 0
		 */
		public:
			static const UInt_t NCHAR = 9*4; // 9 uint32_t
			static const UInt_t NSENSOR;
			static const UInt_t NERROR;
			
			static const UInt_t OFFSET_ERROR;
			static const UInt_t WIDTH_ERROR;
			static const UInt_t OFFSET_PIPELINE_ADDRESS;
			static const UInt_t WIDTH_PIPELINE_ADDRESS;
			static const UInt_t OFFSET_CBCDATA;
			static const UInt_t WIDTH_CBCDATA;
			static const UInt_t OFFSET_GLIBFLAG;
			static const UInt_t WIDTH_GLIBFLAG;
			static const UInt_t OFFSET_CBCSTABDATA;
			static const UInt_t WIDTH_CBCSTABDATA;

			CbcEvent(UInt_t pFeId, UInt_t pId=0);
			CbcEvent(const CbcEvent &pE);			
			~CbcEvent(){}


			void Set( char *pEvent );

			//user interface
			//functions to get Error bit
			bool Error( UInt_t i )const;
			//functions to get all Error bits
			UInt_t Error()const;
			//functions to get pipeline address
			UInt_t PipelineAddress()const;
			//function to get a CBC pixel bit data
			bool DataBit(UInt_t i)const;
			//function to get bit string of CBC data
			std::string DataBitString()const;
			//function to get bit string in hexadecimal format for CBC data
			std::string DataHexString()const;
			//function to get GLIB flag string
			std::string GlibFlagString()const;
			//function to get bit string for CBC STAB data
			std::string StubBitString()const;
			//function to get char at the global data string at position 8*i
			char Char( UInt_t pBytePosition ){ return fBuf[pBytePosition]; }
			//function to get the bit at the global data string position 
			bool Bit( UInt_t pPosition )const;

			//function to get bit string from the data offset and width
			std::string BitString( UInt_t pOffset, UInt_t pWidth )const;
			UInt_t Id()const{ return fCbcId; }
		private:
			UInt_t fFeId;
			UInt_t fCbcId;
			char *fBuf;
	};
}
#endif

