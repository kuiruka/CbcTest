#include "GUIHistoFrame.h"
#include "CbcDaq/GUI.h"
#include "CbcDaq/DAQController.h"
#include "SignalShape/GUISSControllerFrame.h"
#include "SignalShape/SSController.h"

#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TGFrame.h>
#include <TGTab.h>

namespace SignalShape{

	SignalShapeGraphFrame::SignalShapeGraphFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth, gMainFrameHeight ),
		fMotherFrame( pFrame ),
		fSSControllerFrame( pSSControllerFrame ){

			fMotherFrame->AddFrame( this, gLHVexpand ); 
		}

	void SignalShapeGraphFrame::RenewFrame(){

		if( fFrame ){
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGTab( this, gMainFrameWidth, gMainFrameHeight );
		AddFrame( fFrame, gLHVexpand );

		UInt_t cNFe = fSSControllerFrame->GetDAQController()->GetNFe();
		UInt_t cNCbc = fSSControllerFrame->GetDAQController()->GetNCbc();

		for( UInt_t cFeId = 0; cFeId < cNFe; cFeId++ ){

		   TGCompositeFrame *cFeFrame = fFrame->AddTab( Form( "FE%u", cFeId ) );
		   TGTab *cFeTabFrame = new TGTab( cFeFrame, gMainFrameWidth, gMainFrameHeight );
		   cFeFrame->AddFrame( cFeTabFrame, gLHVexpand );

		   for( UInt_t cCbcId = 0; cCbcId < cNCbc; cCbcId++ ){

			   TGCompositeFrame *cCbcFrame = cFeTabFrame->AddTab( Form( "CBC%u", cCbcId ) );
			   TRootEmbeddedCanvas *cEcanvas = new TRootEmbeddedCanvas( Form( "SignalShapeEcanvasFe%uCbc%u", cFeId, cCbcId ), cCbcFrame, gMainFrameWidth, 600 );
			   cCbcFrame->AddFrame( cEcanvas, new TGLayoutHints( kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1 ) );
			   TCanvas *cCanvas = cEcanvas->GetCanvas();
			   fSSControllerFrame->GetSSController()->SetSignalShapeGraphDisplayPad( cFeId, cCbcId, cCanvas );
		   }
		}
		MapSubwindows();
		Layout();
	}

	ScurveHistogramFrame::ScurveHistogramFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth, gMainFrameHeight ),
		fMotherFrame( pFrame ), 
		fSSControllerFrame( pSSControllerFrame ){

			fMotherFrame->AddFrame( this, gLHVexpand ); 
		}

	void ScurveHistogramFrame::RenewFrame(){

		if( fFrame ){
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGTab( this, gMainFrameWidth, gMainFrameHeight );
		AddFrame( fFrame, gLHVexpand );
		
		UInt_t cNFe = fSSControllerFrame->GetDAQController()->GetNFe();
		UInt_t cNCbc = fSSControllerFrame->GetDAQController()->GetNCbc();

		for( UInt_t cFeId = 0; cFeId < cNFe; cFeId++ ){

		   TGCompositeFrame *cFeFrame = fFrame->AddTab( Form( "FE%u", cFeId ) );
		   TGTab *cFeTabFrame = new TGTab( cFeFrame, gMainFrameWidth, gMainFrameHeight );
		   cFeFrame->AddFrame( cFeTabFrame, gLHVexpand );

		   for( UInt_t cCbcId = 0; cCbcId < cNCbc; cCbcId++ ){

			   TGCompositeFrame *cCbcFrame = cFeTabFrame->AddTab( Form( "CBC%u", cCbcId ) );
			   TRootEmbeddedCanvas *cEcanvas = new TRootEmbeddedCanvas( Form( "ScurveEcanvasFe%uCbc%u", cFeId, cCbcId ), cCbcFrame, gMainFrameWidth, 600 );
			   cCbcFrame->AddFrame( cEcanvas, new TGLayoutHints( kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1 ) );
			   TCanvas *cCanvas = cEcanvas->GetCanvas();
			   fSSControllerFrame->GetSSController()->SetScurveHistogramDisplayPad( cFeId, cCbcId, cCanvas );

		   }
		}
		MapSubwindows();
		Layout();
	}

	DataStreamFrame::DataStreamFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth, gMainFrameHeight ),
		fMotherFrame( pFrame ), fGUIFrame( pGUIFrame ){

			fMotherFrame->AddFrame( this, gLHVexpand ); 
		}
	void DataStreamFrame::RenewFrame(){

		if( fFrame ){
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGTab( this, gMainFrameWidth, gMainFrameHeight );
		AddFrame( fFrame, gLHVexpand );

		UInt_t cNFe = fGUIFrame->GetDAQController()->GetNFe();
		UInt_t cNCbc = fGUIFrame->GetDAQController()->GetNCbc();

		for( UInt_t cFeId = 0; cFeId < cNFe; cFeId++ ){

			TGCompositeFrame *cFeFrame = fFrame->AddTab( Form( "FE%u", cFeId ) );

			TRootEmbeddedCanvas *cEcanvas = new TRootEmbeddedCanvas( Form( "DataStreamFe%u", cFeId ), cFeFrame, gMainFrameWidth, 600 );
			cFeFrame->AddFrame( cEcanvas, new TGLayoutHints( kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1 ) );
			TCanvas *cCanvas = cEcanvas->GetCanvas();
			cCanvas->Divide( 1,  cNCbc );
			for( UInt_t cCbcId=0; cCbcId < cNCbc; cCbcId++ ){
				fGUIFrame->fDAQController->SetAnalyserHistPad( cFeId, cCbcId, (TPad *) (cCanvas->GetPad( cCbcId+1 ) ) );	
			}
		}
		MapSubwindows();
		Layout();
	}
}


