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
#include "SEUTestController.h"
#include <fstream>
#include <unistd.h>
#include <iomanip>
#include "CbcDaq/GUIFrame.h"
#include "Strasbourg/BeController.h"
#include "Strasbourg/BeEmulator.h"
#include "Strasbourg/Data.h"
#include "utils/Utilities.h"
#include "utils/Exception.h"
#include "Analysers/ErrorAnalyser.h"
#include "TCanvas.h"
#include <TThread.h>
#include <sstream>
#include <string>
#include <TObjArray.h>
#include <TObjString.h>

namespace SEUTest{

	SEUTestController::SEUTestController( const char *pConfigFile ):
		DAQController( pConfigFile ){
	}

	SEUTestController::~SEUTestController(){
	}

	void SEUTestController::initialiseSetting(){

		DAQController::initialiseSetting();
		fRunSetting.insert( RunItem( "5)TestType", Type1 ) );
		fRunSetting.insert( RunItem( "6)Type1OneCycleDurationInSec", 600 ) );
		fRunSetting.insert( RunItem( "7)Type2ExpectedDataValue", 0 ) );
	}

	void SEUTestController::ConfigureAnalyser(){

		delete fErrorAnalyser;

		fOutputDir = Form( "SEUTestBE%01uNeg%d", fBeId, fNegativeLogicCBC );
		system( Form( "mkdir -p %s", fOutputDir.c_str() ) );

		fErrorAnalyser = new ErrorAnalyser( fBeId, fNFe, fNCbc, 
				&( fHwController->GetCbcRegSetting() ),
				fNegativeLogicCBC, fOutputDir.c_str(), fGUIFrame ); 
		fErrorAnalyser->Initialise();
		fAnalyser = fErrorAnalyser;

		Emit( "Message( const char * )", Form( "ErrorAnalyser Configured." ) ); 

		return;
	}
	void SEUTestController::ConfigureRun(){

		DAQController::ConfigureRun();

		unsigned int cTPDelay         = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_FAST_RESET" );
		unsigned int cL1ADelayAfterTP = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_TEST_PULSE" );
		unsigned int cFRDelayAfterL1A = GetGlibSetting( "COMMISSIONNING_MODE_DELAY_AFTER_L1A"        );
		unsigned int cStopClockSEU    = GetGlibSetting( "COMMISSIONNING_MODE_STOP_CLOCK_SEU"         );
		unsigned int cBeamIntensity   = fRunSetting.find( "2)BeamIntensity" )->second;
		unsigned int cExpectedDataValue = fRunSetting.find( "7)Type2ExpectedDataValue" )->second;

		unsigned int cL1APointer( cTPDelay - 2 );
		unsigned int cNclockFRtoL1A = cTPDelay + cL1ADelayAfterTP; 
		unsigned int cNclockCycle   = cNclockFRtoL1A + cFRDelayAfterL1A;

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

	void SEUTestController::Run(){

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
			case Type2:
				RunType2();
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
		fRunSetting.find( "1)RunNumber" )->second = cThisRunNumber + 1;

	}

	void SEUTestController::RunType1(){

		UInt_t cNthAcq = 0;

		boost::posix_time::milliseconds cPause(100);
		unsigned int cLiveTimeUsec(0);

		time_t timer;

		struct timeval t0;
		gettimeofday( &t0, 0 );

		TString cStrLog;

		unsigned int cNRead(0);
		unsigned int cNWrongBits(0);

		unsigned int c1CycleTime = fRunSetting.find( "6)Type1OneCycleDurationInSec" )->second;
//		fHwController->CbcHardReset();
		ReConfigureCbc();
		fHwController->ReadCbcRegisters();

		while( !fStop ){

//			fHwController->CbcHardReset();
			ReConfigureCbc();
		
			gettimeofday( &t0, 0 );

			//
			//			for( unsigned int i=0; i < c1CycleTime*10; i++ ) {
			//				fHwController->CbcI2cRefresh();
			//				boost::this_thread::sleep(cPause);
			//				//sleep( 1 );
			//			}

//			fHwController->StartAcquisition();
//			for( unsigned int i=0; i < 6000; i++ ){
//				try{
//					fHwController->ReadDataInSRAM( cNthAcq, false, (unsigned int )( fErrorAnalyser->Nclock1Cycle() * 25e-6 * 2 ) );
//				}
//				catch(Exception &e){
//
//					Emit( "Message( const char * )", e.what() );
//					fLogFile << e.what() << std::endl; 
//					fErrorAnalyser->Analyse( 0, 0, e.type() );
//
//					fHwController->EndAcquisition( cNthAcq );
//					cStrLog = "Data acquisition timed out.";
//					Emit( "Message( const char * )", cStrLog.Data() );
//					fLogFile << cStrLog << std::endl; 
//
//					break;
//				}
//			}
//			fHwController->EndAcquisition( cNthAcq );
			sleep( c1CycleTime ); 
			long utime = getTimeTook( t0, 0 );
			cLiveTimeUsec += utime;

			cStrLog = Form( "\tTotalLiveTime          : %8.1f [msec]",  (double )cLiveTimeUsec / 1000 ); 
			Emit( "Message( const char * )", cStrLog.Data() );

			//write the wrong register information with wait information.
			//			std::cout << "Read CbcRegisters starts" << std::endl;
			std::vector<const CbcRegItem *> cList = fHwController->ReadCbcRegisters();
			//			std::cout << "Read CbcRegisters ends" << std::endl;
			cNRead++;
			if( fGUIFrame ){
				fGUIFrame->UpdateCbcRegFrame( cList );
			}
			if( cList.size() ){

//				fHwController->CbcHardReset();

				std::cout << "\a" << std::endl;

				if( cList.size() > 10 ){

					cNRead --;
					cStrLog = "\tCBC I2C communication failure : ";
					time(&timer);
					cStrLog += Form("%s%ju secs since the Epock ", asctime( localtime(&timer) ), (uintmax_t)timer );
					cStrLog += ctime(&timer); 
					cStrLog += Form( " : %10d th read is removed and hard reset is sent.", cNRead );
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
			fErrorAnalyser->DrawText( cStrLog );
		}
		UInt_t cLiveTime = cNRead * c1CycleTime; 
		unsigned int cBeamIntensity   = fRunSetting.find( "2)BeamIntensity" )->second;
		cStrLog = Form( "\tStopped run at : %s", ctime( &timer ) );
		cStrLog += Form( " Registers are read %10u times(%10usec), #(rate) of wrong bits = %5u(%f, %f/(sec*intensity)).", 
				cNRead, cLiveTime, cNWrongBits, (double)cNWrongBits / cLiveTime, (double) cNWrongBits / (cLiveTime * cBeamIntensity ) );
		Emit( "Message( const char * )", cStrLog.Data() );
		fLogFile << cStrLog << std::endl; 
	}
	void SEUTestController::RunType2(){

		UInt_t cNthAcq = 0;

		TString cStrLog;

		fHwController->StartAcquisition();
		try{
			fHwController->ReadDataInSRAM( cNthAcq, false, (unsigned int )( fErrorAnalyser->Nclock1Cycle() * 25e-6 * 2 ) );
		}
		catch(Exception &e){
			Emit( "Message( const char * )", e.what() );
			fLogFile << e.what() << " : Data acquisition is not successful.  Run is stoped." << std::endl; 
			std::cout << "\a" << std::endl;
			return;
		}
		fHwController->EndAcquisition( cNthAcq );

		UInt_t cInitialNTotalAcq = fHwController->GetNumberOfTotalAcq();
		UInt_t cInitialNAcqNoData = fHwController->GetNumberOfNoDataAcq();
		UInt_t cNevents = fRunSetting.find( "3)Nevents" )->second;
		UInt_t cN(0);
		usleep( 100 );

		fHwController->StartAcquisition();

		//time
		struct timeval start;
		gettimeofday( &start, 0 );

		while( !fStop ){

			struct timeval t;
			gettimeofday( &t, 0 );

			//		fHwController->StartAcquisition();
			try{
				fHwController->ReadDataInSRAM( cNthAcq, false, (unsigned int )( fErrorAnalyser->Nclock1Cycle() * 25e-6 * 2 ) );
				//fHwController->ReadDataInSRAM( cNthAcq, false, 1000 );
			}
			catch(Exception &e){
				cN++;
				Emit( "Message( const char * )", e.what() );
				fLogFile << e.what() << std::endl; 
				fErrorAnalyser->Analyse( 0, 0, e.type() );

				fHwController->EndAcquisition( cNthAcq );
				cStrLog = "Data acquisition timed out.";
				Emit( "Message( const char * )", cStrLog.Data() );
				fLogFile << cStrLog << std::endl; 
				std::cout << "\a" << std::endl;

//				fHwController->CbcHardReset();

				ReConfigureCbc();	

				fHwController->StartAcquisition();

				try{
					fHwController->ReadDataInSRAM( cNthAcq, false, (unsigned int )( fErrorAnalyser->Nclock1Cycle() * 25e-6 * 2 ) );
					//fHwController->ReadDataInSRAM( cNthAcq, false, 1000 );
				}
				catch(Exception &e){
					Emit( "Message( const char * )", e.what() );
					fLogFile << e.what() << " : Data acquisition is not successful.  Run is stoped." << std::endl; 
					std::cout << "\a" << std::endl;
					break;	
				}
				cStrLog = "Restarting Acquisition";
				Emit( "Message( const char * )", cStrLog.Data() );
				fLogFile << cStrLog << std::endl; 
				//				continue;
			}
			//		fHwController->EndAcquisition( cNthAcq );
			struct timeval t0;
			gettimeofday( &t0, 0 );
			//			float ut1 = (float) getTimeTook( t, 0 );

			float utime = (float) getTimeTook( start, 0 );
			fErrorAnalyser->SetRealTime( utime * 1e3);


			bool cFillDataStream( true );

			const Event *cEvent = fHwController->GetNextEvent();
			unsigned int cNHits(0);
			if( cEvent ){

				//std::cout << "EventCount = " << cEvent->GetEventCount() << std::endl; 
				/*
				   const FeEvent *cFeEvent = cEvent->GetFeEvent( 0 );
				   if( cFeEvent == 0 ) continue; 
				   const CbcEvent *cCbcEvent0 = cFeEvent->GetCbcEvent( 0 );
				   if( cCbcEvent0 == 0 ) continue;
				   std::cout << "DATASTRING : " << cCbcEvent0->DataHexString() << std::endl; 
				 */

				//		std::cout << "EventCount = " << cEvent->GetEventCount() << std::endl;
				//std::cout << "EventCountCBC = " << cEvent->GetEventCountCBC() << std::endl;

				cNHits += fAnalyser->Analyse( cEvent, cFillDataStream );	

				if( fErrorAnalyser->ErrorDetected() ){

					cStrLog = fErrorAnalyser->DumpEventError();
					Emit( "Message( const char * )", cStrLog.Data() );
					fLogFile << cStrLog << std::endl; 

					float cLiveTime = fErrorAnalyser->GetLiveTime() * 1e-6;
					cStrLog = Form( "\tLiveTime         : %.0f [msec]; %5u acquisitions are made.", cLiveTime, cN  ); 
					Emit( "Message( const char * )", cStrLog.Data() );
					fLogFile << cStrLog << std::endl; 

					std::cout << "\a" << std::endl;

					fHwController->EndAcquisition( cNthAcq );
//					fHwController->CbcHardReset();

					ReConfigureCbc();	

					fHwController->StartAcquisition();
					try{
						fHwController->ReadDataInSRAM( cNthAcq, false, (unsigned int )( fErrorAnalyser->Nclock1Cycle() * 25e-6 * 2 ) );
						//fHwController->ReadDataInSRAM( cNthAcq, false, 1000 );
					}
					catch(Exception &e){
						Emit( "Message( const char * )", e.what() );
						fLogFile << e.what() << " : Data acquisition is not successful.  Run is stoped." << std::endl; 
						std::cout << "\a" << std::endl;
						break;	
					}
					cStrLog = "Restarting Acquisition";
					Emit( "Message( const char * )", cStrLog.Data() );
					fLogFile << cStrLog << std::endl; 
				}

				cFillDataStream = false;	
				cN++; 
				if( cNevents != 0 && cN == cNevents ){
					fStop = true;
					break;
				}
			}
			fAnalyser->DrawHists();
			fAnalyser->DrawText();

			if( fDataFile && fErrorAnalyser->ErrorDetected() ){
				UInt_t cBufSize(0);
				const char *cDataBuffer = fHwController->GetBuffer( cBufSize ); 
				fDataFile->write( cDataBuffer, cBufSize );
			}
			//			float ut0 = (float) getTimeTook( t0, 0 );
			//			float ut  = (float) getTimeTook( t, 0 );
			//			std::cout << "ut0 / ut = " << ut0 / ut << std::endl; 
			//			std::cout << "ut = " << ut/1e6 << std::endl; 
			//			std::cout << "ut1 = " << ut1/1e6 << std::endl; 
		}
		fHwController->EndAcquisition( cNthAcq );

		long mtime = getTimeTook( start, 1 );
		long min = (mtime/1000)/60;
		long sec = (mtime/1000)%60;
		gettimeofday( &start, 0 );
		cStrLog = Form( "\tRunTime          : %ld [msec]",  mtime ); 
		Emit( "Message( const char * )", cStrLog.Data() );
		fLogFile << cStrLog << std::endl; 
		float cLiveTime = fErrorAnalyser->GetLiveTime() * 1e-6;
		cStrLog = Form( "\tLiveTime         : %.0f [msec]", cLiveTime ); 
		Emit( "Message( const char * )", cStrLog.Data() );
		fLogFile << cStrLog << std::endl; 
		float cLiveTimeRate = cLiveTime / mtime * 100;
		cStrLog = Form( "\tLiveTimeRate     : %.1f %%", cLiveTimeRate  ); 
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
		UInt_t cFinalNAcqNoData = fHwController->GetNumberOfNoDataAcq();
		UInt_t cNAcqNoData = cFinalNAcqNoData - cInitialNAcqNoData;
		msg = Form( "\tFailures of %d acquisitions are observed for this run.", cNAcqNoData );
		Emit( "Message( const char * )", msg.Data() );
		fLogFile << msg << std::endl;
		return;
	}
}

