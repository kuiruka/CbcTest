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
#include "Controller.h"
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include "../CbcDaq/GUIFrame.h"
#include "Strasbourg/BeController.h"
#include "Strasbourg/BeEmulator.h"
#include "Strasbourg/Data.h"
#include "utils/Utilities.h"
#include "utils/Exception.h"
#include "Analysers/ErrorAnalyser.h"
#include "TCanvas.h"
#include <TThread.h>
#include <sstream>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>

using namespace CbcDaq;

namespace RadTest{

	Controller::Controller( const char *pConfigFile ):
		DAQController( pConfigFile )
	{
	}

	Controller::~Controller(){
	}
	void Controller::initialiseSetting(){

		DAQController::initialiseSetting();
		fRunSetting.insert( RunItem( "5)TestType", Type1 ) );
		fRunSetting.insert( RunItem( "6)CalibrationCycleInSec", 10 ) );
		fSetting.insert( SettingItem( "CalibrationSetting", "settings_cactus2.0.2/SingleCbcCalibElectron.txt" ) ); 

	}
	void Controller::ReadSettingFile( const char *pFname ){

		DAQController::ReadSettingFile( pFname );

		Configuration::iterator cIt = fConfiguration.begin();
		for( ; cIt != fConfiguration.end(); cIt++ ){

			std::string cType = cIt->first;
			std::string cValue = cIt->second;

			if( cType.find( "RadTest" ) != std::string::npos ){
				std::string cName = cType.substr( 8, std::string::npos );
				Setting::iterator cIt = fSetting.find( cName.c_str() );
				if( cIt != fSetting.end() ) cIt->second = cValue; 
			}
		}
	}
	void Controller::ConfigureAnalyser(){

		delete fAnalyser;

		fOutputDir = Form( "RadTestOutput" );
		system( Form( "mkdir -p %s", fOutputDir.c_str() ) );

		fAnalyser = new ErrorAnalyser( fBeId, fNFe, fNCbc, 
				&( fHwController->GetCbcRegSetting() ),
				fNegativeLogicCBC, fOutputDir.c_str(), fGUIFrame ); 
		fAnalyser->Initialise();

		Emit( "Message( const char * )", Form( "ErrorAnalyser Configured." ) ); 

		return;
	}
	void Controller::ConfigureRun(){

		DAQController::ConfigureRun();

		unsigned int cTPDelay         = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_FAST_RESET" );
		unsigned int cL1ADelayAfterTP = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_TEST_PULSE" );
		unsigned int cFRDelayAfterL1A = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_L1A"        );
		//unsigned int cStopClockSEU    = GetGlibSetting( "COMMISSIONNING_MODE_STOP_CLOCK_SEU"         );
		unsigned int cBeamIntensity   = fRunSetting.find( "2)BeamIntensity" )->second;
		unsigned int cExpectedDataValue = fRunSetting.find( "7)ExpectedDataValue" )->second;

		unsigned int cL1APointer( cTPDelay - 2 );
		unsigned int cNclockFRtoL1A = cTPDelay + cL1ADelayAfterTP; 
		unsigned int cNclockCycle   = cNclockFRtoL1A + cFRDelayAfterL1A;

		unsigned int cStopClockSEU(0);
		if( cStopClockSEU == 0 ){
			cL1APointer += cL1ADelayAfterTP;
		}
		else{
			cL1APointer += 3;
		}
		cL1APointer %= 256;
		( (ErrorAnalyser *)fAnalyser )->SetL1APointer   ( cL1APointer    );
		( (ErrorAnalyser *)fAnalyser )->SetNclockFRtoL1A( cNclockFRtoL1A );
		( (ErrorAnalyser *)fAnalyser )->SetNclock1Cycle ( cNclockCycle   );
		( (ErrorAnalyser *)fAnalyser )->SetBeamIntensity( cBeamIntensity );
		( (ErrorAnalyser *)fAnalyser )->SetExpectedDataValue( cExpectedDataValue );

		Emit( "Message( const char * )", "Run for SEU test Configured" );
	}

	void Controller::Run(){

		fAnalyser->Reset();

		int cTestType = fRunSetting.find( "5)TestType"        )->second;

		fStop = false;

		//message and log for RunStart
		time_t timer;
		time(&timer);
		TString cStrLog = Form( "\tRunStart       : %s", ctime( &timer ) );
		Emit( "Message( const char * )", cStrLog.Data() );
		fLogFile << cStrLog << std::endl; 
		std::cout << cTestType << std::endl;
		switch (cTestType) {

			case Type1:
				RunType1();
				break;
			default :
				RunType1();
		}
		time(&timer);
		cStrLog = Form( "\tRunEnd         : %s", ctime( &timer ) );
		Emit( "Message( const char * )", cStrLog.Data() );
		fLogFile << cStrLog; 

		cStrLog = fAnalyser->Dump();
		fLogFile << cStrLog << std::endl;;
		TObjArray *cStrings = cStrLog.Tokenize( "\n" );
		if( cStrings->GetEntriesFast()){
			TIter iString(cStrings);
			TObjString *os=0;
			while( (os = (TObjString *) iString() ) ){

				Emit( "Message( const char * )", os->GetString().Data() );
			}
		}

		if( fGUIFrame ){
			cStrLog = fGUIFrame->GetInputLogFrame()->GetText()->AsString();
			fLogFile << cStrLog << std::endl;
			Emit( "Message( const char * )", cStrLog.Data() );
		}
		fLogFile.close();
		fAnalyser->FinishRun();
		if( fDataFile )fDataFile->close();
		int cThisRunNumber = fRunSetting.find( "1)RunNumber" )->second;


	}

	void Controller::RunType1(){

		UInt_t cNthAcq = 0;

		boost::posix_time::milliseconds cPause(100);
		unsigned int cLiveTimeUsec(0);

		time_t timer;

		struct timeval t0;
		gettimeofday( &t0, 0 );

		TString cStrLog;

		unsigned int cNRead(0);
		unsigned int cNWrongBits(0);

		unsigned int c1CycleTime = fRunSetting.find( "6)CalibrationCycleInSec" )->second;
//		fHwController->CbcHardReset();
		ReConfigureCbc();
		fHwController->ReadCbcRegisters();

		while( !fStop ){

			//			fHwController->CbcHardReset();
			timeval t1;
			gettimeofday( &t1, 0 );
			ReConfigureCbc();

			bool cTakeData(true);
			while( cTakeData ){
				long utime = getTimeTook( t1, 0 );



				if( utime *1e-6 > c1CycleTime ) cTakeData = false;
			}
			std::vector<const CbcRegItem *> cList = fHwController->ReadCbcRegisters();
			cNRead++;
			if( fGUIFrame ){
				fGUIFrame->UpdateCbcRegFrame( cList );
			}
			if( cList.size() ){

				std::cout << "\a" << std::endl;

				if( cList.size() > 10 ){

					cNRead --;
					cStrLog = "\tCBC I2C communication failure : ";
					time(&timer);
					cStrLog += Form("%s%ju secs since the Epock ", asctime( localtime(&timer) ), (uintmax_t)timer );
					cStrLog += ctime(&timer); 
					cStrLog += Form( " : %10d th read is removed.", cNRead );
					Emit( "Message( const char * )", cStrLog.Data() );
					fLogFile << cStrLog << std::endl; 
				}
				else{

					time(&timer);
					cStrLog = "\tSEU detected at: ";
					cStrLog += Form("%s%ju secs since the Epock ", asctime( localtime(&timer) ), (uintmax_t)timer );
					cStrLog += ctime(&timer); 
					cStrLog += Form( " : %10d th read.", cNRead );
					Emit( "Message( const char * )", cStrLog.Data() );
					fLogFile << cStrLog << std::endl; 

					for( unsigned int i=0; i < cList.size(); i++ ){
						TString cStrLog = cList[i]->Dump();
						Emit( "Message( const char * )", cStrLog.Data() );
						fLogFile << cStrLog << std::endl; 
						cNWrongBits += cList[i]->NumberOfWrongBits();
					}
				}
			}
			UInt_t cLiveTime = cNRead * c1CycleTime; 
			unsigned int cBeamIntensity   = fRunSetting.find( "2)BeamIntensity" )->second;
			cStrLog = Form( "\nRegisters are read %10u times(%10usec), #(rate) of wrong bits = %5u(%f/sec, %f/(sec*intensity)).\n", 
					cNRead, cLiveTime, cNWrongBits, (double)cNWrongBits / cLiveTime, (double) cNWrongBits / ( cLiveTime * cBeamIntensity ) );
			( (ErrorAnalyser *)fAnalyser)->DrawText( cStrLog );

			time_t now = time(0);
			struct tm tstruct;
			tstruct = *localtime(&now);
			char buf[80];
			strftime( buf, sizeof(buf), "%y%m%d-%H%M", &tstruct );

			int cRunNumber = fRunSetting.find( "1)RunNumber" )->second;

			TString cCalibOutDir = Form( "%s/Calibration-Run%06d-%s", fOutputDir.c_str(), cRunNumber, buf ); 
			TString cCommand = Form( "calib %s %s 2>&1 | tee %s_log.txt", 
					fSetting.find( "CalibrationSetting" )->second.c_str(), 
					cCalibOutDir.Data(),
					cCalibOutDir.Data()); 

			TString cStrLog = cCommand;
			Emit( "Message( const char * )", cStrLog.Data() );

			system( cCommand.Data() );

			fLogFile.close();

			ConfigureGlib();
			ConfigureCbc();
			fRunSetting.find( "1)RunNumber" )->second = cRunNumber + 1;
			ConfigureAnalyser();
			ConfigureRun();
		}

		long utime = getTimeTook( t0, 0 );
		cStrLog = Form( "\tTotalRunTime          : %8.1f [msec]",  (double )utime / 1000 ); 
		Emit( "Message( const char * )", cStrLog.Data() );

	}
}
