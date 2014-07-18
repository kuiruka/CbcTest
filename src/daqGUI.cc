#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include "TApplication.h"
#include <TGClient.h>
#include "CbcDaq/DAQController.h"
#include "CbcDaq/GUIFrame.h"
#include "CbcDaq/GUI.h"

using namespace CbcDaq;

int main( int argc, char *argv[] ){

	TString cSettingFile( "settings/CbcCalibElectron.txt" );
	if( argc > 1 ){
		cSettingFile = argv[1];
	}

	TApplication theApp( "App", &argc, argv );

	std::string cAnalyserName = "Analyser";
	DAQController *cDaq = new DAQController( cAnalyserName.c_str(), cSettingFile );

	cDaq->Initialise();

	GUIFrame cGUIFrame( gClient->GetRoot(), gMainFrameWidth, gMainFrameHeight, "CbcDaqAnalysis", cDaq );

	cGUIFrame.AddFrames();

	cDaq->SetGUIFrame( &cGUIFrame );

	theApp.Run();

	return 0;
}	
