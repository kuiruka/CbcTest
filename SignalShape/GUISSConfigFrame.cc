#include "CbcDaq/GUI.h"
#include "GUISSConfigFrame.h"
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <iostream>
#include "GUISSControllerFrame.h"
#include "SSController.h"
#include "TGNumberEntry.h"

namespace SignalShape{

	SSConfigurationFrame::SSConfigurationFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth, 100 ),
		fMotherFrame( pFrame ),
		fSSControllerFrame( pSSControllerFrame ),
		fFrame(0){

			fMotherFrame->AddFrame( this, gLHVexpand );
			RenewFrame();
		}

	void SSConfigurationFrame::RenewFrame(){

		if( fFrame ){
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGCompositeFrame( this, gMainFrameWidth, 100 );
		AddFrame( fFrame, gLHVexpand );

		fSSSettingEntries.clear();
		const SSSetting &cSSSetting = fSSControllerFrame->GetSSController()->GetSSSetting();	
		UInt_t cNrows = cSSSetting.size();

		UInt_t cNcols(2);
		TGTableLayout * cTableLayout = new TGTableLayout( fFrame, cNrows, cNcols, kFALSE, 5, kLHintsNormal );
		fFrame->SetLayoutManager( cTableLayout );
		UInt_t cRowId(0);
		TGLabel *cLabel;
		SSSetting::const_iterator cIt = cSSSetting.begin();

		for( ; cIt != cSSSetting.end(); cIt++ ){

			cLabel = new TGLabel( fFrame, cIt->first.c_str() );
			TGNumberEntry *cE = new TGNumberEntry(
					fFrame, cIt->second, 5, ++gNumberEntryId, 
					TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 0xFF  );
			fFrame->AddFrame( cLabel,  new TGTableLayoutHints( 0, 1, cRowId, cRowId+1 ) );
			fFrame->AddFrame( cE, new TGTableLayoutHints( 1, 2, cRowId,cRowId+1 ) );
			cRowId++;
			fSSSettingEntries.insert( SSItemEntry(cIt->first, cE ) );
		}
		fFrame->MapSubwindows();
		fFrame->Layout();
		fMotherFrame->MapSubwindows();
		fMotherFrame->Layout();
	}

}

