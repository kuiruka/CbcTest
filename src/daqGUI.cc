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

	TApplication theApp( "App", &argc, argv );

//	std::string cAnalyserName = "Analyser";
	std::string cAnalyserName = "ErrorAnalyser";
	DAQController *cDaq = new DAQController( cAnalyserName.c_str(), "settings/SEUElectron.txt" );

	cDaq->Initialise();

	GUIFrame cGUIFrame( gClient->GetRoot(), gMainFrameWidth, gMainFrameHeight, "CbcDaqErrorAnalysis", cDaq );

	cGUIFrame.AddFrames();

	cDaq->SetGUIFrame( &cGUIFrame );

	theApp.Run();

	return 0;
}	
