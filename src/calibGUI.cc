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

	TApplication theApp( "App", &argc, argv );

	ICCalib::Calibrator *cDaq = new ICCalib::Calibrator( "settings/CbcCalibElectron.txt" );

	cDaq->Initialise();

	ICCalib::CalibratorFrame cCalibratorFrame( gClient->GetRoot(), gMainFrameWidth, gMainFrameHeight, "CBC Calibration", cDaq );

	cCalibratorFrame.AddFrames();

	cDaq->SetGUIFrame( &cCalibratorFrame );

	theApp.Run();

	return 0;
}	
