#include <uhal/uhal.hpp>
#include <boost/format.hpp>
#include <boost/date_time.hpp>
#include <boost/thread.hpp>
#include <time.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "Calibrator.h"
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include "../CbcDaq/GUIFrame.h"
#include "Strasbourg/BeController.h"
#include "Strasbourg/BeEmulator.h"
#include "Strasbourg/Data.h"
#include "utils/Utilities.h"
#include "utils/Exception.h"
#include "Analysers/ScurveAnalyser.h"
#include "TCanvas.h"
#include <TThread.h>
#include <sstream>

using namespace CbcDaq;

namespace ICCalib{

	Calibrator::Calibrator( const char *pConfigFile ):
		DAQController( pConfigFile ),
		fCalibSetting(),
		fScurveAnalyser(0),
		fTestGroupMap(0),
		fGroupList(),
		fNonTestGroupOffset(0xFF)
	{
	}

	Calibrator::~Calibrator(){

		delete fTestGroupMap;

	}
	void Calibrator::initialiseSetting(){
		//CalibSetting initialisation
		fCalibSetting.insert( CalibItem( "NAcq", 10 ) );
		fCalibSetting.insert( CalibItem( "EnableTestPulse", 0x00 ) );
		fCalibSetting.insert( CalibItem( "SkipVplusTuning", 0x00 ) );
		fCalibSetting.insert( CalibItem( "QuickVplusTuning", 0x00 ) );
		fCalibSetting.insert( CalibItem( "SkipOffsetTuning", 0x01 ) );
		fCalibSetting.insert( CalibItem( "TestPulsePotentiometer", 0xF1 ) );
		fCalibSetting.insert( CalibItem( "InitialOffset", 0x00 ) );
		fCalibSetting.insert( CalibItem( "TargetOffset", 0x50 ) );
		fCalibSetting.insert( CalibItem( "TargetVCth", 0x78 ) );
		fCalibSetting.insert( CalibItem( "VplusMin.", 0x60 ) );
		fCalibSetting.insert( CalibItem( "VplusMax.", 0x90 ) );
		fCalibSetting.insert( CalibItem( "VplusStep", 0x10 ) );
	}

	void Calibrator::ConfigureAnalyser(){

		fNonTestGroupOffset = 0xFF;
		if( !fNegativeLogicCBC ) fNonTestGroupOffset = 0x00;

		TString msg;
		CalibSetting::const_iterator cItC = fCalibSetting.begin();
		for( ; cItC != fCalibSetting.end(); cItC++ ){
			msg = Form( "\tCalib_%-30s : 0x%02X", cItC->first.c_str(), cItC->second );
			Emit( "Message( const char * )", msg.Data() ); 
		}
		UInt_t cTargetOffset    = fCalibSetting.find( "TargetOffset"           )->second;
		UInt_t cTargetVCth      = fCalibSetting.find( "TargetVCth"             )->second;
		UInt_t cEnableTestPulse = fCalibSetting.find( "EnableTestPulse"        )->second;
		UInt_t cTPPot           = fCalibSetting.find( "TestPulsePotentiometer" )->second;
		fNAcq                   = fCalibSetting.find( "NAcq"                   )->second;

		for( UInt_t cFe = 0; cFe < fNFe; cFe++ ){
			for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
				fHwController->AddCbcRegUpdateItemsForEnableTestPulse( cFe, cCbc, cEnableTestPulse );
				if( cEnableTestPulse ) fHwController->AddCbcRegUpdateItem( cFe, cCbc, 0, 0x0D, cTPPot ); 
			}
		}
		UpdateCbcRegValues();
		if( fOutputDir == "" ) fOutputDir = Form( "BE%01uNeg%dOff%XVCth%X", fBeId, fNegativeLogicCBC, cTargetOffset, cTargetVCth );
		system( Form( "mkdir -p %s", fOutputDir.c_str() ) );

		delete fTestGroupMap;
		delete fScurveAnalyser;
		fTestGroupMap   = new CalibrationTestGroupMap( *fTestPulseGroupMap );
		fScurveAnalyser = new ScurveAnalyser( fBeId, fNFe, fNCbc, 
				fTestGroupMap, &(fHwController->GetCbcRegSetting()),
				fNegativeLogicCBC, cTargetVCth, fOutputDir.c_str(), fGUIFrame );
		fScurveAnalyser->Initialise();
		fAnalyser = fScurveAnalyser;
		Emit( "Message( const char * )", "Calibration Configured." ); 

		return;
	}

	void Calibrator::Calibrate(){

		fStop = false;
		UInt_t cInitialNTotalAcq = fHwController->GetNumberOfTotalAcq();
		UInt_t cNCbcI2cWritePage1_INIT = fHwController->NCbcI2cWritePage1();
		UInt_t cNCbcI2cWritePage2_INIT = fHwController->NCbcI2cWritePage2();

		struct timeval start;
		gettimeofday( &start, 0 );

		//test pulse setting( default setting : disabled )
		UInt_t cEnableTestPulse = fCalibSetting.find( "EnableTestPulse" )->second;
		if( cEnableTestPulse ){
			for( UInt_t cFe = 0; cFe < fNFe; cFe++ ){
				for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
					fHwController->AddCbcRegUpdateItemsForEnableTestPulse( cFe, cCbc, cEnableTestPulse );
				}
			}
			UpdateCbcRegValues();
		}

		UInt_t cSkipVplusTuning = fCalibSetting.find( "SkipVplusTuning" )->second;
		if( !cSkipVplusTuning ){

			UInt_t cQuickVplusTuning = fCalibSetting.find( "QuickVplusTuning" )->second;
			if( cQuickVplusTuning ){
				std::vector< std::vector<unsigned int> > cVplus1 = FindVplusQuick(true);
//				std::vector< std::vector<unsigned int> > cVplus2 = FindVplusQuick(false);

				for( UInt_t cFe=0; cFe < fNFe; cFe++ ){

					for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){

						unsigned int cValue1 = cVplus1.at(cFe).at(cCbc);
//						unsigned int cValue2 = cVplus2.at(cFe).at(cCbc);
//						unsigned int cValue = ( ( cValue1 + cValue2 ) / 2.0 + 0.5 );
						uint32_t cPage(0), cAddr(0x0B);
//						Emit( "Message( const char * )", Form( "\tVplus for FE %d CBC %d is %X, %X, %X.", cFe, cCbc, cValue1, cValue2, cValue ) );
						Emit( "Message( const char * )", Form( "\tVplus for FE %d CBC %d is %X.", cFe, cCbc, cValue1) );
//						fHwController->AddCbcRegUpdateItem( cFe, cCbc, cPage, cAddr, cValue );
					}
				}
//				UpdateCbcRegValues();

			}
			else{
				FindVplus();
				if( fStop ) return;

				//Update Vplus
				for( UInt_t cFe=0; cFe < fNFe; cFe++ ){

					for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){

						uint32_t cPage(0), cAddr(0x0B), cValue(0);
						cValue = fScurveAnalyser->GetVplus( cFe, cCbc );
						Emit( "Message( const char * )", Form( "\tVplus for FE %d CBC %d is %X.", cFe, cCbc, cValue ) );
						fHwController->AddCbcRegUpdateItem( cFe, cCbc, cPage, cAddr, cValue );
					}
				}
				UpdateCbcRegValues();
			}
		}
		long mtime_VPLUS = getTimeTook( start, 1 );
		long min_VPLUS = (mtime_VPLUS/1000)/60;
		long sec_VPLUS = (mtime_VPLUS/1000)%60;
		UInt_t cFinalNTotalAcq_VPLUS = fHwController->GetNumberOfTotalAcq();
		UInt_t cNAcq_VPLUS = cFinalNTotalAcq_VPLUS - cInitialNTotalAcq;
		UInt_t cNCbcI2cWritePage1_VPLUS = fHwController->NCbcI2cWritePage1() - cNCbcI2cWritePage1_INIT;
		UInt_t cNCbcI2cWritePage2_VPLUS = fHwController->NCbcI2cWritePage2() - cNCbcI2cWritePage2_INIT;
		TString msg = Form( "\tTime took for VPLUS = %ld min %ld sec.", min_VPLUS, sec_VPLUS ); 
		Emit( "Message( const char * )", msg.Data() );
		Emit( "Message( const char * )", Form( "Total %d acquisitions (%d events per acq.) are made for VPLUS scan.", 
					cNAcq_VPLUS, fNeventPerAcq ) );
		Emit( "Message( const char * )", Form( "Total %d I2C page 1 register write are made for VPLUS scan.", cNCbcI2cWritePage1_VPLUS ) );
		Emit( "Message( const char * )", Form( "Total %d I2C page 2 register write are made for VPLUS scan.", cNCbcI2cWritePage2_VPLUS ) );
		Emit( "Message( const char * )", Form( "Total # of readBlock() for VPLUS scan = %d.",  cFinalNTotalAcq_VPLUS ) );

		UInt_t cSkipOffsetTuning = fCalibSetting.find( "SkipOffsetTuning" )->second;
		if( cSkipOffsetTuning ){
			Emit( "Message( const char * )", "FinishedCalibration" );
			return; 
		}

		CalibrateOffsets();
		if( fStop ) return;
		//fScurveAnalyser->DumpResult();
		SetCalibratedOffsets(); 

		SaveCbcRegInfo();

		long mtime = getTimeTook( start, 1 );
		long min = (mtime/1000)/60;
		long sec = (mtime/1000)%60;
		UInt_t cFinalNTotalAcq = fHwController->GetNumberOfTotalAcq();
		UInt_t cNAcq = cFinalNTotalAcq - cInitialNTotalAcq;
		UInt_t cNCbcI2cWritePage1 = fHwController->NCbcI2cWritePage1();
		UInt_t cNCbcI2cWritePage2 = fHwController->NCbcI2cWritePage2();


		msg = Form( "\tTime took for this offset scan = %ld min %ld sec.", min - min_VPLUS, sec - sec_VPLUS ); 
		Emit( "Message( const char * )", msg.Data() );
		Emit( "Message( const char * )", Form( "Total %d acquisitions (%d events per acq.) are made for this offset scan.", 
					cNAcq - cNAcq_VPLUS, fNeventPerAcq ) );
		Emit( "Message( const char * )", Form( "Total %d I2C page 1 register write are made for this offset scan.", 
					cNCbcI2cWritePage1 - cNCbcI2cWritePage1_VPLUS) );
		Emit( "Message( const char * )", Form( "Total %d I2C page 2 register write are made for this offset scan.", 
					cNCbcI2cWritePage2 - cNCbcI2cWritePage2_VPLUS) );

		msg = Form( "\tTime took for this calibration loop = %ld min %ld sec.", min, sec ); 
		Emit( "Message( const char * )", msg.Data() );
		Emit( "Message( const char * )", Form( "Total %d acquisitions (%d events per acq.) are made for this calibration.", cNAcq, fNeventPerAcq ) );
		Emit( "Message( const char * )", Form( "Total %d I2C page 1 register write are made for this calibration.", cNCbcI2cWritePage1 ) );
		Emit( "Message( const char * )", Form( "Total %d I2C page 2 register write are made for this calibration.", cNCbcI2cWritePage2 ) );

		Emit( "Message( const char * )", "FinishedCalibration" );

		return;
	}

	void Calibrator::VCthScanForAllGroups(){

		//VCthScan with current offsets for all test groups.

		fStop = false;

		Int_t cTargetBit = 8;

		fScurveAnalyser->SetOffsets();

		for( UInt_t cTestGroup=0; cTestGroup<fTestGroupMap->size(); cTestGroup++ ){

			UInt_t cEnableTestPulse = fCalibSetting.find( "EnableTestPulse" )->second;
			if( cEnableTestPulse ){
				ActivateGroup( cTestGroup );
			}
			fTestGroupMap->Activate( cTestGroup );

			fScurveAnalyser->Configure( ScurveAnalyser::SINGLEVCTHSCAN );

			UInt_t cMinVCth(0), cMaxVCth(0xFF);
			ConfigureCbcOffset( cTargetBit, cMinVCth, cMaxVCth );

			VCthScan( cMinVCth, cMaxVCth );

			if( fStop ) return;

			fScurveAnalyser->FitHists( cMinVCth, cMaxVCth );
			if( fGUIFrame ){
				fScurveAnalyser->DrawHists();
				fScurveAnalyser->PrintScurveHistogramDisplayPads();
			}
			Emit( "Message( const char * )", Form( "\tVCthScan for TestGroup = %d finished.", cTestGroup ) );
		}
		SetCalibratedOffsets();

		Emit( "Message( const char * )", "FinishedVCthScan");
		return;
	}

	const CalibrationResult *Calibrator::GetCalibrationResult()const{

		if( fScurveAnalyser ){
			return &fScurveAnalyser->GetResult();
		}
		return 0;
	}
	void Calibrator::SetVplusVsVCth0GraphDisplayPad( UInt_t pFeId, TPad *pPad ){

		if( fScurveAnalyser ){
			fScurveAnalyser->SetVplusVsVCth0GraphDisplayPad( pFeId, pPad );
		}
	}
	void Calibrator::SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){

		if( fScurveAnalyser ){
			fScurveAnalyser->SetScurveHistogramDisplayPad( pFeId, pCbcId, pPad );
		}
	}
	void Calibrator::ReadSettingFile( const char *pFname ){

		DAQController::ReadSettingFile( pFname );

		//Configuration file settings are set to CbcConfigFileMap, and CalibSetting.
		Configuration::iterator cIt = fConfiguration.begin();
		for( ; cIt != fConfiguration.end(); cIt++ ){

			std::string cType = cIt->first;
			std::string cValue = cIt->second;

			if( cType.find( "Calib" ) != std::string::npos ){
				std::string cName = cType.substr( 6, std::string::npos );
				CalibSetting::iterator cIt = fCalibSetting.find( cName.c_str() );
				if( cIt != fCalibSetting.end() ) cIt->second = strtol( cValue.c_str(), 0, 16 ); 
			}
		}
	}

	Int_t Calibrator::GetDataStreamHistId( UInt_t pFeId, UInt_t pCbcId ){

		return fNCbc * pFeId + pCbcId;

	}
	std::vector< std::vector<unsigned int > > Calibrator::FindVplusQuick( bool pStartFrom0 ){

		unsigned int cInitVplus(0);
		if( pStartFrom0 == false ) cInitVplus = 0xFF;

		fScurveAnalyser->SetOffsets( fCalibSetting.find( "TargetOffset" )->second );
		int cVCth = fCalibSetting.find( "TargetVCth"             )->second;
		fHwController->AddCbcRegUpdateItemsForNewVCth( cVCth );
		UpdateCbcRegValues();

		std::vector< std::vector< std::vector<unsigned int> > > cVplus( fNFe );
		for( unsigned int cFeId=0; cFeId < fNFe; cFeId++ ){
			std::vector<std::vector<unsigned int> > cVplusCbc( fNCbc );
			for( unsigned int cCbcId=0; cCbcId < fNCbc; cCbcId++ ){
				std::vector<unsigned int> cVplusGroup( fTestGroupMap->size(), cInitVplus );
				cVplusCbc.at(cCbcId) = cVplusGroup; 
			}
			cVplus.at(cFeId) = cVplusCbc;
		}

		for( UInt_t cTestGroupId=0; cTestGroupId<fTestGroupMap->size(); cTestGroupId++ ){

			UInt_t cEnableTestPulse = fCalibSetting.find( "EnableTestPulse" )->second;
			if( cEnableTestPulse ){
				ActivateGroup( cTestGroupId );
			}
			fTestGroupMap->Activate( cTestGroupId );
			UInt_t cMinVCth(0), cMaxVCth(0xFF);//this is just for ConfigureCbcOffset function. 
			ConfigureCbcOffset( 8, cMinVCth, cMaxVCth );

			CalibrationTestGroup *cTestGroup = fTestGroupMap->GetActivatedGroup();

			//UInt_t cNthAcq = 0;
			for( Int_t cTargetBit = 7; cTargetBit >= -1; cTargetBit-- ){

/*
				TString msg = Form( "\tVplus Target Bit : %d.", cTargetBit ); 
				Emit( "Message( const char * )", msg.Data() );
				*/

				std::vector< std::vector<int> > cNhit( fNFe );
				for( unsigned int i=0; i < fNFe; i++ ){
					std::vector<int> cNhitCbc(fNCbc, 0 );
					cNhit.at(i) = cNhitCbc; 
				}

				if( cTargetBit >= 0 ){
					for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

						for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

							if( pStartFrom0 )
								cVplus.at(cFeId).at(cCbcId).at(cTestGroupId) |=  1 << cTargetBit;
							else
								cVplus.at(cFeId).at(cCbcId).at(cTestGroupId) &=  ~( 1 << cTargetBit );

						}
					}
				}
				ConfigureVplus( cVplus, cTestGroupId );

				unsigned int cNthAcq(0);
				for( unsigned int i=0; i< fNAcq; i++ ){

					fHwController->StartAcquisition();
					fHwController->ReadDataInSRAM( cNthAcq, true );
					fHwController->EndAcquisition( cNthAcq );

					bool cFillDataStream( true );

					int cNevent(0);	
					const Event *cEvent = fHwController->GetNextEvent();

					while( cEvent ){

						for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

							const FeEvent *cFeEvent = cEvent->GetFeEvent(cFeId);	

							for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

								const CbcEvent *cCbcEvent = cFeEvent->GetCbcEvent( cCbcId );

								for( UInt_t cChannelId=0; cChannelId < CbcEvent::NSENSOR; cChannelId++ ){

									if( ! cTestGroup->Has( cChannelId ) ) continue;

									bool bit = cCbcEvent->DataBit( cChannelId );
									if( bit ) cNhit.at(cFeId).at(cCbcId)++; 
								}
							}
						}
						cNevent++;
						cFillDataStream = false;
						cEvent = fHwController->GetNextEvent();
					}//Event
				}//Nacq
				for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

					for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

						double Ntotal = ( cTestGroup->size() * fNAcq * fNeventPerAcq );
						double cOcc = (double)cNhit.at(cFeId).at(cCbcId) / Ntotal;
						double cOccError = sqrt( cOcc * ( 1 - cOcc ) / Ntotal ); 
//						if( cTargetBit == -1 ){
							TString msg = Form( "\tFE: %2d, CBC: %2d, Group Id : %d, Vplus : %2d, Occupancy : %1.3f+/-%1.3f.", 
									cFeId, cCbcId, cTestGroupId, cVplus.at(cFeId).at(cCbcId).at(cTestGroupId), cOcc, cOccError ); 
							Emit( "Message( const char * )", msg.Data() );
//						}

						if( cTargetBit >= 0 ){
							if( pStartFrom0 & cOcc < 0.5 ){
								cVplus.at(cFeId).at(cCbcId).at(cTestGroupId) &= ~( 1 << cTargetBit );
							}
							else if( !pStartFrom0 & cOcc > 0.5 ){
								cVplus.at(cFeId).at(cCbcId).at(cTestGroupId) |= 1 << cTargetBit;
							}
						}
					}
				}
				if( fStop ) break;
			}//TargetBit
		}//Group
		std::vector< std::vector<unsigned int> > cVplusFinal( fNFe );
		for( unsigned int cFeId=0; cFeId < fNFe; cFeId++ ){
			std::vector<unsigned int> cVplusCbc( fNCbc );
			for( unsigned int cCbcId=0; cCbcId < fNCbc; cCbcId++ ){
				double s(0);
				double v2(0);
				for( UInt_t cTestGroupId=0; cTestGroupId<fTestGroupMap->size(); cTestGroupId++ ){
					unsigned int value = cVplus.at(cFeId).at(cCbcId).at(cTestGroupId);
					s += value;
					v2 += ( value * value ); 
				}
				double mean = s / fTestGroupMap->size();
				double err  = sqrt( v2 /fTestGroupMap->size() - mean*mean );
				cVplusCbc.at(cCbcId) = (unsigned int) ( mean + 0.5 );
				TString msg = Form( "\tFE: %2d, CBC: %2d, Vplus : %3.2f+/-%3.3f.", 
						cFeId, cCbcId, mean, err ); 
				Emit( "Message( const char * )", msg.Data() );
			}
			cVplusFinal.at(cFeId) = cVplusCbc;
		}

		return cVplusFinal;	
	}
	void Calibrator::FindVplus(){

		fScurveAnalyser->SetOffsets( fCalibSetting.find( "TargetOffset" )->second );

		for( UInt_t cTestGroup=0; cTestGroup<fTestGroupMap->size(); cTestGroup++ ){

			UInt_t cEnableTestPulse = fCalibSetting.find( "EnableTestPulse" )->second;
			if( cEnableTestPulse ){
				ActivateGroup( cTestGroup );
			}
			fTestGroupMap->Activate( cTestGroup );

			UInt_t cMinVCth(0), cMaxVCth(0xFF);
			ConfigureCbcOffset( 8, cMinVCth, cMaxVCth );

			UInt_t cVplus( fCalibSetting.find( "VplusMin." ) -> second );
			unsigned int cStep( fCalibSetting.find( "VplusStep" ) -> second );
			while( cVplus <= fCalibSetting.find( "VplusMax." ) -> second ){

				VCthScanForVplusCalibration( cVplus );
				cVplus+=cStep;
				if( fStop ) break;
			}
			if( fStop ) break;
			TString msg = Form( "\tFindVplus TestGroup = %d finished.", cTestGroup ); 
			Emit( "Message( const char * )", msg.Data() );
		}
		if( fStop ) return;
		fScurveAnalyser->SetVplus();
		PrintVplusScan();
		return;
	}

	void Calibrator::CalibrateOffsets(){

		fScurveAnalyser->SetOffsets( fCalibSetting.find( "InitialOffset" )->second );

		for( UInt_t cTestGroup=0; cTestGroup<fTestGroupMap->size(); cTestGroup++ ){

			UInt_t cEnableTestPulse = fCalibSetting.find( "EnableTestPulse" )->second;
			if( cEnableTestPulse ){
				ActivateGroup( cTestGroup );
			}
			fTestGroupMap->Activate( cTestGroup );

			for( Int_t cTargetBit = 8; cTargetBit >=-1; cTargetBit-- ){

				VCthScanForOffsetCalibration( cTargetBit ); 
				if( fStop ) break;
			}
			if( fStop ) break;
			Emit( "Message( const char * )", Form( "\tFinished offset calibration for TestGroup = %d", cTestGroup ) );
		}
		return;
	}

	void Calibrator::ConfigureVplus( std::vector< std::vector< std::vector<unsigned int> > > &pVplus, unsigned int pGroupId ){

		uint32_t cPage(0), cAddr(0x0B), cValue(0);
		for( unsigned int cFeId=0; cFeId < fNFe; cFeId++ ){
			for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){
				cValue = pVplus.at(cFeId).at(cCbcId).at(pGroupId);
				fHwController->AddCbcRegUpdateItem( cFeId, cCbcId, cPage, cAddr, cValue );
			}
		}
		UpdateCbcRegValues();
		return;
	}
	void Calibrator::ConfigureVplusScan( UInt_t pVplus ){

		uint32_t cPage(0), cAddr(0x0B), cValue(0);
		cValue = pVplus;

		for( UInt_t cFe=0; cFe < fNFe; cFe++ ){
			for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
				fHwController->AddCbcRegUpdateItem( cFe, cCbc, cPage, cAddr, cValue );
			}
		}
		UpdateCbcRegValues();

		return;
	}

	//Configuration for Vth scan
	void Calibrator::ConfigureCbcOffset( Int_t pOffsetTargetBit, UInt_t &pMinVCth, UInt_t &pMaxVCth ){

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fTestGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) throw Exception( "Call ActivateGroup() before ConfigureCbcOffset()" );

		//UInt_t cEnableTestPulse = fCalibSetting.find( "EnableTestPulse" )->second;

		uint32_t cPage(1);
		uint32_t cFe(0), cCbc(0), cChannel(0);
		uint32_t cVal(0);
		for( cFe = 0; cFe < fNFe; cFe++ ){

			for( cCbc=0; cCbc < fNCbc; cCbc++ ){

				for( uint16_t cAddr = 0x01; cAddr < 0xFF; cAddr++ ){ 

					cChannel = cAddr - 1; 	
					bool cTestChannel = false;
					if( cTestGroup->Has( cChannel ) ) cTestChannel = true;
					if( !( cTestChannel || pOffsetTargetBit == 8 ) ) continue;

					if( cTestChannel ) cVal = fScurveAnalyser->GetOffset( cFe, cCbc, cChannel ); 
					else{ 
						cVal = fNonTestGroupOffset;
					}
					fHwController->AddCbcRegUpdateItem( cFe, cCbc, cPage, cAddr, cVal );
				}
			}
		}
		UpdateCbcRegValues();

		if( pOffsetTargetBit == 8 || pOffsetTargetBit == 7 ){
			pMinVCth = 0;
			pMaxVCth = 0xFF;
		}
		else{
			uint32_t cMinVCth0 = fScurveAnalyser->GetMinVCth0(); 
			uint32_t cMaxVCth0 = fScurveAnalyser->GetMaxVCth0(); 
			int cTmp = cMinVCth0 - ( cMaxVCth0 - cMinVCth0 );
			pMinVCth = 0 < cTmp ? cTmp : 0;
			cTmp = cMaxVCth0 + ( cMaxVCth0 - cMinVCth0 );
			pMaxVCth = 0xFF > cTmp ? cTmp : 0xFF;
		}
		usleep( 10 );

#ifdef __CBCDAQ_DEV__
		std::cout << "cbc offset configuration completed." << std::endl;
#endif
		return;
	}

	void Calibrator::VCthScanForVplusCalibration( UInt_t pVplus ){


		fScurveAnalyser->Configure( ScurveAnalyser::VPLUSSEARCH );

		ConfigureVplusScan( pVplus );

		UInt_t cMinVCth(0), cMaxVCth(0xFF);
		VCthScan( cMinVCth, cMaxVCth );

		if( !fStop ){
			fScurveAnalyser->FitHists( cMinVCth, cMaxVCth );
			if( fGUIFrame ) fScurveAnalyser->DrawHists();
			fScurveAnalyser->FillGraphVplusVCth0();
			if( fGUIFrame ) DrawVplusVCthScanResult();
		}

#ifdef __CBCDAQ_DEV__
		std::cout << "VCthScanForVplusCalibration Vplus = " << pVplus << " finished." << std::endl;
#endif
		return;
	}

	void Calibrator::VCthScanForOffsetCalibration( Int_t pTargetBit ){ 

		fStop = false;

		fScurveAnalyser->Configure( ScurveAnalyser::OFFSETCALIB, pTargetBit );

		UInt_t cMinVCth(0);
		UInt_t cMaxVCth(0xFF);
		ConfigureCbcOffset( pTargetBit, cMinVCth, cMaxVCth );

#ifdef __CBCDAQ_DEV__
		std::cout << "Starting VCth = " << cMinVCth << " for target bit = " << pTargetBit << std::endl;
#endif

		VCthScan( cMinVCth, cMaxVCth );

		if( !fStop ){
			fScurveAnalyser->FitHists( cMinVCth, cMaxVCth );
			if( fGUIFrame ){
				fScurveAnalyser->DrawHists();
				fScurveAnalyser->PrintScurveHistogramDisplayPads();
			}
		}

		return;
	}

	//Vth Scan
	void Calibrator::VCthScan( UInt_t &pMinVCth, UInt_t &pMaxVCth ){

#ifdef __CBCDAQ_DEV__
		std::cout << "Starting VCth scan" << std::endl;
		struct timeval start0, start1;
		long mtime;
#endif

		UInt_t cNthAcq = 0;
		usleep( 100 );

		int cVCth = fNegativeLogicCBC ? pMinVCth : pMaxVCth;
		int cStep = fNegativeLogicCBC ? 10 : -10;
		bool cNoneZero(false);
		int cAllOneCount(0);
		while( 0x00 <= cVCth && cVCth <= 0xFF ){

#ifdef __CBCDAQ_DEV__
			gettimeofday( &start0, 0 );
			std::cout << "beginning of the loop. Vcth = " << std::dec << cVCth << std::endl;
#endif

			fHwController->AddCbcRegUpdateItemsForNewVCth( cVCth );
			UpdateCbcRegValues();

			int cNHits(0);
			for( unsigned int i=0; i< fNAcq; i++ ){
				fHwController->StartAcquisition();
				fHwController->ReadDataInSRAM( cNthAcq, true );
				fHwController->EndAcquisition( cNthAcq );

				bool cFillDataStream( true );

				int cNevent(0);	
				const Event *cEvent = fHwController->GetNextEvent();
				//std::cout << "Event = " << cEvent << std::endl;
				while( cEvent ){
					cNevent++;
					//std::cout << "EventCount = " << cEvent->GetEventCount() << std::endl; 
					//	const FeEvent *cFeEvent = cEvent->GetFeEvent( 0 );
					//	const CbcEvent *cCbcEvent = cFeEvent->GetCbcEvent( 0 );
					//	std::cout << "DATASTRING : " << cCbcEvent->DataHexString() << std::endl; 
					fAnalyser->Analyse( cEvent, cFillDataStream );	
					cFillDataStream = false;
					cNHits += fScurveAnalyser->FillHists( cVCth, cEvent );	
					cEvent = fHwController->GetNextEvent();
				}
			}
			//if( fGUIFrame )fScurveAnalyser->Analyser::DrawHists();	
			if( fGUIFrame ){
				fScurveAnalyser->Analyser::DrawHists();	
			}

			if( !(cVCth % 20) ){
#ifdef __CBCDAQ_DEV__
				gettimeofday(&start1, 0);
#endif
				if( fGUIFrame )fScurveAnalyser->DrawHists();
#ifdef __CBCDAQ_DEV__
				mtime = getTimeTook( start1, 1 );
				Emit( "Message( const char * )", Form( "\tTime took for drawing data: %ld ms.", mtime ) ); 
#endif
			}

			unsigned int cNChannels = fTestGroupMap->GetActivatedGroup()->size() * fNFe * fNCbc;
#ifdef __CBCDAQ_DEV__
			std::cout << fTestGroupMap->GetActivatedGroup()->size() << std::endl;
			std::cout << cNChannels << std::endl;
			std::cout << "Analysis for this Acquisition is done. # of hits = " << cNHits << std::endl;
			//std::cout << cNoneZero << std::endl;
#endif
			if( cNoneZero == false && cNHits != 0 ) {
				cNoneZero = true;
				cVCth -= 2 * cStep;
				cStep /= 10;
				continue;
			}
#ifdef __CBCDAQ_DEV__
			std::cout << fNAcq << std::endl;
			//std::cout << cStep << std::endl;
			//std::cout << cAllOneCount << std::endl;
			//std::cout << fNeventPerAcq << std::endl;
#endif
			if( cNHits == (int) ( fNAcq * fNeventPerAcq * cNChannels ) ) cAllOneCount++;	
			if( cAllOneCount == 10 ) break;

			cVCth += cStep;
			if( fStop ) return; 

#ifdef __CBCDAQ_DEV__
			mtime = getTimeTook( start0, 1 );
			std::cout << "Time took for one VCth: " << std::dec << mtime << " ms." << std::endl;
			std::cout << "end of the loop." << std::endl;
#endif
		}
		if( fNegativeLogicCBC ) pMaxVCth = cVCth - 1;
		else pMinVCth = cVCth + 1;

		return;
	}

	void Calibrator::PrintScurveHistogramDisplayPads(){

		fScurveAnalyser->PrintScurveHistogramDisplayPads();
	}

	void Calibrator::DrawVplusVCthScanResult(){

		fScurveAnalyser->DrawVplusVCth0();
	}

	void Calibrator::PrintVplusScan(){

		fScurveAnalyser->PrintVplusVsVCth0DisplayPads();
	}

	void Calibrator::SetCalibratedOffsets(){

		uint32_t cPage(1);
		uint32_t cFe(0), cCbc(0), cChannel(0);
		uint32_t cVal(0);
		for( cFe = 0; cFe < fNFe; cFe++ ){

			for( cCbc=0; cCbc < fNCbc; cCbc++ ){

				for( uint16_t cAddr = 0x01; cAddr < 0xFF; cAddr++ ){ 

					cChannel = cAddr - 1; 	
					cVal = fScurveAnalyser->GetOffset( cFe, cCbc, cChannel ); 
					fHwController->AddCbcRegUpdateItem( cFe, cCbc, cPage, cAddr, cVal );
				}
			}
		}
		UpdateCbcRegValues();
	}
	/*
	   void Calibrator::InitialiseDataStreamHistograms(){

	   if( fGUIFrame == 0 ) return;

	   fDataStreamHistMap.clear();

	   for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

	   for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

	   TString cHname = Form( "hDataBE%uFE%uCBC%u", fBeId, cFeId, cCbcId );
	   TObject *cObj = gROOT->FindObject( cHname ); 
	   if( cObj ) delete cObj;

	   TH1F *cHist = new TH1F( cHname, 
	   Form( "Data stream BE: %u FE:%u CBC:%u", fBeId, cFeId, cCbcId ), 
	   CbcEvent::NSENSOR, -0.5, CbcEvent::NSENSOR-0.5 ); 

	   TAxis *cAxis = cHist->GetXaxis();

//UInt_t cGroupColor[8] = { 1,2,3,4,5,6,7,8};
TestGroupMap::const_iterator cIt = fTestPulseGroupMap->begin();
for( ; cIt != fTestPulseGroupMap->end(); cIt++ ){
//UInt_t cColor = cGroupColor[cIt->first];
const TestGroup &cGroup = cIt->second;
for( UInt_t i=0; i < cGroup.size(); i++ ){
if( !( cGroup[i]%2 ) )
cAxis->SetBinLabel( cGroup[i]+1, Form("%02X [%1d]", cGroup[i], cIt->first ) );	
cAxis->SetLabelSize( 0.05 );
}
}
Int_t cId = GetDataStreamHistId( cFeId, cCbcId );
fDataStreamHistMap.insert( std::pair<Int_t, TH1F *>( cId, cHist ) );
}
}
}
	 */
}

