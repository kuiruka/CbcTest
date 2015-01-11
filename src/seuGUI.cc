#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include "TApplication.h"
#include <TGClient.h>
#include "SEUTest/SEUTestController.h"
#include "CbcDaq/GUIFrame.h"
#include "CbcDaq/GUI.h"

using namespace CbcDaq;

int main( int argc, char *argv[] ){

	TString cSettingFile( "settings_cactus2.0.2/CbcSEUTestElectronTest1.txt" );
	if( argc > 1 ){
		cSettingFile = argv[1];
	}

	TApplication theApp( "App", &argc, argv );

	SEUTest::SEUTestController *cDaq = new SEUTest::SEUTestController( cSettingFile );

	cDaq->Initialise();

	std::cout << "Controller initialised." << std::endl;

	gMainFrameHeight = 600;
	GUIFrame cGUIFrame( gClient->GetRoot(), gMainFrameWidth, gMainFrameHeight, "CbcDaqErrorAnalysis", cDaq );

	std::cout << "GUIFrame created." << std::endl;

	cGUIFrame.AddFrames();

	std::cout << "GUIFrame::AddFrames() done." << std::endl;

	cDaq->SetGUIFrame( &cGUIFrame );

	std::cout << "DAQController::SetGUIFrame() done." << std::endl;

	theApp.Run();

	return 0;
}	
