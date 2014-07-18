#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include "TApplication.h"
#include <TGClient.h>
#include "ICCalib/Calibrator.h"
#include "ICCalib/GUICalibratorFrame.h"
#include "CbcDaq/GUI.h"

int main( int argc, char *argv[] ){

	TString cSettingFile( "settings/CbcCalibElectron.txt" );
	if( argc >= 2 ){
		cSettingFile = argv[1];
	}

	TApplication theApp( "App", &argc, argv );

	ICCalib::Calibrator *cDaq = new ICCalib::Calibrator( cSettingFile );

	cDaq->Initialise();

	ICCalib::CalibratorFrame cCalibratorFrame( gClient->GetRoot(), gMainFrameWidth, gMainFrameHeight, "CBC Calibration", cDaq );

	cCalibratorFrame.AddFrames();

	cDaq->SetGUIFrame( &cCalibratorFrame );

	theApp.Run();

	return 0;
}	
