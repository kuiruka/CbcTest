#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include "TApplication.h"
#include <TGClient.h>
#include "RadTest/Controller.h"
#include "CbcDaq/GUIFrame.h"
#include "CbcDaq/GUI.h"

using namespace CbcDaq;

int main( int argc, char *argv[] ){

	TString cSettingFile( "settings/SingleCbcElectronRadTest.txt" );
	if( argc > 1 ){
		cSettingFile = argv[1];
	}

	TApplication theApp( "App", &argc, argv );

	RadTest::Controller *cDaq = new RadTest::Controller( cSettingFile );

	cDaq->Initialise();

	GUIFrame cGUIFrame( gClient->GetRoot(), gMainFrameWidth, gMainFrameHeight, "CbcDaqErrorAnalysis", cDaq );

	cGUIFrame.AddFrames();

	cDaq->SetGUIFrame( &cGUIFrame );

	theApp.Run();

	return 0;
}	
