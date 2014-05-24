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

int main( int argc, char *argv[] ){

	std::string cConfigFileName = "settings/CbcCalibElectron.txt";

	if( argc >= 2 ){
		if( !strcmp( argv[1], "hole" ) ){

			cConfigFileName = "settings/CbcCalibHole.txt";
		}	
		if( argc == 3 ){
			cConfigFileName = std::string( argv[2] );
		}
	}

	ICCalib::Calibrator cDaq( cConfigFileName.c_str() );

	TQObject::Connect( &cDaq, "Message( const char * )", 0, 0, "PrintLog( const char * )" ); 

	cDaq.Initialise();

	cDaq.ConfigureGlib();

	cDaq.ConfigureCbc();

	cDaq.ConfigureAnalyser();

	cDaq.Calibrate();

}

