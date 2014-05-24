#include "GUI.h"
#include "GUIConfigFrame.h"
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <iostream>
#include "GUIFrame.h"
#include "DAQController.h"

namespace CbcDaq{

	ConfigurationFileFrame::ConfigurationFileFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame, const char *pFname ):
		TGHorizontalFrame( pFrame, gMainFrameWidth, 30 ),
		fGUIFrame( pGUIFrame ){

			pFrame->AddFrame( this, gLLeftTop );

			fLabel = new TGLabel( this, " configuration filename" );
			AddFrame( fLabel, gLLeft );

			fTextEntry = new GUITextEntry( this, pFname  );
			fTextEntry->Connect( "TextChanged(const char *)", "CbcDaq::GUITextEntry", fTextEntry, "SetChangedBackgroundColor()" );
			AddFrame( fTextEntry, gLLeft );
			fTextEntry->Resize( 400, 25 );

			//LoadConfigurationFile
			fButton = new TGTextButton( this, "&Load" );
			fButton->Connect( "Clicked()", "CbcDaq::ConfigurationFileFrame", this, "Load()" );
			AddFrame( fButton, gLLeft );
		}

	void ConfigurationFileFrame::Load(){

		const char *cName = fTextEntry->GetText();	
		fGUIFrame->LoadConfiguration( cName );
		fTextEntry->SetText( fGUIFrame->GetDAQController()->GetConfigurationFileName().c_str() ); 
		ULong_t cColor(0);
		gClient->GetColorByName( "white", cColor );
		fTextEntry->SetBackgroundColor( cColor );
		gClient->NeedRedraw( fTextEntry );
	}
}

