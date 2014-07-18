#include "SSController.h"
#include "Strasbourg/BeController.h"
#include "Strasbourg/BeEmulator.h"
#include "Strasbourg/Data.h"
#include "Analysers/SignalShapeAnalyser.h"
#include "utils/Exception.h"
#include "utils/Utilities.h"
#include <uhal/uhal.hpp>

namespace SignalShape{

	SSController::SSController( const char *pConfigFile ):
		DAQController( "SignalShapeAnalyser", pConfigFile ),
		fSSSetting(),
		fNonTestGroupOffset(0xFF){
		}
	SSController::~SSController(){

		delete fTestGroupMap;
		delete fSignalShapeAnalyser;
	}
	void SSController::initialiseSetting(){

		fSSSetting.insert( SSItem( "TestPulsePotentiometer", 0xF3 ) );
		fSSSetting.insert( SSItem( "Ipre1", 0xF0 ) );
	}
	//main functions
	void SSController::ConfigureAnalyser(){

		fNonTestGroupOffset = 0xFF;
		if( !fNegativeLogicCBC ) fNonTestGroupOffset = 0x00;

		TString msg;
		SSSetting::const_iterator cItC = fSSSetting.begin();
		for( ; cItC != fSSSetting.end(); cItC++ ){
			msg = Form( "\tSS_%-30s : 0x%02X", cItC->first.c_str(), cItC->second );
			Emit( "Message( const char * )", msg.Data() ); 
		}
		UInt_t cEnableTestPulse(1);
		UInt_t cTPPot           = fSSSetting.find( "TestPulsePotentiometer" )->second;
		UInt_t cIpre1           = fSSSetting.find( "Ipre1"                  )->second;
		for( UInt_t cFe = 0; cFe < fNFe; cFe++ ){
			for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
				fHwController->AddCbcRegUpdateItemsForEnableTestPulse( cFe, cCbc, cEnableTestPulse );
				fHwController->AddCbcRegUpdateItem( cFe, cCbc, 0, 0x0D, cTPPot ); 
				fHwController->AddCbcRegUpdateItem( cFe, cCbc, 0, 0x03, cIpre1 ); 
			}
		}
		UpdateCbcRegValues();

		std::string cSignalType = Form( "TPPot%2XIpre1%2X", cTPPot, cIpre1 );
		fOutputDir = Form( "SignalShapeBE%01uNeg%d", fBeId, fNegativeLogicCBC );
		system( Form( "mkdir -p %s", fOutputDir.c_str() ) );

		delete fTestGroupMap;
		fTestGroupMap   = new SignalShapeTestGroupMap( *fAnalyserGroupMap );

		delete fSignalShapeAnalyser;
		fSignalShapeAnalyser = new SignalShapeAnalyser( fBeId, fNFe, fNCbc, 
				fTestGroupMap, &(fHwController->GetCbcRegSetting()),
				fNegativeLogicCBC, cSignalType.c_str(), fOutputDir.c_str(), fGUIFrame );
//		std::cout << "Initialise() begin" << std::endl;
		fSignalShapeAnalyser->Initialise();
//		std::cout << "Initialise() end" << std::endl;
		fAnalyser = fSignalShapeAnalyser;
		Emit( "Message( const char * )", "Calibration Configured." ); 
	}
	void SSController::Run(){

		std::cout << "I am here" << std::endl;
		fSignalShapeAnalyser->SetOffsets();
		const char *cTriggerDelayString = "COMMISSIONNING_MODE_DELAY_AFTER_TEST_PULSE"; 
		UInt_t cBaseDelay = fHwController->GetGlibSetting( cTriggerDelayString );
		Double_t cCurrentTime(0);

		for( Int_t cTriggerDelay=-1; cTriggerDelay < 14; cTriggerDelay++ ){

			//		for( Int_t cPulseDelay=24; cPulseDelay>=0; cPulseDelay-- ){
			for( Int_t cPulseDelay=24; cPulseDelay>=0; cPulseDelay-=5 ){

				uint32_t cVal = (uint32_t) (cBaseDelay + cTriggerDelay);
				std::cout << "Val = " << cVal << std::endl;

				uhal::HwInterface *cBoard = ( (BeController *)fHwController)->GetBoard();
				cBoard->getNode( cTriggerDelayString ).write( cVal );
				cBoard->dispatch();

				cCurrentTime = 25 - ( cPulseDelay + 1 ) + cTriggerDelay * 25;

				std::cout << "CurrentTime = " << cCurrentTime << std::endl;

				uint32_t cFe(0), cCbc(0);
				for( cFe = 0; cFe < fNFe; cFe++ ){

					for( cCbc=0; cCbc < fNCbc; cCbc++ ){

						fHwController->AddCbcRegUpdateItemsForNewTestPulseDelay( cFe, cCbc, cPulseDelay );
					}
				}
				UpdateCbcRegValues();

				TestGroupMap::iterator cIt = fTestPulseGroupMap->begin();


				for( ; cIt != fTestPulseGroupMap->end(); cIt++ ){

					UInt_t cTestGroup = cIt->first;
					if( cTestGroup != 0 ) continue;
					ActivateGroup( cTestGroup );
					fTestGroupMap->Activate( cTestGroup );

					UInt_t cMinVCth(0), cMaxVCth(0xFF);
					ConfigureCbcOffset( cMinVCth, cMaxVCth );

					fTestGroupMap->Activate( cTestGroup );

					fSignalShapeAnalyser->Configure( cCurrentTime );


					VCthScan( cMinVCth, cMaxVCth );

					if( fStop ) return;

					fSignalShapeAnalyser->FitHists( cMinVCth, cMaxVCth );

					if( fGUIFrame ){
						fSignalShapeAnalyser->DrawHists();
						fSignalShapeAnalyser->DrawGraphs();
						//fSignalShapeAnalyser->PrintScurveHistogramDisplayPads();
					}

					Emit( "Message( const char * )", Form( "\tVCthScan for TestGroup = %d finished.", cTestGroup ) );

				}
			}
		}
		InitialiseOffsets();
		fSignalShapeAnalyser->SaveSignalShape();
		Emit( "Message( const char * )", "FinishedScan");
		return;
		}


		void SSController::VCthScan( UInt_t &pMinVCth, UInt_t &pMaxVCth ){

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

				fHwController->StartAcquisition();
				fHwController->ReadDataInSRAM( cNthAcq, true );
				fHwController->EndAcquisition( cNthAcq );

				bool cFillDataStream( true );

				int cNHits(0);
				int cNevent(0);	
				const Event *cEvent = fHwController->GetNextEvent();
				while( cEvent ){
					cNevent++;
					//std::cout << "EventCount = " << cEvent->GetEventCount() << std::endl; 
					//			std::cout << "DATASTRING : " << cCbcEvent0->DataHexString() << std::endl; 
					fAnalyser->Analyse( cEvent, cFillDataStream );	
					cFillDataStream = false;
					cNHits += fSignalShapeAnalyser->FillHists( cVCth, cEvent );	
					cEvent = fHwController->GetNextEvent();
				}
				if( fGUIFrame ){
					fSignalShapeAnalyser->Analyser::DrawHists();	
				}

				if( !(cVCth % 20) ){
#ifdef __CBCDAQ_DEV__
					gettimeofday(&start1, 0);
#endif
					if( fGUIFrame )fSignalShapeAnalyser->DrawHists();
#ifdef __CBCDAQ_DEV__
					mtime = getTimeTook( start1, 1 );
					Emit( "Message( const char * )", Form( "\tTime took for drawing data: %ld ms.", mtime ) ); 
#endif
				}

				unsigned int cNChannels = fTestGroupMap->GetActivatedGroup()->size() * fNFe * fNCbc;
#ifdef __CBCDAQ_DEV__
				std::cout << "SetData is done. # of hits = " << cNHits << std::endl;
				//std::cout << cNChannels << std::endl;
				//std::cout << cNoneZero << std::endl;
#endif
				if( cNoneZero == false && cNHits != 0 ) {
					cNoneZero = true;
					cVCth -= 2 * cStep;
					cStep /= 10;
					continue;
				}
#ifdef __CBCDAQ_DEV__
				//std::cout << cStep << std::endl;
				//std::cout << cAllOneCount << std::endl;
				//std::cout << fNeventPerAcq << std::endl;
#endif
				if( cNHits == (int) ( fNeventPerAcq * cNChannels ) ) cAllOneCount++;	
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
		void SSController::SetSignalShapeGraphDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){

			if( fSignalShapeAnalyser ){
				fSignalShapeAnalyser->SetSignalShapeGraphDisplayPad( pFeId, pCbcId, pPad );
			}
		}
		void SSController::SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){

			if( fSignalShapeAnalyser ){
				fSignalShapeAnalyser->SetScurveHistogramDisplayPad( pFeId, pCbcId, pPad );
			}
		}

		void SSController::ConfigureCbcOffset( UInt_t &pMinVCth, UInt_t &pMaxVCth ){

			UInt_t cGroupId(0);
			SignalShapeTestGroup *cTestGroup = fTestGroupMap->GetActivatedGroup(cGroupId);
			//TestGroup *cTestGroup = fTestPulseGroupMap->GetActivatedGroup(cGroupId);
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

						if( cTestChannel ){
							//						cVal = fHwController->GetCbcRegSetting().GetValue0( cFe, cCbc, 1, cChannel+1 );
							cVal = fSignalShapeAnalyser->GetOffset( cFe, cCbc, cChannel ); 

						}
						else{ 
							cVal = fNonTestGroupOffset;
							//			std::cout << "Channel=" << cChannel << " Offset=" << cVal << std::endl;
						}
						//				std::cout << "OffsetCheck : CBC = " << cCbc << "[" << cChannel << "] " << cVal << std::endl;  
						fHwController->AddCbcRegUpdateItem( cFe, cCbc, cPage, cAddr, cVal );
					}
				}
			}
			UpdateCbcRegValues();
			/*
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
			 */
#ifdef __CBCDAQ_DEV__
			std::cout << "cbc offset configuration completed." << std::endl;
#endif
			return;
		}
		void SSController::InitialiseOffsets(){

			uint32_t cPage(1);
			uint32_t cFe(0), cCbc(0), cChannel(0);
			uint32_t cVal(0);
			for( cFe = 0; cFe < fNFe; cFe++ ){

				for( cCbc=0; cCbc < fNCbc; cCbc++ ){

					for( uint16_t cAddr = 0x01; cAddr < 0xFF; cAddr++ ){ 

						cChannel = cAddr - 1; 	
						cVal = fHwController->GetCbcRegSetting().GetValue0( cFe, cCbc, 1, cChannel+1 );
						fHwController->AddCbcRegUpdateItem( cFe, cCbc, cPage, cAddr, cVal );
					}
				}
			}
			UpdateCbcRegValues();
		}
		void SSController::ReadSettingFile( const char *pFname ){

			DAQController::ReadSettingFile( pFname );

			//Configuration file settings are set to CbcConfigFileMap, and CalibSetting.
			Configuration::iterator cIt = fConfiguration.begin();
			for( ; cIt != fConfiguration.end(); cIt++ ){

				std::string cType = cIt->first;
				std::string cValue = cIt->second;

				if( cType.find( "SS" ) != std::string::npos ){
					std::string cName = cType.substr( 3, std::string::npos );
					SSSetting::iterator cIt = fSSSetting.find( cName.c_str() );
					if( cIt != fSSSetting.end() ) cIt->second = strtol( cValue.c_str(), 0, 16 ); 
				}
			}
		}
	}

