#include "GUIControlButtonFrame.h"
namespace CbcDaq{
	ControlButtonFrame::ControlButtonFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ):
		TGHorizontalFrame( pFrame, gMainFrameWidth, 80 ),
		fMotherFrame( pFrame ),
		fGUIFrame( pGUIFrame ){

			fMotherFrame->AddFrame( this, new TGLayoutHints( kLHintsCenterX, 2,2,2,2 ) );
		}
		void ControlButtonFrame::AddButtons(){
			//ConfigureGlib
			fGlibConfigurationButton = new TGTextButton( this, "&ConfigureGlib" );
			fGlibConfigurationButton->Connect( "Clicked()", "CbcDaq::GUIFrame", fGUIFrame, "ConfigureGlib()" );
			AddFrame( fGlibConfigurationButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );
			fMainControlButtons.push_back( fGlibConfigurationButton );

			//ConfigureCBC
			fCbcConfigurationButton = new TGTextButton( this, "&ConfigureCbc" );
			fCbcConfigurationButton->Connect( "Clicked()", "CbcDaq::GUIFrame", fGUIFrame, "ConfigureCbc()" );
			AddFrame( fCbcConfigurationButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );
			fMainControlButtons.push_back( fCbcConfigurationButton );

			//AnalyserCalibration
			fAnalyserConfigurationButton = new TGTextButton( this, "&ConfigureAnalyser" );
			fAnalyserConfigurationButton->Connect( "Clicked()", "CbcDaq::GUIFrame", fGUIFrame, "ConfigureAnalyser()" );
			AddFrame( fAnalyserConfigurationButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );
			fMainControlButtons.push_back( fAnalyserConfigurationButton );
			
			addProcessButtons();

			fStopButton = new TGTextButton( this, "&Stop" );
			fStopButton->Connect( "Clicked()", "CbcDaq::GUIFrame", fGUIFrame, "Stop()" );
			AddFrame( fStopButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );

			fCbcHardResetButton = new TGTextButton( this, "&CbcHardReset" ); 
			fCbcHardResetButton->Connect( "Clicked()", "CbcDaq::GUIFrame", fGUIFrame, "CbcHardReset()" );
			AddFrame( fCbcHardResetButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );

			TString cText = "Show data stream display";

			fDataStreamDisplayButton = new TGTextButton( this, cText );
			fDataStreamDisplayButton->Connect( "Clicked()", "CbcDaq::GUIFrame", fGUIFrame, "ShowDataStream()" );
			AddFrame( fDataStreamDisplayButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );

			fExitButton = new TGTextButton( this, "&Exit", "gApplication->Terminate(0)" );
			AddFrame( fExitButton, new TGLayoutHints( kLHintsCenterX, 5,5,3,4 ));

			resetMainControlButtons();
			fStopButton->SetState( kButtonDisabled );
			fCbcHardResetButton->SetState( kButtonDisabled );
			Resize( GetDefaultSize() );

			fMotherFrame->MapSubwindows();
			fMotherFrame->Layout();

		}

	void ControlButtonFrame::addProcessButtons(){

			fDaqStartButton = new TGTextButton( this, "&Run" );
			fDaqStartButton->Connect( "Clicked()", "CbcDaq::GUIFrame", fGUIFrame, "Run()" );
			AddFrame( fDaqStartButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );
			fMainControlButtons.push_back( fDaqStartButton );
			fProcessButtons.push_back( fDaqStartButton );

	}
	void ControlButtonFrame::setMainControlButtons( EButtonState pState, ULong_t pColor ){

		for( UInt_t i=0; i < fMainControlButtons.size(); i++ ){
			TGTextButton *cButton = fMainControlButtons[i];
			cButton->SetBackgroundColor( pColor ); 
			cButton->SetState( pState );
			gClient->NeedRedraw( cButton );
		}
	}

	void ControlButtonFrame::resetMainControlButtons(){

		ULong_t cColor(0); 
		gClient->GetColorByName( "gray", cColor );
		setMainControlButtons( kButtonDisabled, cColor );

		gClient->GetColorByName( "orange", cColor );
		fGlibConfigurationButton->SetBackgroundColor( cColor ); 
		fGlibConfigurationButton->SetState( kButtonUp );
		gClient->NeedRedraw( fGlibConfigurationButton );
	}

	void ControlButtonFrame::SetButtonState( State pState ){ 

		ULong_t cColor(0);

		if( pState == Stopped ){

			gClient->GetColorByName( "gray", cColor );
			setMainControlButtons( kButtonUp, cColor );

			fStopButton->SetBackgroundColor( cColor );
			fStopButton->SetState( kButtonDisabled );
			gClient->NeedRedraw( fStopButton );

			SetButtonState( CbcConfigured );

			return;
		}
		if( pState == CbcHardReset ){

			gClient->GetColorByName( "gray", cColor );
			setMainControlButtons( kButtonUp, cColor );
			SetButtonState( GlibConfigured );
			return;
		}

		if( pState >= Running ){

			gClient->GetColorByName( "gray", cColor );
			setMainControlButtons( kButtonDisabled, cColor );

			gClient->GetColorByName( "orange", cColor );
			fStopButton->SetBackgroundColor( cColor );
			fStopButton->SetState( kButtonUp );
			gClient->NeedRedraw( fStopButton );

			fCbcHardResetButton->SetState( kButtonDisabled );
			gClient->NeedRedraw( fCbcHardResetButton );

			fDataStreamDisplayButton->SetTitle( "Show data stream display" );
		}
		switch ( pState ){

			case InitialState :

				resetMainControlButtons();
				break;

			case GlibConfigured :

				gClient->GetColorByName( "orange", cColor );
				if( fGlibConfigurationButton->GetBackground() != cColor )resetMainControlButtons(); 

				gClient->GetColorByName( "gray", cColor );
				fGlibConfigurationButton->SetBackgroundColor( cColor ); 
				gClient->NeedRedraw( fGlibConfigurationButton );

				gClient->GetColorByName( "orange", cColor );
				fCbcConfigurationButton->SetBackgroundColor( cColor ); 
				fCbcConfigurationButton->SetState( kButtonUp );
				gClient->NeedRedraw( fCbcConfigurationButton );

				fCbcHardResetButton->SetState( kButtonUp );
				gClient->NeedRedraw( fCbcHardResetButton );

				fExitButton->SetState( kButtonUp );
				gClient->NeedRedraw( fExitButton );

				break;

			case CbcConfigured :

				gClient->GetColorByName( "orange", cColor );
				if( fCbcConfigurationButton->GetBackground() != cColor ){
					SetButtonState( GlibConfigured );	
				}
				gClient->GetColorByName( "gray", cColor );
				fCbcConfigurationButton->SetBackgroundColor( cColor ); 
				gClient->NeedRedraw( fCbcConfigurationButton );

				gClient->GetColorByName( "orange", cColor );
				fAnalyserConfigurationButton->SetBackgroundColor( cColor ); 
				fAnalyserConfigurationButton->SetState( kButtonUp );
				gClient->NeedRedraw( fAnalyserConfigurationButton );

				break;

			case AnalyserConfigured :

				gClient->GetColorByName( "orange", cColor );
				if( fAnalyserConfigurationButton->GetBackground() != cColor ){
					SetButtonState( CbcConfigured );	
				}
				gClient->GetColorByName( "gray", cColor );
				fAnalyserConfigurationButton->SetBackgroundColor( cColor ); 
				fAnalyserConfigurationButton->SetState( kButtonUp );
				gClient->NeedRedraw( fAnalyserConfigurationButton );

				gClient->GetColorByName( "orange", cColor );

				for( UInt_t i=0; i<fProcessButtons.size(); i++ ){
					fProcessButtons[i]->SetBackgroundColor( cColor );
					fProcessButtons[i]->SetState( kButtonUp );
					gClient->NeedRedraw( fProcessButtons[i] );
				}

				fExitButton->SetState( kButtonUp );
				gClient->NeedRedraw( fExitButton );

				break;

			case Running :

				gClient->GetColorByName( "gray", cColor );
				if( fProcessButtons.size() == 1 ){
					fProcessButtons[0]->SetBackgroundColor( cColor );
//					fProcessButtons[0]->SetState( kButtonUp );
					fProcessButtons[0]->SetState( kButtonDisabled );
					gClient->NeedRedraw( fProcessButtons[0] );
				}
				fExitButton->SetState( kButtonDisabled ); 
				gClient->NeedRedraw( fExitButton );

				break;

			default:
				break;
		}
	}

	Bool_t ControlButtonFrame::ToggleDataStreamDisplayButton(){

		TString cText = fDataStreamDisplayButton->GetTitle();
		if( cText == "Show data stream display" ){
			fDataStreamDisplayButton->SetTitle( "Stop data stream display" );
			return true;
		}
		else{
			fDataStreamDisplayButton->SetTitle( "Show data stream display" );
			return false;
		}	
		return 0;
	}
}

