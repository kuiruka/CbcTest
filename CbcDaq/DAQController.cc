#include <uhal/uhal.hpp>
#include <boost/format.hpp>
#include <boost/date_time.hpp>
#include <boost/thread.hpp>
#include <ctime>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "DAQController.h"
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include "GUIFrame.h"
#include "Strasbourg/BeController.h"
#include "Strasbourg/BeEmulator.h"
#include "Strasbourg/Data.h"
#include "utils/Utilities.h"
#include "utils/Exception.h"
#include "TCanvas.h"
#include <TThread.h>
#include <sstream>
#include <string>
#include <TObjArray.h>
#include <TObjString.h>

namespace CbcDaq{

	UInt_t DAQController::NBe=0;

	DAQController::DAQController( const char *pConfigFile ):
		fGUIFrame(0),
		fConfigurationFileName( pConfigFile ),
		fUhalConfigFileName( "/opt/testing/trackerDAQ-3.2/CBCDAQ/GlibSupervisor/xml/connections.xml" ),
		fBoardId ("board"),
		fCbcConfigFileMap(),
		fOutputDir("./"),
		fDataFile(0),
		fRunSetting(),
		fHwController(0),
		fAnalyser(0),
		fBeId(0),
		fNFe(1),
		fNCbc(2),
		fNAcq(3),
		fNeventPerAcq(100),//# of events in one data taking
		fNegativeLogicCBC(true),
		fStop(false),
		fTestPulseGroupMap(0),
		fAnalyserGroupMap(0),
		fCurrentTestPulseGroup(-1){
		NBe++;

		//TestPulseGroupMap initialisation.
		fTestPulseGroupMap = new TestGroupMap;

		UInt_t cChannel(0);
		for( int ig = 0; ig < 8; ig++ ){
			//fGroupList.push_back(ig);
			TestGroup cTestGroup;
			for( int i=0; i < 16; i++ ){
				cChannel = i * 16 + ig*2;
				if( cChannel < 254 )cTestGroup.push_back( cChannel );
				if( ++cChannel < 254 )cTestGroup.push_back( cChannel ); 
			}
			fTestPulseGroupMap->insert( std::pair< UInt_t, TestGroup >(ig, cTestGroup ) );
		}
		fAnalyserGroupMap = new TestGroupMap( *fTestPulseGroupMap );
		}

	DAQController::~DAQController(){

		delete fHwController;
		delete fAnalyser;
		delete fTestPulseGroupMap;

	}
	void DAQController::initialiseSetting(){

		//		std::cout << "I am here, DAQController::initialiseSetting()" << std::endl;
		fRunSetting.insert( RunItem( "1)RunNumber", 1 ) );
		fRunSetting.insert( RunItem( "2)BeamIntensity", 0 ) );
		fRunSetting.insert( RunItem( "3)Nevents", 0 ) );
		fRunSetting.insert( RunItem( "4)SaveData", 0 ) );
		fRunSetting.insert( RunItem( "EnableTestPulse", 0 ) );
		fRunSetting.insert( RunItem( "TestGroup", 0 ) );
		fRunSetting.insert( RunItem( "TestPulsePotentiometer", 0xF1 ) );
	}
	void DAQController::Initialise(){

		initialiseSetting();
		//Configuration file is read and set to Configuration.
		if( fConfigurationFileName != "" ){ 

			ReadSettingFile( fConfigurationFileName.c_str() ); 

		}
		//Glib initialisation
		InitialiseGlib();
		std::cout << "I am here" << std::endl;
	}
	void DAQController::InitialiseGlib(){

		delete fHwController;
		//GlibController initialisation
		if( fBoardId == "Emulator" ){
			fHwController = new BeEmulator();
		}
		else{
			fHwController = new BeController( fBoardId.c_str() ); 
		}
		//Configuration file settings are set to HwController.
		Configuration::iterator cIt = fConfiguration.begin();
		for( ; cIt != fConfiguration.end(); cIt++ ){

			std::string cType = cIt->first;
			std::string cValue = cIt->second;

			if( cType.find( "GlibReg" ) != std::string::npos ){
				std::string cName = cType.substr( 8, std::string::npos );	
				fHwController->SetGlibSetting( cName.c_str(), atoi( cValue.c_str() ) );
			}
		}
	}
	void DAQController::ConfigureGlib(){

		fStop = false;

		Emit( "Message( const char * )", Form( "\tBE                    : %u", fBeId ) );
		Emit( "Message( const char * )", Form( "\tGlibConfigurationFile : %s", fUhalConfigFileName.c_str() ) );
		Emit( "Message( const char * )", Form( "\tGlibBoardId           : %s", fBoardId.c_str() ) );


		std::string prefix( "file://" );
		try{
			fHwController->ConfigureGlibController( ( prefix + fUhalConfigFileName ).c_str() );
		}
		catch( ... ){
			throw std::string( "ERROR: HwController::ConfigureGlib() failed." );
		}
		fNFe = fHwController->GetGlibSetting( "FE_expected" );
		fNFe = fNFe == 1 ? 1 : 2;	
		UInt_t cExpectedCBC = fHwController->GetGlibSetting( "CBC_expected" );	
		fNCbc = cExpectedCBC == 1 ? 1 : 2;	
		fNegativeLogicCBC = (bool)fHwController->GetGlibSetting( "user_wb_ttc_fmc_regs.pc_commands2.negative_logic_CBC" );
		fNeventPerAcq     =       fHwController->GetGlibSetting( "user_wb_ttc_fmc_regs.pc_commands.CBC_DATA_PACKET_NUMBER" );

		TString msg;
		const GlibSetting &cGlibSetting = GetGlibSetting();	
		GlibSetting::const_iterator cItG = cGlibSetting.begin();
		for( ; cItG != cGlibSetting.end(); cItG++ ){
			msg = Form( "\tGlibReg_%-70s : %10d", cItG->first.c_str(), cItG->second );
			Emit( "Message( const char * )", msg.Data() ); 
		}

		CbcConfigFileMap::iterator cIt = fCbcConfigFileMap.begin();
		for( ; cIt != fCbcConfigFileMap.end(); cIt++ ){
			const std::string &cName = cIt->first;	
			unsigned int cFe = cName.at(2) - '0';
			unsigned int cCbc = cName.at(6) - '0';
			fHwController->SetCbcRegSettingFileName( cFe, cCbc, cIt->second );
		}
		Emit( "Message( const char * )", "GLIB Configured" );
		return;
	}

	void DAQController::ConfigureCbc(){

		CbcConfigFileMap::iterator cIt = fCbcConfigFileMap.begin();
		for( ; cIt != fCbcConfigFileMap.end(); cIt++ ){
			const std::string &cName = cIt->first;	
			unsigned int cFe = cName.at(2) - '0';
			unsigned int cCbc = cName.at(6) - '0';
			fHwController->SetCbcRegSettingFileName( cFe, cCbc, cIt->second );
		}

		try{
			fHwController->ConfigureCbc();
		}
		catch( Exception &e ){
			throw std::string( e.what() );
		}

		for( UInt_t cFe=0; cFe < fNFe; cFe++ ){
			for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
				const std::string &cFname =	fHwController->GetCbcRegSettingFileName( cFe, cCbc );
				Emit( "Message( const char * )", Form( "\tCbcConfig_FE%d_CBC_%d : %s", cFe, cCbc, cFname.c_str() ) );
			}
		}
		Emit( "Message( const char * )", "CBC Configured" );
		return;
	}

	void DAQController::ConfigureAnalyser(){

		delete fAnalyser;

		fOutputDir = Form( "BE%01uNeg%d", fBeId, fNegativeLogicCBC );
		system( Form( "mkdir -p %s", fOutputDir.c_str() ) );

		fAnalyser = new Analyser( fBeId, fNFe, fNCbc, 
				&( fHwController->GetCbcRegSetting() ),
				fNegativeLogicCBC, fOutputDir.c_str(), fGUIFrame ); 

		fAnalyser->Initialise();

		Emit( "Message( const char * )", Form( "Analyser Configured." ) ); 

		return;
	}
	void DAQController::ConfigureRun(){

		time_t timer;
		struct tm *t_st;
		time(&timer);
		t_st = localtime( &timer );

		int cRunNumber = fRunSetting.find( "1)RunNumber" )->second;
		TString cStrLogFileName = Form( "LogRun%06d-%02d%02d%02d%02d%02d.txt", cRunNumber, t_st->tm_mon+1, t_st->tm_mday, t_st->tm_hour, t_st->tm_min, t_st->tm_sec );
		fLogFile.open( (fOutputDir + "/" + cStrLogFileName.Data() ).c_str() );

		TString msg;
		const GlibSetting &cGlibSetting = GetGlibSetting();	
		GlibSetting::const_iterator cItG = cGlibSetting.begin();
		for( ; cItG != cGlibSetting.end(); cItG++ ){
			msg = Form( "\tGlibReg_%-70s : %10d", cItG->first.c_str(), cItG->second );
			fLogFile << msg << std::endl; 
		}
		for( UInt_t cFe=0; cFe < fNFe; cFe++ ){
			for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
				const std::string &cFname =	fHwController->GetCbcRegSettingFileName( cFe, cCbc );
				msg = Form( "\tCbcConfig_FE%d_CBC_%d : %s", cFe, cCbc, cFname.c_str() );
				fLogFile << msg << std::endl;
			}
		}
		RunSetting::const_iterator cItC = fRunSetting.begin();
		for( ; cItC != fRunSetting.end(); cItC++ ){
			msg = Form( "\tRun_%-30s : %10d", cItC->first.c_str(), cItC->second );
			Emit( "Message( const char * )", msg.Data() ); 
			fLogFile << msg << std::endl;
		}
		UInt_t cEnableTestPulse = fRunSetting.find( "EnableTestPulse"        )->second;
		UInt_t cTPPot           = fRunSetting.find( "TestPulsePotentiometer" )->second;
		UInt_t cGroupId         = fRunSetting.find( "TestGroup"              )->second;

		for( UInt_t cFe = 0; cFe < fNFe; cFe++ ){
			for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
				fHwController->AddCbcRegUpdateItemsForEnableTestPulse( cFe, cCbc, cEnableTestPulse );
				if( cEnableTestPulse ){
					fHwController->AddCbcRegUpdateItem( cFe, cCbc, 0, 0x0D, cTPPot ); 
					fHwController->AddCbcRegUpdateItemsForNewTestPulseGroup( cFe, cCbc, cGroupId );
				}
			}
			UpdateCbcRegValues();

			if( fAnalyserName == "ErrorAnalyser" ){

				unsigned int cTPDelay         = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_FAST_RESET" );
				unsigned int cL1ADelayAfterTP = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_TEST_PULSE" );
				unsigned int cFRDelayAfterL1A = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_L1A"        );
				unsigned int cStopClockSEU    = (0);
				if( fBoardId == "boardSEU" ){
					cStopClockSEU = GetGlibSetting( "COMMISSIONNING_MODE_STOP_CLOCK_SEU"         );
				}
				unsigned int cBeamIntensity   = fRunSetting.find( "2)BeamIntensity" )->second;

				unsigned int cL1APointer( cTPDelay - 2 );
				unsigned int cNclockFRtoL1A = cTPDelay + cL1ADelayAfterTP; 
				unsigned int cNclockCycle   = cNclockFRtoL1A + cFRDelayAfterL1A;

				Emit( "Message( const char * )", "Run Configured" );

				UInt_t cSaveData         = fRunSetting.find( "4)SaveData"              )->second;
				if( cSaveData ){

					int cRunNumber = fRunSetting.find( "1)RunNumber" )->second;
					TString cStrFileName = Form( "RawDataRun%06d-%02d%02d%02d%02d%02d.dat", cRunNumber, t_st->tm_mon+1, t_st->tm_mday, t_st->tm_hour, t_st->tm_min, t_st->tm_sec );

					delete fDataFile;
					fDataFile = new std::ofstream( (fOutputDir + "/" + cStrFileName.Data() ).c_str(), std::ofstream::binary );
					fLogFile << "\tOutputFileName : " << cStrFileName << std::endl;
				}
			}

			void DAQController::Run(){

				fAnalyser->Reset();

				fStop = false;
				UInt_t cInitialNTotalAcq = fHwController->GetNumberOfTotalAcq();
				UInt_t cNevents = fRunSetting.find( "3)Nevents" )->second;
				UInt_t cNthAcq = 0;
				UInt_t cN(0);
				usleep( 100 );

				//message and log for RunStart
				time_t timer;
				time(&timer);
				TString cStrLog = Form( "\tRunStart       : %s", ctime( &timer ) );
				Emit( "Message( const char * )", cStrLog.Data() );
				fLogFile << cStrLog; 

				//time
				struct timeval start;
				gettimeofday( &start, 0 );

				while( !fStop ){

					fHwController->StartAcquisition();
					fHwController->ReadDataInSRAM( cNthAcq, true );
					fHwController->EndAcquisition( cNthAcq );

					bool cFillDataStream( true );

					const Event *cEvent = fHwController->GetNextEvent();
					while( cEvent ){

						//std::cout << "EventCount = " << cEvent->GetEventCount() << std::endl; 
						//			std::cout << "DATASTRING : " << cCbcEvent0->DataHexString() << std::endl; 
						fAnalyser->Analyse( cEvent, cFillDataStream );	
						cEvent = fHwController->GetNextEvent();

						cFillDataStream = false;	
						cN++; 
						if( cNevents != 0 && cN == cNevents ){
							fStop = true;
							break;
						}
					}
					fAnalyser->DrawHists();
					fAnalyser->DrawText();
					if( fDataFile ){
						UInt_t cBufSize(0);
						const char *cDataBuffer = fHwController->GetBuffer( cBufSize ); 
						fDataFile->write( cDataBuffer, cBufSize );
					}
				}
				long mtime = getTimeTook( start, 1 );
				long min = (mtime/1000)/60;
				long sec = (mtime/1000)%60;
				gettimeofday( &start, 0 );
				cStrLog = Form( "\tRunTime        : %ld [msec]",  mtime ); 
				Emit( "Message( const char * )", cStrLog.Data() );
				fLogFile << cStrLog << std::endl; 

				TString msg = Form( "\tTime took for this run = %ld min %ld sec.", min, sec ); 
				Emit( "Message( const char * )", msg.Data() );
				fLogFile << msg << std::endl;
				Emit( "Message( const char * )", "RunEnd" );
				UInt_t cFinalNTotalAcq = fHwController->GetNumberOfTotalAcq();
				UInt_t cNAcq = cFinalNTotalAcq - cInitialNTotalAcq;
				msg = Form( "\tTotal %d acquisitions (%d events per acq.) are made for this run.", cNAcq, fNeventPerAcq );
				Emit( "Message( const char * )", msg.Data() );
				fLogFile << msg << std::endl;
				msg = fAnalyser->Dump();
				fLogFile << msg << std::endl;;
				TObjArray *cStrings = msg.Tokenize( "\n" );
				if( cStrings->GetEntriesFast()){
					TIter iString(cStrings);
					TObjString *os=0;
					while( (os = (TObjString *) iString() ) ){

						Emit( "Message( const char * )", os->GetString().Data() );
					}
				}

				if( fGUIFrame ){
					msg = fGUIFrame->GetInputLogFrame()->GetText()->AsString();
					fLogFile << msg << std::endl;
					Emit( "Message( const char * )", msg.Data() );
				}
				fLogFile.close();
				fAnalyser->FinishRun();
				if( fDataFile )fDataFile->close();
				int cThisRunNumber = fRunSetting.find( "1)RunNumber" )->second;
				fRunSetting.find( "1)RunNumber" )->second = cThisRunNumber + 1;
				return;
			}

			void DAQController::SetGlibSetting( const char *pName, UInt_t pValue ){

				fHwController->SetGlibSetting( pName, pValue );
			}
			void DAQController::SetCbcRegSettingFileName( UInt_t pFeId, UInt_t pCbcId, std::string pName ){ 

				//std::cout << pFeId << "," << pCbcId << "," << pName << std::endl;

				std::string cName = Form( "FE%dCBC%d", pFeId, pCbcId );
				CbcConfigFileMap::iterator cIt = fCbcConfigFileMap.find( cName ); 	
				if( cIt != fCbcConfigFileMap.end() ){
					cIt->second = pName; 
				}
				else {
					fCbcConfigFileMap.insert( CbcConfigFilePair( cName, pName ) );
				}
			}

			void DAQController::AddCbcRegUpdateItem( const CbcRegItem *pCbcRegItem, UInt_t pValue ){

				fHwController->AddCbcRegUpdateItem( pCbcRegItem->FeId(), pCbcRegItem->CbcId(), pCbcRegItem->Page(), pCbcRegItem->Address(), pValue ); 
			}

			void DAQController::UpdateCbcRegValues( bool pWrite ){

				if( fGUIFrame ){
					fGUIFrame->UpdateCbcRegFrame( fHwController->UpdateCbcRegValues( pWrite ) );
				}
				else fHwController->UpdateCbcRegValues( pWrite );
			}

			std::vector<const CbcRegItem *> DAQController::ResetCbcRegUpdateList(){

				return fHwController->ResetCbcRegUpdateList();
			}

			const GlibSetting &DAQController::GetGlibSetting()const{ return fHwController->GetGlibSetting(); }

			UInt_t DAQController::GetGlibSetting( const char * pName )const{

				return fHwController->GetGlibSetting( pName );
			}

			const CbcRegMap &DAQController::GetCbcRegSetting()const{ return fHwController->GetCbcRegSetting(); }

			void DAQController::SetAnalyserHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){	

				fAnalyser->SetHistPad( pFeId, pCbcId, pPad );
			}
			void DAQController::SetAnalyserTextView( TGTextView *pTextView ){

				fAnalyser->SetTextView( pTextView );
			}
			void DAQController::ReadSettingFile( const char *pFname ){

				fConfiguration.clear();
				Int_t cNCustomizedGroupList(0);
				fConfigurationFileName = pFname;
				std::ifstream cFile;
				cFile.open( pFname );
				if( !cFile.is_open() ){
					Emit( "Message( const char * )", Form( "The file : %s could not be opened.", pFname ) ); 
					std::cout << Form( "The file : %s could not be opened.", pFname ) << std::endl; 
					return;
				}
				Emit( "Message( const char * )", Form( "The setting file : %s.", pFname ) ); 
				std::cout << Form( "The setting file : %s.", pFname ) << std::endl;; 

				fCbcConfigFileMap.clear();
				std::string cLine;
				std::string cType;
				std::string cName;
				std::string cValue;
				size_t cFound;
				while( ! (getline( cFile, cLine ).eof()) ){  

					if( cLine.find_first_not_of( " \t" ) == std::string::npos ) continue; 
					if( cLine.at(0) == '#' || cLine.at(0) =='*' ) continue;
					cFound = cLine.find( ":" );
					if( cFound == std::string::npos ) continue;
					cType = cLine.substr( 0, cFound - 1 );
					cType.erase( remove_if( cType.begin(), cType.end(), isspace ), cType.end() );
					cValue = cLine.substr( cFound + 1, std::string::npos );
					cValue.erase( remove_if( cValue.begin(), cValue.end(), isspace ), cValue.end() );
					fConfiguration.insert( ConfigItem( cType, cValue ) );
				}
				cFile.close();

				//Configuration file settings are set to CbcConfigFileMap, and CalibSetting.
				Configuration::iterator cIt = fConfiguration.begin();
				for( ; cIt != fConfiguration.end(); cIt++ ){

					std::string cType = cIt->first;
					std::string cValue = cIt->second;

					if( cType.find( "GlibConfigurationFile" ) != std::string::npos ){
						fUhalConfigFileName = cValue; 
					}
					else if( cType.find( "GlibBoardId" ) != std::string::npos ){
						fBoardId = cValue;
					}
					else if( cType.find( "BeId" ) != std::string::npos ){
						fBeId = strtol( cValue.c_str(), 0, 10 );
					}
					else if( cType.find( "CbcConfig" ) != std::string::npos ){ 
						std::string cName = cType.substr( 10, std::string::npos );
						fCbcConfigFileMap.insert( CbcConfigFilePair( cName, cValue ) );
					}
					else if( cType.find( "Run" ) != std::string::npos ){
						std::string cName = cType.substr( 4, std::string::npos );
						RunSetting::iterator cIt = fRunSetting.find( cName.c_str() );
						if( cIt != fRunSetting.end() ){
							if( cName == "TestPulsePotentiometer" )	
								cIt->second = strtol( cValue.c_str(), 0, 16 ); 
							else
								cIt->second = strtol( cValue.c_str(), 0, 10 ); 
						}
					}
					else if( cType.find( "Analyser" ) != std::string::npos ){
						std::string cName = cType.substr( 9, std::string::npos );
						if( cName.find( "Group" ) != std::string::npos ){

							if( cNCustomizedGroupList == 0 ){
								TestGroupMap::iterator cIt = fAnalyserGroupMap->begin();
								for( ; cIt != fAnalyserGroupMap->end(); cIt++ ){
									cIt->second.clear();
								}
							}
							cNCustomizedGroupList++;

							Int_t cGroup = atoi( cName.substr( 5, std::string::npos ).c_str() );
							TestGroup cTestGroup;
							TString cTempString = cValue.c_str();
							TObjArray *cValues = cTempString.Tokenize( "," );
							if( cValues->GetEntriesFast()){
								TIter cItr(cValues);
								TObjString *cOS=0;
								while( (cOS = (TObjString *) cItr() ) ){
									cTestGroup.push_back( cOS->GetString().Atoi() );
								}
							}
							fAnalyserGroupMap->find( cGroup )->second  = cTestGroup;
						}
					}
				}
			}
			void DAQController::CbcHardReset(){

				fHwController->CbcHardReset();
			}

			void DAQController::ReConfigureCbc(){

				for( UInt_t cFe=0; cFe < fNFe; cFe++ ){
					for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
						std::string cName = Form( "FE%dCBC%d", cFe, cCbc );
						CbcConfigFileMap::iterator cIt = fCbcConfigFileMap.find( cName ); 	
						if( cIt != fCbcConfigFileMap.end() ){ 
							const std::string &cName = cIt->first;	
							unsigned int cFe = cName.at(2) - '0';
							unsigned int cCbc = cName.at(6) - '0';
							fHwController->SetCbcRegSettingFileName( cFe, cCbc, cIt->second );
						}
					}
				}
				if( fGUIFrame )fGUIFrame->UpdateCbcRegFrame( fHwController->ReConfigureCbc() );
				else fHwController->ReConfigureCbc();
				return;
			}
			void DAQController::ReConfigureCbc( UInt_t pFe, UInt_t pCbc ){

				std::string cName = Form( "FE%dCBC%d", pFe, pCbc );
				CbcConfigFileMap::iterator cIt = fCbcConfigFileMap.find( cName ); 	
				if( cIt != fCbcConfigFileMap.end() ){ 
					const std::string &cName = cIt->first;	
					unsigned int cFe = cName.at(2) - '0';
					unsigned int cCbc = cName.at(6) - '0';
					fHwController->SetCbcRegSettingFileName( cFe, cCbc, cIt->second );
				}
				if( fGUIFrame )fGUIFrame->UpdateCbcRegFrame( fHwController->ReConfigureCbc( pFe, pCbc ) );
				else fHwController->ReConfigureCbc( pFe, pCbc );

				return;
			}

			void DAQController::SaveCbcRegInfo( UInt_t pFe, UInt_t pCbc ){ 

				fHwController->SaveCbcRegInfo( pFe, pCbc );

				const std::string &cFname =	fHwController->GetCbcRegSettingFileName( pFe, pCbc );
				Emit( "Message( const char * )", Form( "\tFE %d CBC %d configuration is written to file : %s", pFe, pCbc, cFname.c_str() ) );
			}

			void DAQController::SaveCbcRegInfo( const char *pNewDir ){ 

				if( pNewDir == 0 ){
					fHwController->SaveCbcRegInfo( fOutputDir.c_str() ); 
				}
				else{
					fHwController->SaveCbcRegInfo( pNewDir ); 
				}

				for( UInt_t cFe=0; cFe < fNFe; cFe++ ){
					for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
						const std::string &cFname =	fHwController->GetCbcRegSettingFileName( cFe, cCbc );
						Emit( "Message( const char * )", Form( "\tFE %d CBC %d configuration is written to file : %s", cFe, cCbc, cFname.c_str() ) );
					}
				}
				Emit( "Message( const char * )", "Current CBC register values are saved." );
			}
			void DAQController::ReadCbcRegisters(){

				std::vector<const CbcRegItem *> cList = fHwController->ReadCbcRegisters();
				if( fGUIFrame ){
					fGUIFrame->UpdateCbcRegFrame( cList );
				}
			}

			void DAQController::ActivateGroup( UInt_t pGroupId ){

				fTestPulseGroupMap->Activate( pGroupId );

				if( fCurrentTestPulseGroup != (Int_t)pGroupId ){
					for( UInt_t cFe = 0; cFe < fNFe; cFe++ ){
						for( UInt_t cCbc=0; cCbc < fNCbc; cCbc++ ){
							fHwController->AddCbcRegUpdateItemsForNewTestPulseGroup( cFe, cCbc, pGroupId );
						}
					}
					UpdateCbcRegValues();
					fCurrentTestPulseGroup = pGroupId;
					Emit( "Message( const char *)", Form( "Activated TestPulseGroup = %d", pGroupId ) );
				}
#ifdef __CBCDAQ_DEV__
				std::cout << "Activated group is " << pGroupId << std::endl; 
#endif
			}
		}

