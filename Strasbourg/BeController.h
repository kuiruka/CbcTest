#ifndef __BECONTROLLER_H__
#define __BECONTROLLER_H__
#include <time.h>
#include <string>
#include <TROOT.h>
#include <limits.h>
//#include "LocalBoostTypedef.h"
#include <boost/cstdint.hpp>
#include "HwController.h"

using namespace Cbc;

namespace uhal{
	class HwInterface;
}

namespace Strasbourg{

	class BeController : public HwController{

		public:
			static void DecodeRegInfo( uint32_t pVecReq, uint32_t &pCbcId, uint32_t &pPage, uint32_t &pAddr, uint32_t &pWrite);
			static void AddCbcReg(std::vector<uint32_t>& pVecReq, uint32_t pCbcId, uint32_t pPage, uint32_t pAddr, uint32_t pWrite);

		public:
			static const std::string fStrI2cSettings; 
			static const std::string fStrI2cCommand;
			static const std::string fStrI2cReply;
			static const uint32_t fI2cSlave;
			static const unsigned int fI2cSramSize;

			enum DEVFLAG{ DEV0 };

		public:
			BeController();
			~BeController();

			void ConfigureGlib( const char *pUhalConfigFileName, const char *pBoardId );
			void StartAcquisition();
			void ReadDataInSRAM( unsigned int pNthAcq, bool pBreakTrigger );
			void EndAcquisition( unsigned int pNthAcq );
			void WriteAndReadbackCbcRegValues( uint16_t pFe, std::vector<uint32_t>& pVecReq );
			void CbcHardReset();
			void CbcFastReset();
			uhal::HwInterface *GetBoard(){ return fBoard; }

			//functions used internally for a moment.
			void SelectSramForI2C( unsigned int pFe );
			void SelectSramForDAQ( unsigned int pNthAcq );
			bool I2cCmdAckWait( uint32_t pAckVal, unsigned int pNcount=1 );
			void SendBlockCbcI2cRequest( uint32_t pFe, std::vector<uint32_t>& pVecReq, bool pWrite);
			void WriteCbcRegValues( uint16_t pFe, std::vector<uint32_t>& pVecReq );
			void ReadCbcRegValues( uint16_t pFe, std::vector<uint32_t>& pVecReq );
			int ReadI2cValuesInSRAM( unsigned int pFe, std::vector<uint32_t> &pVecReq );

			//not tested functions
			bool CbcI2cWrite3Values(); //not tested
			void EnableI2c( bool pEnable );//not tested
			uint32_t SendI2c( uint16_t pAddr, uint16_t pData, bool pWrite );//not tested

		private:
			void decodeRegInfo( uint32_t pVecReq, uint32_t &pCbcId, uint32_t &pPage, uint32_t &pAddr, uint32_t &pWrite);
			void addCbcReg(std::vector<uint32_t>& pVecReq, uint32_t pCbcId, uint32_t pPage, uint32_t pAddr, uint32_t pWrite);
			uhal::HwInterface *fBoard;

			std::string fStrSram, fStrOtherSram, fStrSramUserLogic, fStrFull, fStrReadout;

			struct timeval fStartVeto;
			int fDevFlag;
	};
}
#endif

