#include "CbcDaq/GUI.h"
#include "CbcDaq/GUILogFrame.h"
#include "GUISSControllerFrame.h"
#include "GUISSConfigFrame.h"
#include "GUISSControlButtonFrame.h"
#include "SSController.h"
#include <TThread.h>
#include <TGFrame.h>
#include <iostream>
#include <TGTab.h>
#include <TGNumberEntry.h>

namespace SignalShape{

	SSControllerFrame::SSControllerFrame( const TGWindow *p, UInt_t w, UInt_t h, 
			const char *pWindowName, SSController *pSSController ): 
		GUIFrame( p, w, h, pWindowName, pSSController ),
		fThreadScan(0), 
		fSSController( pSSController ),
		fSSConfigurationFrame(0),
		fScurveHistogramFrame(0),
		fSignalShapeGraphFrame(0),
		fDataStreamFrame(0){
		}

	void SSControllerFrame::AddAnalyserFrames( TGTab *pTab ){

		TGCompositeFrame *cTabFrame = pTab->AddTab( "SS configuration" );
		fSSConfigurationFrame = new SSConfigurationFrame( cTabFrame, this );

		cTabFrame = pTab->AddTab( "Scurve Histograms" );
		fScurveHistogramFrame = new ScurveHistogramFrame( cTabFrame, this );

		cTabFrame = pTab->AddTab( "SignalShape Graphs" );
		fSignalShapeGraphFrame = new SignalShapeGraphFrame( cTabFrame, this );

		cTabFrame = pTab->AddTab( "Data" );
		fDataStreamFrame = new DataStreamFrame( cTabFrame, this );
	}
	void SSControllerFrame::AddControlButtonFrame(){
		//Controll button frame
		fControlButtonFrame = new SSControlButtonFrame( this, this );
		fControlButtonFrame->AddButtons();
	}

	void SSControllerFrame::ProcessDAQControllerMessage( const char *pMessage ){

		fLogFrame->AddText( new TGText( pMessage ) );

		if( ! strcmp( pMessage, "FinishedScan" ) ){

			fControlButtonFrame->SetButtonState( ControlButtonFrame::Stopped );
		}
	}

	void SSControllerFrame::ConfigureAnalyser(){

		//Set setting from GUI to Calibrator and configure Calibration 
		const SSSettingEntries &cSSSettingEntries = fSSConfigurationFrame->GetSSSettingEntries();
		SSSettingEntries::const_iterator cIt = cSSSettingEntries.begin();
		for( ; cIt != cSSSettingEntries.end(); cIt++ ){
			Int_t cValue = (Int_t) cIt->second->GetNumber();
			fSSController->SetSSSetting( cIt->first.c_str(), cValue ); 
		}
		fSSController->ConfigureAnalyser();

		fScurveHistogramFrame->RenewFrame();
		fSignalShapeGraphFrame->RenewFrame();
		fDataStreamFrame->RenewFrame();

		fControlButtonFrame->SetButtonState( ControlButtonFrame::AnalyserConfigured );
	}
	void SSControllerFrame::StartScan(){

		const RunSettingEntries &cRunSettingEntries = fDaqMainConfigFrame->GetRunSettingEntries();
		RunSettingEntries::const_iterator cIt = cRunSettingEntries.begin();
		for( ; cIt != cRunSettingEntries.end(); cIt++ ){
			Int_t cValue = (Int_t) cIt->second->GetNumber();
			fSSController->SetRunSetting( cIt->first.c_str(), cValue ); 
		}
//		fSSController->ConfigureRun();

		ULong_t cColor(0);
		gClient->GetColorByName( "white", cColor );
		fDaqMainConfigFrame->SetRunConfigColors( cColor ); 

		fControlButtonFrame->SetButtonState( ControlButtonFrame::Running );

		if( fThreadScan ) fThreadScan->Delete();
		fThreadScan = new TThread( "run", SSControllerFrame::StartScan, this );
		fThreadScan->Run();
	}
	void SSControllerFrame::StartScan( void *p ){

		SSControllerFrame *obj = (SSControllerFrame *) p;
		obj->fSSController->Run();

	}
}

