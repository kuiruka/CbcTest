#include "CbcDaq/GUI.h"
#include "GUICalibConfigFrame.h"
#include <TGLabel.h>
#include <TGTextEntry.h>
#include <TGButton.h>
#include <iostream>
#include "GUICalibratorFrame.h"
#include "Calibrator.h"
#include "TGNumberEntry.h"

namespace ICCalib{

	CalibrationConfigurationFrame::CalibrationConfigurationFrame( TGCompositeFrame *pFrame, CalibratorFrame *pCalibratorFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth, 100 ),
		fMotherFrame( pFrame ),
		fCalibratorFrame( pCalibratorFrame ),
		fFrame(0){

			fMotherFrame->AddFrame( this, gLHVexpand );
			RenewFrame();
		}

	void CalibrationConfigurationFrame::RenewFrame(){

		if( fFrame ){
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGCompositeFrame( this, gMainFrameWidth, 100 );
		AddFrame( fFrame, gLHVexpand );

		fCalibSettingEntries.clear();
		const CalibSetting &cCalibSetting = fCalibratorFrame->GetCalibrator()->GetCalibSetting();	
		UInt_t cNrows = cCalibSetting.size();

		UInt_t cNcols(2);
		TGTableLayout * cTableLayout = new TGTableLayout( fFrame, cNrows, cNcols, kFALSE, 5, kLHintsNormal );
		fFrame->SetLayoutManager( cTableLayout );
		UInt_t cRowId(0);
		TGLabel *cLabel;
		CalibSetting::const_iterator cIt = cCalibSetting.begin();

		for( ; cIt != cCalibSetting.end(); cIt++ ){

			cLabel = new TGLabel( fFrame, cIt->first.c_str() );
			TGNumberEntry *cE = new TGNumberEntry(
					fFrame, cIt->second, 5, ++gNumberEntryId, 
					TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 0xFF  );
			fFrame->AddFrame( cLabel,  new TGTableLayoutHints( 0, 1, cRowId, cRowId+1 ) );
			fFrame->AddFrame( cE, new TGTableLayoutHints( 1, 2, cRowId,cRowId+1 ) );
			cRowId++;
			fCalibSettingEntries.insert( CalibItemEntry(cIt->first, cE ) );
		}
		fFrame->MapSubwindows();
		fFrame->Layout();
		fMotherFrame->MapSubwindows();
		fMotherFrame->Layout();
	}

}

