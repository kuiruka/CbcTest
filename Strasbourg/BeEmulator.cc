#include "BeEmulator.h"
#include "Data.h"

namespace Strasbourg {

	BeEmulator::BeEmulator(): HwController( "BeEmulator" ){}
	BeEmulator::~BeEmulator(){}
	void BeEmulator::ConfigureGlib( const char *pUhalConfigFileName, const char *pBoardId ){

		Emit( "Message( const char * )", Form( "\tBeEmulator::ConfigureGlib()" ) );
	}
	void BeEmulator::StartAcquisition(){
//		Emit( "Message( const char * )", Form( "\tBeEmulator::StartAcquisition()" ) );
	}
	void BeEmulator::ReadDataInSRAM( unsigned int pNthAcq, bool pBreakTrigger, unsigned int pTimeOut ){
//		Emit( "Message( const char * )", Form( "\tBeEmulator::ReadDataInSRAM()" ) );
	}
	void BeEmulator::EndAcquisition( unsigned int pNthAcq ){
//		Emit( "Message( const char * )", Form( "\tBeEmulator::EndAcquisition()" ) );
		fNTotalAcq++;
	}
	void BeEmulator::WriteAndReadbackCbcRegValues( uint16_t pFe, std::vector<uint32_t>& pVecReq, bool pWrite ){
	}
	void BeEmulator::CbcHardReset(){
		Emit( "Message( const char * )", Form( "\tBeEmulator::CbcRegHadReset()" ) );
	}
	void BeEmulator::CbcFastReset(){
		Emit( "Message( const char * )", Form( "\tBeEmulator::CbcRegFastReset()" ) );
	}
	void BeEmulator::decodeRegInfo( uint32_t pVecReq, uint32_t &pCbcId, uint32_t &pPage, uint32_t &pAddr, uint32_t &pWrite){

		BeController::DecodeRegInfo( pVecReq, pCbcId, pPage, pAddr, pWrite);
	}
	void BeEmulator::addCbcReg(std::vector<uint32_t>& pVecReq, uint32_t pCbcId, uint32_t pPage, uint32_t pAddr, uint32_t pWrite){

		BeController::AddCbcReg( pVecReq, pCbcId, pPage, pAddr, pWrite);
	}
}

