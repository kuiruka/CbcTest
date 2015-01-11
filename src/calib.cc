/*
	Application to calibrate CBC2 for electron and hole modes.
	USAGE: calib ( [mode] [setting filename] )
		mode: electron or hole

		default mode is electron.
			Configuration files are expected to be as follows,
				electron : ./settings/CbcCalibElectron.txt
				hole     : ./settings/CbcCalibHole.txt
			If those files do not exist, the application still runs with the default setting found in DAQController.cc


*/
#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include "ICCalib/Calibrator.h"
#include "TApplication.h"
#include <iostream>
#include "CbcDaq/GUI.h"
#include <TString.h>

const unsigned int NPOT = 5;
unsigned int gTPPot[NPOT] = { 0xF3, 0xE7, 0xDB, 0xCF, 0xC3 };

int main( int argc, char *argv[] ){

	std::string cConfigFileName = "settings/CbcCalibElectron.txt";
	std::string cOutputDir = "";

	if( argc >= 2 ){
		if( !strcmp( argv[1], "hole" ) ){

			cConfigFileName = "settings/CbcCalibHole.txt";
		}	
		else{
			cConfigFileName = std::string( argv[1] );
		}
	}
	if( argc >= 3 ){
		cOutputDir = std::string( argv[2] );
	}

	ICCalib::Calibrator cDaq( cConfigFileName.c_str() );
	cDaq.SetOutputDir( cOutputDir.c_str() );

	TQObject::Connect( &cDaq, "Message( const char * )", 0, 0, "PrintLog( const char * )" ); 

	cDaq.Initialise();

	cDaq.ConfigureGlib();

/*
	TString cCbcRegSettingFile = Form( "%s/FE0CBC0.txt", cOutputDir.c_str() );	
	std::string cFile = cCbcRegSettingFile.Data();
	cDaq.SetCbcRegSettingFileName( 0, 0, cFile ); 
*/
	cDaq.ConfigureCbc();

	cDaq.ConfigureAnalyser();

	cDaq.Calibrate();

	cDaq.SetCalibSetting( "EnableTestPulse", 1 );
	for( unsigned int i=0; i < NPOT; i++ ){

		cDaq.SetCalibSetting( "TestPulsePotentiometer", gTPPot[i] ); 

		cDaq.ConfigureAnalyser();
		cDaq.VCthScanForAllGroups();
	}
}

