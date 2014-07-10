#include "GUISSControlButtonFrame.h"

namespace SignalShape {

	SSControlButtonFrame::SSControlButtonFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame ):
		ControlButtonFrame( pFrame, pSSControllerFrame ){
		}

	void SSControlButtonFrame::addProcessButtons(){

		fStartScanButton = new TGTextButton( this, "&StartScan" );
		//fStartScanButton->Connect( "Clicked()", "SingalShape::SSControllerFrame", fGUIFrame, "StartScan()" );
//		fStartScanButton->Connect( "Clicked()", "SignalShape::SSControllerFrame", fGUIFrame, "Stop()" );
		fStartScanButton->Connect( "Clicked()", "SignalShape::SSControllerFrame", fGUIFrame, "StartScan()" );
		AddFrame( fStartScanButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );
		fMainControlButtons.push_back( fStartScanButton );
		fProcessButtons.push_back( fStartScanButton );

	}
}


