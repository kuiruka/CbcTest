#include "GUIAnalyserFrame.h"
#include "GUIFrame.h"
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TGFrame.h>

namespace CbcDaq{

	const double AnalyserFrame::fTextFrameHeight = gMainFrameHeight * 0.18; 

	AnalyserFrame::AnalyserFrame( TGCompositeFrame *pFrame, GUIFrame
	*pGUIFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth, gMainFrameHeight ),
		fMotherFrame( pFrame ),
		fGUIFrame( pGUIFrame ),
		fTextFrame(0),
		fHistFrame(0)
	{ 

		fMotherFrame->AddFrame( this, gLHVexpand );
		
		fTextFrame = new AnalyserTextFrame( this, fGUIFrame );
		fHistFrame = new AnalyserHistFrame( this, fGUIFrame );

		MapSubwindows();
		Layout();

	}
	void AnalyserFrame::RenewFrame(){

		fTextFrame->RenewFrame();
		fHistFrame->RenewFrame();
		MapSubwindows();
		Layout();


	}

	AnalyserTextFrame::AnalyserTextFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ):
		TGTextView( pFrame, gMainFrameWidth, (UInt_t)( AnalyserFrame::fTextFrameHeight ) ),
		fMotherFrame( pFrame ),
		fGUIFrame( pGUIFrame ){

			fMotherFrame->AddFrame( this, gLHexpand );
			Resize( GetDefaultSize() );
			//this->ChangeOptions( this->GetOptions() | kFixedHeight );
			//this->Resize( gMainFrameWidth, 100 );
		}
	void AnalyserTextFrame::RenewFrame(){

		fGUIFrame->fDAQController->SetAnalyserTextView( this );	
	}
	AnalyserHistFrame::AnalyserHistFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth, (UInt_t)( gMainFrameHeight - AnalyserFrame::fTextFrameHeight ) ),
		fMotherFrame( pFrame ), fGUIFrame( pGUIFrame ), fFrame(0){

			fMotherFrame->AddFrame( this, gLHexpand ); 
			Resize( GetDefaultSize() );
		}
	void AnalyserHistFrame::RenewFrame(){

		if( fFrame ){
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGTab( this, gMainFrameWidth, (UInt_t)( gMainFrameHeight - AnalyserFrame::fTextFrameHeight ) );
		AddFrame( fFrame, gLHexpand );

		UInt_t cNFe = fGUIFrame->GetDAQController()->GetNFe();
		UInt_t cNCbc = fGUIFrame->GetDAQController()->GetNCbc();

		for( UInt_t cFeId = 0; cFeId < cNFe; cFeId++ ){

			TGCompositeFrame *cFeFrame  = fFrame->AddTab( Form( "FE%u", cFeId ) );
			TGTab            *cFeTabFrame = new TGTab( cFeFrame, gMainFrameWidth, (UInt_t)(gMainFrameHeight - AnalyserFrame::fTextFrameHeight ) );
			cFeFrame->AddFrame( cFeTabFrame, gLHexpand );

			for( UInt_t cCbcId=0; cCbcId < cNCbc; cCbcId++ ){

				TGCompositeFrame *cCbcFrame = cFeTabFrame->AddTab( Form( "CBC%u", cCbcId ) );
				TRootEmbeddedCanvas *cEcanvas = new TRootEmbeddedCanvas( Form( "AnalyserPadFe%uCbc%u", cFeId, cCbcId ), cCbcFrame, gMainFrameWidth, (UInt_t)(gMainFrameHeight - AnalyserFrame::fTextFrameHeight ) );
				cCbcFrame->AddFrame( cEcanvas, new TGLayoutHints( kLHintsExpandX | kLHintsExpandY, 10, 10, 10, 1 ) );
				TCanvas *cCanvas = cEcanvas->GetCanvas();
				fGUIFrame->fDAQController->SetAnalyserHistPad( cFeId, cCbcId, (TPad *) cCanvas );	
			}
		}
		MapSubwindows();
		Layout();
	}
}

