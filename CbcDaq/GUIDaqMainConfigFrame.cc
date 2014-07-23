#include "GUIDaqMainConfigFrame.h"
#include "GUI.h"
#include "DAQController.h"
#include "GUIFrame.h"
#include "TGNumberEntry.h"
#include "TGLabel.h"

namespace CbcDaq{

	GlibMainConfigurationFrame::GlibMainConfigurationFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ): TGCompositeFrame( pFrame, gMainFrameWidth, gMainFrameHeight ), 
	fMotherFrame( pFrame ), fGUIFrame( pGUIFrame ), fFrame(0){

		fMotherFrame->AddFrame( this, gLHVexpand );
		//		ChangeOptions( ( GetOptions() & ~kFitWidth ) & ~kFitHeight );
		RenewFrame();
	}

	void GlibMainConfigurationFrame::RenewFrame(){

		if( fFrame ){
			fGlibSettingEntries.clear();
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}

		fFrame = new TGVerticalFrame( this, gMainFrameWidth/3, gMainFrameHeight );
		AddFrame( fFrame, gLHVexpand );
		//fFrame->Resize( fFrame->GetDefaultSize() );

		//GLIB connection setting frame 
		TGCompositeFrame *cUhalFrame = new TGCompositeFrame( fFrame, gMainFrameWidth/2, 60 );
		fFrame->AddFrame( cUhalFrame, new TGLayoutHints( kLHintsExpandX | kLHintsTop, 5, 5, 10, 5 ) ); 
		//TableLayout
		UInt_t cNrows(3), cNcols(3);
		TGTableLayout * cTableLayout = new TGTableLayout( cUhalFrame, cNrows, cNcols, kFALSE, 5, kLHintsExpandX | kLHintsTop );
		cUhalFrame->SetLayoutManager( cTableLayout );

		TGLabel *label0 = new TGLabel( cUhalFrame, "UHAL configuration file path " );
		cUhalFrame->AddFrame( label0, new TGTableLayoutHints( 0, 1, 0, 1, kLHintsLeft | kLHintsCenterY ) );
		label0->SetTextJustify( kTextLeft );

		fTextEntryUhalConfigFile = new GUITextEntry( cUhalFrame );
		cUhalFrame->AddFrame( fTextEntryUhalConfigFile, new TGTableLayoutHints( 1, 2, 0, 1, kLHintsLeft | kLHintsCenterY ) );
		fTextEntryUhalConfigFile->SetText( fGUIFrame->GetDAQController()->GetUhalConfigFileName().c_str() );
		fTextEntryUhalConfigFile->Resize( 500, 25 );
		fTextEntryUhalConfigFile->Connect( "TextChanged(const char *)", "CbcDaq::GUITextEntry", fTextEntryUhalConfigFile, "SetChangedBackgroundColor()" );

		TGLabel *label1 = new TGLabel( cUhalFrame, "Board connection id "  );
		cUhalFrame->AddFrame( label1, new TGTableLayoutHints(0,1,1,2, kLHintsLeft | kLHintsCenterY ) );
		label1->SetTextJustify( kTextLeft );

		fTextEntryBoardId = new GUITextEntry( cUhalFrame );
		fTextEntryBoardId->SetText( fGUIFrame->GetDAQController()->GetBoardId().c_str() );
		cUhalFrame->AddFrame( fTextEntryBoardId, new TGTableLayoutHints(1,2,1,2, kLHintsLeft | kLHintsCenterY ) );
		fTextEntryBoardId->Resize( 500, 25 );
		fTextEntryBoardId->Connect( "TextChanged(const char *)", "CbcDaq::GUITextEntry", fTextEntryBoardId, "SetChangedBackgroundColor()" );

		TGLabel *label2 = new TGLabel( cUhalFrame, "Board firmware type "  );
		cUhalFrame->AddFrame( label2, new TGTableLayoutHints(0,1,1,2, kLHintsLeft | kLHintsCenterY ) );
		label2->SetTextJustify( kTextLeft );

		fTextEntryBoardFirmwareType = new GUITextEntry( cUhalFrame );
		fTextEntryBoardFirmwareType->SetText( fGUIFrame->GetDAQController()->GetBoardFirmwareType().c_str() );
		cUhalFrame->AddFrame( fTextEntryBoardFirmwareType, new TGTableLayoutHints(1,2,1,2, kLHintsLeft | kLHintsCenterY ) );
		fTextEntryBoardFirmwareType->Resize( 500, 25 );
		fTextEntryBoardFirmwareType->Connect( "TextChanged(const char *)", "CbcDaq::GUITextEntry", fTextEntryBoardFirmwareType, "SetChangedBackgroundColor()" );

		//GLIB register setting frame
		TGCompositeFrame *cGlibSettingFrame = new TGCompositeFrame( fFrame, gMainFrameWidth/2, gMainFrameHeight );
		fFrame->AddFrame( cGlibSettingFrame, gLHVexpand );
		//TableLayout
		const GlibSetting &cGlibSetting = fGUIFrame->GetDAQController()->GetGlibSetting();	
		cNrows = cGlibSetting.size();
		cNcols = 2; 
		cTableLayout = new TGTableLayout( cGlibSettingFrame, cNrows, cNcols, kTRUE, 5, kLHintsExpandX | kLHintsExpandY );
		cGlibSettingFrame->SetLayoutManager( cTableLayout );

		UInt_t cRowId(0);
		GlibSetting::const_iterator cIt = cGlibSetting.find("FE_expected");
		AddGlibSettingItem( cIt->first.c_str(), cIt->second, cRowId, cGlibSettingFrame );

		cIt = cGlibSetting.find("CBC_expected");
		AddGlibSettingItem( cIt->first.c_str(), cIt->second, ++cRowId, cGlibSettingFrame );

		cIt = cGlibSetting.find( "COMMISSIONNING_MODE_DELAY_AFTER_FAST_RESET" );
		AddGlibSettingItem( cIt->first.c_str(), cIt->second, ++cRowId, cGlibSettingFrame );

		cIt = cGlibSetting.find( "COMMISSIONNING_MODE_DELAY_AFTER_TEST_PULSE" );
		AddGlibSettingItem( cIt->first.c_str(), cIt->second, ++cRowId, cGlibSettingFrame );

		cIt = cGlibSetting.find( "COMMISSIONNING_MODE_DELAY_AFTER_L1A" );
		AddGlibSettingItem( cIt->first.c_str(), cIt->second, ++cRowId, cGlibSettingFrame );

		cIt = cGlibSetting.begin();
		for( ; cIt != cGlibSetting.end(); cIt++ ){
			if( cIt->first    == "FE_expected" 
					|| cIt->first == "CBC_expected"
					|| cIt->first == "COMMISSIONNING_MODE_DELAY_AFTER_FAST_RESET"
					|| cIt->first == "COMMISSIONNING_MODE_DELAY_AFTER_TEST_PULSE" 
					|| cIt->first == "COMMISSIONNING_MODE_DELAY_AFTER_L1A"
			  ) {

				continue;
			}
			AddGlibSettingItem( cIt->first.c_str(), cIt->second, ++cRowId, cGlibSettingFrame );
		}

		fFrame->MapSubwindows();
		fFrame->Layout();
		MapSubwindows();
		Layout();
	}

	GlibMainConfigurationFrame::~GlibMainConfigurationFrame(){

	}
	void GlibMainConfigurationFrame::SetColors(ULong_t pColor ){

		fTextEntryUhalConfigFile->SetBackgroundColor( pColor );
		gClient->NeedRedraw( fTextEntryUhalConfigFile );
		fTextEntryBoardId->SetBackgroundColor( pColor );
		gClient->NeedRedraw( fTextEntryBoardId );
//		fTextEntryBoardFirmwareType->SetBackgroundColor( pColor );
//		gClient->NeedRedraw( fTextEntryBoardFirmwareType );
		GlibSettingEntries::const_iterator cIt = fGlibSettingEntries.begin();
		for( ; cIt != fGlibSettingEntries.end(); cIt++ ){
			cIt->second->GetNumberEntry()->SetBackgroundColor( pColor );
			gClient->NeedRedraw( cIt->second->GetNumberEntry() );
		}

	}

	void GlibMainConfigurationFrame::AddGlibSettingItem( const char *pName, UInt_t pValue, UInt_t pRowId, TGCompositeFrame *pFrame ){

		TGLabel *label;
		label = new TGLabel( pFrame, pName );
		pFrame->AddFrame( label,  new TGTableLayoutHints( 0, 1, pRowId, pRowId+1, kLHintsLeft | kLHintsCenterY ) );

		TGNumberEntry *cE = 
			new GUINumberEntry( 
					pFrame, pValue, 8, ++gDaqMainConfigFrameNumberEntryId, 
					TGNumberFormat::kNESInteger,
					TGNumberFormat::kNEANonNegative,
					TGNumberFormat::kNELLimitMinMax,
					0, 0xFFFFFFFF );
		cE->Connect( "ValueSet(Long_t)", "CbcDaq::GUINumberEntry", cE, "SetChangedBackgroundColor()" );
		pFrame->AddFrame( cE, new TGTableLayoutHints( 1, 2, pRowId, pRowId+1, kLHintsLeft | kLHintsCenterY ) );
		//	cE->ChangeOptions( cE->GetOptions() | kFixedWidth );
		cE->Resize( 100, 25 );

		fGlibSettingEntries.insert( GlibItemEntry( pName, cE ) );
	}

	CbcRegFileNameFrame::CbcRegFileNameFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ):
		TGCompositeFrame( pFrame, gMainFrameWidth/3, gMainFrameHeight ),
		fMotherFrame( pFrame ), fGUIFrame( pGUIFrame ), fFrame(0){

			fMotherFrame->AddFrame( this, gLHVexpand );
			//		ChangeOptions( ( GetOptions() & ~kFitWidth ) & ~kFitHeight );
		}

	void CbcRegFileNameFrame::RenewFrame(){

		if( fFrame ){
			fCbcRegFileNameEntries.clear();
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGVerticalFrame( this, gMainFrameWidth/3, gMainFrameHeight );
		AddFrame( fFrame, gLHVexpand );

		const CbcRegMap &cCbcRegMap = fGUIFrame->GetDAQController()->GetCbcRegSetting();
		CbcRegMap::const_iterator cIt0 = cCbcRegMap.begin();
		for( ; cIt0 != cCbcRegMap.end(); cIt0++ ){
			UInt_t cFeId = cIt0->first;
			const FeCbcRegMap &cFeCbcRegMap = cIt0->second;
			TGGroupFrame *cGFrame = new TGGroupFrame( fFrame, Form( "FE %d", cFeId ), kVerticalFrame ); 
			fFrame->AddFrame( cGFrame, gLHexpandTop );
			FeCbcRegMap::const_iterator cIt1 = cFeCbcRegMap.begin();
			for( ; cIt1 != cFeCbcRegMap.end(); cIt1++ ){
				TGHorizontalFrame *cHFrame = new TGHorizontalFrame( cGFrame, gMainFrameWidth/2, 30 );
				cGFrame->AddFrame( cHFrame, gLHexpandTop );

				TGLabel *cLabel = new TGLabel( cHFrame, Form( "CBC %d", cIt1->first ) );
				cHFrame->AddFrame( cLabel, gLLeft );

				TGTextEntry *cTextEntry = new GUITextEntry( cHFrame, cIt1->second.GetFileName().c_str()  );
				cHFrame->AddFrame( cTextEntry, gLHexpand );
				cTextEntry->Connect( "TextChanged(const char *)", "CbcDaq::GUITextEntry", cTextEntry, "SetChangedBackgroundColor()" );

				fCbcRegFileNameEntries.insert( CbcRegFileNameEntry( getStringId( cFeId, cIt1->first), cTextEntry ) );
			}
		}
		fFrame->MapSubwindows();
		fFrame->Layout();	
		MapSubwindows();
		Layout();
	}
	const CbcRegFileNameEntries &CbcRegFileNameFrame::GetCbcRegFileNameEntries()const{ 

		return fCbcRegFileNameEntries; 
	}

	void CbcRegFileNameFrame::SetColors( ULong_t pColor ){

		CbcRegFileNameEntries::const_iterator cIt = fCbcRegFileNameEntries.begin();
		for( ; cIt != fCbcRegFileNameEntries.end(); cIt++ ){
			cIt->second->SetBackgroundColor( pColor );
			gClient->NeedRedraw( cIt->second );
		}
	}
	void CbcRegFileNameFrame::SetCbcRegFileName( UInt_t pFeId, UInt_t pCbcId, const char *pName ){

		TString cStringId = getStringId( pFeId, pCbcId );
		CbcRegFileNameEntries::iterator cIt = fCbcRegFileNameEntries.find( cStringId );
		cIt->second->SetText( pName );
	}

	RunConfigurationFrame::RunConfigurationFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ):
		TGVerticalFrame( pFrame, gMainFrameWidth/3, gMainFrameHeight ),
		fMotherFrame( pFrame ), fGUIFrame( pGUIFrame ), fFrame(0), fInputLogGroupFrame(0), fInputLogFrame(0){

			fMotherFrame->AddFrame( this, gLHVexpand );
			RenewFrame();
		}

	void RunConfigurationFrame::RenewFrame(){

		if( fFrame ){
			fRunSettingEntries.clear();
			RemoveFrame( fFrame );
			fFrame->DestroyWindow();
			delete fFrame;
		}
		fFrame = new TGGroupFrame( this, "Run Configuration", kVerticalFrame ); 
		AddFrame( fFrame, gLHexpandTop );

		if( fInputLogGroupFrame ){
			fInputLogFrame->Clear();
		}
		else{
			fInputLogGroupFrame = new TGGroupFrame( this, "Log for this run", kVerticalFrame ); 
			AddFrame( fInputLogGroupFrame, gLHVexpand );
			fInputLogFrame = new TGTextEdit( fInputLogGroupFrame );
			fInputLogGroupFrame->AddFrame( fInputLogFrame, gLHVexpand );
		}
		TGCompositeFrame *cFrame = new TGCompositeFrame( fFrame, gMainFrameWidth/2, gMainFrameHeight );
		fFrame->AddFrame( cFrame, gLHVexpand );

		//TableLayout
		const RunSetting &cRunSetting = fGUIFrame->GetDAQController()->GetRunSetting();	
		UInt_t cNrows = cRunSetting.size();
		UInt_t cNcols = 2; 
		TGTableLayout *cTableLayout = new TGTableLayout( cFrame, cNrows, cNcols, kTRUE, 5, kLHintsExpandX | kLHintsExpandY );
		cFrame->SetLayoutManager( cTableLayout );

		UInt_t cRowId(0);
		RunSetting::const_iterator cIt = cRunSetting.begin();
		for( ; cIt != cRunSetting.end(); cIt++ ){
			AddRunSettingItem( cIt->first.c_str(), cIt->second, cRowId++, cFrame );
		}
		fFrame->MapSubwindows();
		fFrame->Layout();
		MapSubwindows();
		Layout();
	}
	void RunConfigurationFrame::SetColors( ULong_t pColor ){ 

		RunSettingEntries::const_iterator cIt = fRunSettingEntries.begin();
		for( ; cIt != fRunSettingEntries.end(); cIt++ ){
			cIt->second->GetNumberEntry()->SetBackgroundColor( pColor );
			gClient->NeedRedraw( cIt->second->GetNumberEntry() );
		}
	}
	void RunConfigurationFrame::AddRunSettingItem( const char *pName, UInt_t pValue, UInt_t pRowId, TGCompositeFrame *pFrame ){

		TGLabel *label;
		label = new TGLabel( pFrame, pName );
		pFrame->AddFrame( label,  new TGTableLayoutHints( 0, 1, pRowId, pRowId+1, kLHintsLeft | kLHintsCenterY ) );

		TGNumberEntry *cE = 
			new GUINumberEntry( 
					pFrame, pValue, 8, ++gDaqMainConfigFrameNumberEntryId, 
					TGNumberFormat::kNESInteger,
					TGNumberFormat::kNEANonNegative,
					TGNumberFormat::kNELLimitMinMax,
					0, 0xFFFFFFFF );
		cE->Connect( "ValueSet(Long_t)", "CbcDaq::GUINumberEntry", cE, "SetChangedBackgroundColor()" );
		pFrame->AddFrame( cE, new TGTableLayoutHints( 1, 2, pRowId, pRowId+1, kLHintsLeft | kLHintsCenterY ) );
		//	cE->ChangeOptions( cE->GetOptions() | kFixedWidth );
		cE->Resize( 100, 25 );

		fRunSettingEntries.insert( RunItemEntry( pName, cE ) );
	}

	DaqMainConfigurationFrame::DaqMainConfigurationFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ): TGHorizontalFrame( pFrame, gMainFrameWidth, 600 ),
	fMotherFrame( pFrame ), fGUIFrame( pGUIFrame ), fGlibMainConfigFrame(0), fCbcRegFileNameFrame(0){

		fMotherFrame->AddFrame( this, gLHVexpand );

		fGlibMainConfigFrame = new GlibMainConfigurationFrame( this, fGUIFrame );
		fCbcRegFileNameFrame = new CbcRegFileNameFrame( this, fGUIFrame );
		fRunConfigFrame      = new RunConfigurationFrame( this, fGUIFrame );

	}

	void DaqMainConfigurationFrame::RenewGlibMainConfigurationFrame(){

		fGlibMainConfigFrame->RenewFrame();
	}

	void DaqMainConfigurationFrame::RenewCbcRegFileNameFrame(){

		fCbcRegFileNameFrame->RenewFrame();
	}
	void DaqMainConfigurationFrame::RenewRunConfigurationFrame(){

		fRunConfigFrame->RenewFrame();
	}

	DaqMainConfigurationFrame::~DaqMainConfigurationFrame(){

	}
	const CbcRegFileNameEntries &DaqMainConfigurationFrame::GetCbcRegFileNameEntries()const{ 

		return fCbcRegFileNameFrame->GetCbcRegFileNameEntries(); 
	}
	void DaqMainConfigurationFrame::SetGlibConfigColors( ULong_t pColor ){ 

		fGlibMainConfigFrame->SetColors( pColor );
	}
	void DaqMainConfigurationFrame::SetCbcRegFileNameColors( ULong_t pColor ){ 

		fCbcRegFileNameFrame->SetColors( pColor );
	}
	void DaqMainConfigurationFrame::SetRunConfigColors( ULong_t pColor ){ 

		fRunConfigFrame->SetColors( pColor );
	}
}

