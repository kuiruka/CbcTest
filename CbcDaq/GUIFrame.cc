#include "GUIFrame.h"
#include "GUI.h"
#include <TObjArray.h>
#include <TObjString.h>
#include <TThread.h>
#include <TCanvas.h>
#include <TGButton.h>
#include <TGFrame.h>
#include "DAQController.h"
#include <iostream>
#include <TStyle.h>
#include <TGTab.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGTableLayout.h>
#include <TGFileDialog.h>
#include <TPRegexp.h>
#include <TGTextView.h>
#include <TGNumberEntry.h>
#include <map>
#include <vector>
#include <TH1F.h>
#include "Strasbourg/Data.h"
#include "Strasbourg/Event.h"
#include <TRootEmbeddedCanvas.h>
#include "GUIConfigFrame.h"
#include "GUIDaqMainConfigFrame.h"
#include "GUILogFrame.h"
#include "GUICbcRegFrame.h"
#include "GUIAnalyserFrame.h"

namespace CbcDaq{

	GUIFrame::GUIFrame( const TGWindow *p, UInt_t w, UInt_t h, 
			const char *pWindowName, 
			DAQController * pDAQController): 
		TGMainFrame( p, w, h ), 
		fWindowName( pWindowName ),
		fConfigurationFileFrame(0),
		fDaqMainConfigFrame(0),
		fLogFrame(0),
		fCbcRegFrames(0),
		fAnalyserFrame(0),
		fControlButtonFrame(0),
		fDAQController(pDAQController), 
		fDaq(0) 
	{
		TQObject::Connect( "TQObject", "Message( const char * )", "CbcDaq::GUIFrame", this, "ProcessDAQControllerMessage( const char * )" ); 
		}
		void GUIFrame::AddFrames(){

		//Calibration configuration filename entry frame
		fConfigurationFileFrame = new ConfigurationFileFrame( this, this, fDAQController->GetConfigurationFileName().c_str() ); 

		//Glib, Cbc, Calibration configuration and histogram frame 
		TGTab *cTab = new TGTab( this, gMainFrameWidth, gMainFrameHeight );
		AddFrame( cTab, gLHVexpand );

		TGCompositeFrame *cTabFrame(0);

		cTabFrame = cTab->AddTab( "DAQ Main configuration" );
		fDaqMainConfigFrame = new DaqMainConfigurationFrame( cTabFrame, this );

		cTabFrame = cTab->AddTab( "Log" );
		fLogFrame = new LogFrame( cTabFrame, this );

		cTabFrame = cTab->AddTab( "CBC register" );
		fCbcRegFrames = new CbcRegFrames( cTabFrame, this );

		AddAnalyserFrames( cTab );
		AddControlButtonFrame();

		SetWindowName( fWindowName );
		MapSubwindows();
		//	Resize( GetDefaultSize() );
		Resize( gClient->GetDisplayWidth(), gClient->GetDisplayHeight() );
		MapWindow();
		Layout();

		gStyle->SetOptStat(0);
	}

	void GUIFrame::AddAnalyserFrames( TGTab *pTab ){

		TGCompositeFrame *cTabFrame = pTab->AddTab( "Analyser" );
		fAnalyserFrame = new AnalyserFrame( cTabFrame, this );
	}

	void GUIFrame::AddControlButtonFrame(){
		//Controll button frame
		fControlButtonFrame = new ControlButtonFrame( this, this );
		fControlButtonFrame ->AddButtons();
	}

	void GUIFrame::ProcessDAQControllerMessage( const char *pMessage ){

		fLogFrame->AddText( new TGText( pMessage ) );

		if( ! strcmp( pMessage, "RunEnd" ) ){

			fControlButtonFrame->SetButtonState( ControlButtonFrame::Stopped );
		}
	}


	void GUIFrame::LoadConfiguration( const char *pName ){

		gDaqMainConfigFrameNumberEntryId = 0;
		fDAQController->ReadSettingFile( pName );
		fDAQController->InitialiseGlib();
		if( fDaqMainConfigFrame ){
			fDaqMainConfigFrame->RenewGlibMainConfigurationFrame();
			fDaqMainConfigFrame->RenewRunConfigurationFrame();
		}
		if( fControlButtonFrame )fControlButtonFrame->SetButtonState(ControlButtonFrame::InitialState);
	}

	void GUIFrame::ConfigureGlib(){

		//Set setting from GUI to DAQController and configure Glib

		ULong_t cColor(0);	
		fDAQController->SetUhalConfigFileName( fDaqMainConfigFrame->GetUhalConfigFileName() ); 
		const GlibSettingEntries &cGlibSettingEntries = fDaqMainConfigFrame->GetGlibSettingEntries();
		GlibSettingEntries::const_iterator cIt = cGlibSettingEntries.begin();
		for( ; cIt != cGlibSettingEntries.end(); cIt++ ){
			Int_t cValue = (Int_t) cIt->second->GetNumber();
			fDAQController->SetGlibSetting( cIt->first.c_str(), cValue ); 
		}
		try{
			fDAQController->ConfigureGlib();
		}
		catch( std::string &e ){
			fLogFrame->AddText( new TGText( e.c_str() ) );
			gClient->GetColorByName( "red", cColor );
			fDaqMainConfigFrame->SetGlibConfigColors( cColor ); 
			return;
		}
		gClient->GetColorByName( "white", cColor );
		fDaqMainConfigFrame->SetGlibConfigColors( cColor ); 
		fDaqMainConfigFrame->RenewCbcRegFileNameFrame();
		fControlButtonFrame->SetButtonState( ControlButtonFrame::GlibConfigured );

	}
	void GUIFrame::ConfigureCbc(){

		//Set setting from GUI to DAQController and configure Cbc 
		const CbcRegFileNameEntries &cCbcRegFileNameEntries = fDaqMainConfigFrame->GetCbcRegFileNameEntries();
		CbcRegFileNameEntries::const_iterator cIt = cCbcRegFileNameEntries.begin();
		for( ; cIt != cCbcRegFileNameEntries.end(); cIt++ ){
			const char *cName = cIt->second->GetText();
			UInt_t cFe(0), cCbc(0);
			getId( cIt->first, cFe, cCbc );
			fDAQController->SetCbcRegSettingFileName( cFe, cCbc, cName ); 
		}
		ULong_t cColor(0);	

		try{
			fDAQController->ConfigureCbc();
		}
		catch( std::string &e ){
			fLogFrame->AddText( new TGText( e.c_str() ) );
			gClient->GetColorByName( "red", cColor );
			fDaqMainConfigFrame->SetCbcRegFileNameColors( cColor );
			return;
		}
		gClient->GetColorByName( "white", cColor );
		fDaqMainConfigFrame->SetCbcRegFileNameColors( cColor );
		fCbcRegFrames->RenewCbcRegFrames(); 	

		if( fCbcRegFrames->ConfigureFailed() ){
			fControlButtonFrame->SetButtonState( ControlButtonFrame::GlibConfigured );
		}
		else{
			fControlButtonFrame->SetButtonState( ControlButtonFrame::CbcConfigured );
		}

	}
	void GUIFrame::ConfigureAnalyser(){

		fDAQController->ConfigureAnalyser();

		fAnalyserFrame->RenewFrame();
		
		fControlButtonFrame->SetButtonState( ControlButtonFrame::AnalyserConfigured );

	}
	void GUIFrame::Run(){

		const RunSettingEntries &cRunSettingEntries = fDaqMainConfigFrame->GetRunSettingEntries();
		RunSettingEntries::const_iterator cIt = cRunSettingEntries.begin();
		for( ; cIt != cRunSettingEntries.end(); cIt++ ){
			Int_t cValue = (Int_t) cIt->second->GetNumber();
			fDAQController->SetRunSetting( cIt->first.c_str(), cValue ); 
		}
		fDAQController->ConfigureRun();

		ULong_t cColor(0);
		gClient->GetColorByName( "white", cColor );
		fDaqMainConfigFrame->SetRunConfigColors( cColor ); 


		fControlButtonFrame->SetButtonState( ControlButtonFrame::Running );
		fDAQController->ShowDataStream( kFALSE );

		if( fDaq ) fDaq->Delete();
		fDaq = new TThread( "Run", GUIFrame::Run, this );
		fDaq->Run();
	}
	void GUIFrame::Run( void *p ){

		GUIFrame *obj = (GUIFrame *) p;
		obj->fDAQController->Run();

	}

	void GUIFrame::Stop(){

		fDAQController->Stop();
		fControlButtonFrame->SetButtonState( ControlButtonFrame::Stopped );
	}
	void GUIFrame::CbcHardReset(){

		fDAQController->CbcHardReset();
		fControlButtonFrame->SetButtonState( ControlButtonFrame::CbcHardReset );
	}
	void GUIFrame::ShowDataStream(){

		Bool_t cStatus = fControlButtonFrame->ToggleDataStreamDisplayButton();
		fDAQController->ShowDataStream( cStatus );
	}

	//Called from DAQController
	void GUIFrame::UpdateCbcRegFrame( std::vector<const CbcRegItem *> pList ){

		fCbcRegFrames->Update( pList );
	}

	//Called from CbcRegRWFrameCommandFrame::ReConfigureCbc()
	void GUIFrame::SetCbcRegFileName( UInt_t pFeId, UInt_t pCbcId, const char *pName ){

		fDaqMainConfigFrame->SetCbcRegFileName( pFeId, pCbcId, pName );
	}
}

