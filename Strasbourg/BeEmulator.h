#ifndef __BEEMULATOR_H__
#define __BEEMULATOR_H__

#include <TQObject.h>
#include "HwController.h"
#include "BeController.h"

namespace Strasbourg{

	class BeEmulator : public HwController, public TQObject {

		public:

			enum DEVFLAG{ DEV0 };

		public:
			BeEmulator();
			~BeEmulator();

			void ConfigureGlib( const char *pUhalConfigFileName );
			void StartAcquisition();
			void ReadDataInSRAM( unsigned int pNthAcq, bool pBreakTrigger );
			void EndAcquisition( unsigned int pNthAcq );
			void WriteAndReadbackCbcRegValues( uint16_t pFe, std::vector<uint32_t>& pVecReq );
			void CbcHardReset();
			void CbcFastReset();

		private:
			void decodeRegInfo( uint32_t pVecReq, uint32_t &pCbcId, uint32_t &pPage, uint32_t &pAddr, uint32_t &pWrite);
			void addCbcReg(std::vector<uint32_t>& pVecReq, uint32_t pCbcId, uint32_t pPage, uint32_t pAddr, uint32_t pWrite);

			int fDevFlag;
	};
}
#endif

