#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include "TApplication.h"
#include <TGClient.h>
#include "SignalShape/SSController.h"
#include "SignalShape/GUISSControllerFrame.h"
#include "CbcDaq/GUI.h"

int main( int argc, char *argv[] ){

	TString cSettingFile( "settings/CbcSignalShapeElectron.txt" );
	if( argc >= 2 ){
		cSettingFile = argv[1];
	}

	TApplication theApp( "App", &argc, argv );

	SignalShape::SSController *cDaq = new SignalShape::SSController( cSettingFile );

	cDaq->Initialise();

	SignalShape::SSControllerFrame cSSControllerFrame( gClient->GetRoot(), gMainFrameWidth, gMainFrameHeight, "CBC SignalShapeInvestigator", cDaq );

	cSSControllerFrame.AddFrames();

	cDaq->SetGUIFrame( &cSSControllerFrame );

	theApp.Run();

	return 0;
}	
