#ifndef __HWCONTROLLER_H__
#define __HWCONTROLLER_H__

#include <string>
#include <map>
#include <vector>
#include <boost/cstdint.hpp>
#include "Cbc/CbcRegInfo.h"

using namespace Cbc;

namespace Strasbourg{

	typedef std::pair< std::string, unsigned int > GlibItem;
	typedef std::map< std::string, unsigned int > GlibSetting; 

	class Data;
	class Event;

	//first: FE id
	class CbcRegUpdateMap : public std::map< unsigned int, std::vector<uint32_t> > {
		public:
			CbcRegUpdateMap(){}
			~CbcRegUpdateMap(){}
			void Reset(unsigned int pNFe );
			void ClearList();
			int GetListSize();
	};

	class HwController {

		public:
			HwController( const char *pName );
			virtual ~HwController();

			virtual void ConfigureGlib( const char *pUhalConfigFileName )=0;
			virtual void StartAcquisition()=0;
			virtual void ReadDataInSRAM( unsigned int pNthAcq, bool pBreakTrigger, unsigned int pTimeOut = 1 )=0;
			virtual void EndAcquisition( unsigned int pNthAcq )=0;
			virtual void WriteAndReadbackCbcRegValues( uint16_t pFe, std::vector<uint32_t>& pVecReq, bool pWrite = true )=0;
			virtual void CbcHardReset()=0;
			virtual void CbcFastReset()=0;
			virtual void CbcI2cRefresh()=0;

			void ConfigureGlibController( const char *pUhalConfigFileName );
			void ConfigureCbc();

			const std::string & Name()const{ return fName; }
			const Event *GetNextEvent()const;
			const char *GetBuffer( UInt_t &pBufSize )const;
			const GlibSetting &GetGlibSetting()const{ return fGlibSetting; } 
			unsigned int GetGlibSetting( const char * pName )const;
			const std::string &GetCbcRegSettingFileName( unsigned int pFe, unsigned int pCbc )const; 
			const CbcRegMap &GetCbcRegSetting()const{ return fCbcRegSetting; }
			unsigned int GetNumberOfTotalAcq()const{ return fNTotalAcq; }
			unsigned int GetNumberOfNoDataAcq()const{ return fNoDataAcq; }

			std::vector<const CbcRegItem *> UpdateCbcRegValues( bool pWrite = true );
			std::vector<const CbcRegItem *> ReConfigureCbc();
			std::vector<const CbcRegItem *> ReConfigureCbc( unsigned int pFe, unsigned int pCbc );
			std::vector<const CbcRegItem *> ReadCbcRegisters();
			std::vector<const CbcRegItem *> ReadCbcRegisters( unsigned int pFe, unsigned int pCbc );
			void AddGlibSetting( const char * pName, unsigned int pValue );
			void SetGlibSetting( const char * pName, unsigned int pValue );
			void SetCbcRegSettingFileName( unsigned int pFe, unsigned int pCbc, const std::string pName ); 
			void AddCbcRegUpdateItem( unsigned int pFe, unsigned int pCbc, unsigned int pPage, unsigned int pAddr, unsigned int pVal );
			void AddCbcRegUpdateItemsForEnableTestPulse( unsigned int pFe, unsigned int pCbc, bool pEnable );
			void AddCbcRegUpdateItemsForNewTestPulseGroup( unsigned int pFe, unsigned int pCbc, unsigned int pTestPulseGroup );
			void AddCbcRegUpdateItemsForNewTestPulseDelay( unsigned int pFe, unsigned int pCbc, unsigned int pTestPulseDelay );
			void AddCbcRegUpdateItemsForNewVCth( unsigned int pVCth );
			void SetCbcRegUpdateListForRead( unsigned int pFe, unsigned int pCbc );
			std::vector<const CbcRegItem *> ResetCbcRegUpdateList();
			void LoadCbcRegInfo( unsigned int pFe, unsigned int pCbc );
			void SaveCbcRegInfo( unsigned int pFe, unsigned int pCbc );
			void SaveCbcRegInfo( const char *newdir = 0 );
			unsigned int NCbcI2cWritePage1()const{ return fNCbcI2cWritePage1; }
			unsigned int NCbcI2cWritePage2()const{ return fNCbcI2cWritePage2; }
			unsigned int NreadBlock()const{ return fNreadBlock; }


		protected:
			Data *getData()const{ return fData; }
			virtual void decodeRegInfo( uint32_t pVecReq, uint32_t &pCbcId, uint32_t &pPage, uint32_t &pAddr, uint32_t &pWrite)=0;
			virtual void addCbcReg(std::vector<uint32_t>& pVecReq, uint32_t pCbcId, uint32_t pPage, uint32_t pAddr, uint32_t pWrite)=0;

			std::vector<const CbcRegItem *> setReadValueToCbcRegSettings( unsigned int pFe, std::vector<uint32_t> &pReadVecReq );	
			const CbcRegItem * setReadValueToCbcRegSetting( unsigned int pFe, unsigned int pCbc, unsigned int pPage, unsigned int pAddr, unsigned int pValue ); 

			Data *fData;
			unsigned int fPacketSize;

			GlibSetting fGlibSetting; 
			CbcRegMap fCbcRegSetting;
			CbcRegUpdateMap fCbcRegUpdateList; 

			unsigned int fNreadBlock;
			std::string  fName;
			unsigned int fNFe;
			unsigned int fNCbc;
			unsigned int fNeventPerAcq;
			unsigned int fNTotalAcq;
			unsigned int fNoDataAcq;

			unsigned int fNCbcI2cWritePage1;
			unsigned int fNCbcI2cWritePage2;

	};
}

#endif

