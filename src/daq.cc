/*
	Application to calibrate CBC2 for electron and hole modes.
	USAGE: calib ( [mode] [setting filename] )
		mode: electron or hole

		default mode is electron.
			Configuration files are expected to be as follows,
				electron : ./settings/CbcDaqElectron.txt
				hole     : ./settings/CbcDaqHole.txt
			If those files do not exist, the application still runs with the default setting found in DAQController.cc


*/
#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include "CbcDaq/DAQController.h"
#include "TApplication.h"
#include <iostream>

int main( int argc, char *argv[] ){

	std::string cConfigFileName = "settings/CbcDaqElectron.txt";

	if( argc >= 2 ){
		if( !strcmp( argv[1], "hole" ) ){

			cConfigFileName = "settings/CbcDaqHole.txt";
		}	
		if( argc == 3 ){
			cConfigFileName = std::string( argv[2] );
		}
	}
	std::string cAnalyserName = "Analyser";
	CbcDaq::DAQController cDaq( cAnalyserName.c_str(), cConfigFileName.c_str() );

	TQObject::Connect( &cDaq, "Message( const char * )", 0, 0, "PrintLog( const char * )" ); 

	cDaq.Initialise();

	cDaq.ConfigureGlib();

	cDaq.ConfigureCbc();

	cDaq.ConfigureAnalyser();

	cDaq.ConfigureRun();

	cDaq.Run();

}

