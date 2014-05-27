#include "GUICbcRegFrame.h"
#include <TGNumberEntry.h>
#include "GUIFrame.h"
#include "GUI.h"
namespace CbcDaq {

	TGLayoutHints *CbcRegWidget::LayoutThis(){ return new TGLayoutHints( kLHintsLeft | kLHintsCenterY, 1, 1, 1, 1 ); } 	
	TGLayoutHints *CbcRegWidget::LayoutName(){ return new TGLayoutHints( kLHintsLeft | kLHintsCenterY, 1, 1, 1, 1 ); } 	
	TGLayoutHints *CbcRegWidget::LayoutAddress(){ return new TGLayoutHints( kLHintsLeft | kLHintsCenterY, 1, 1, 1, 1 ); } 	
	TGLayoutHints *CbcRegWidget::LayoutValue(){ return new TGLayoutHints( kLHintsLeft | kLHintsCenterY, 1, 1, 1, 1 ); } 	
	TGLayoutHints *CbcRegWidget::LayoutNumberEntry(){ return new TGLayoutHints( kLHintsLeft | kLHintsCenterY, 1, 1, 1, 1 ); } 	

	TGLayoutHints *CbcRegWidget::fLayout = CbcRegWidget::LayoutThis();
	TGLayoutHints *CbcRegWidget::fLayoutName = CbcRegWidget::LayoutName();
	TGLayoutHints *CbcRegWidget::fLayoutAddress = CbcRegWidget::LayoutAddress();
	TGLayoutHints *CbcRegWidget::fLayoutValue = CbcRegWidget::LayoutValue();
	TGLayoutHints *CbcRegWidget::fLayoutNumberEntry = CbcRegWidget::LayoutNumberEntry();

	CbcRegWidget::CbcRegWidget( TGCompositeFrame *pFrame, const CbcRegItem *pCbcRegItem, GUIFrame *pGUIFrame, CbcRegFrames *pCbcRegFrames ):
		TGHorizontalFrame( pFrame, 370, 20 ),
		fCbcRegItem( pCbcRegItem ),
		fGUIFrame( pGUIFrame ),
		fCbcRegFrames( pCbcRegFrames ){

			UInt_t cPage = pCbcRegItem->Page();
			UInt_t cWidth( 370 );
			if( cPage == 1 ) cWidth = 105; 
			pFrame->AddFrame( this, fLayout ); 

			if( cPage == 0 ){
				fLabelName = new TGLabel( this, pCbcRegItem->Name().c_str() );
				AddFrame( fLabelName, fLayoutName );
				fLabelName->ChangeOptions( fLabelName->GetOptions() | kFixedWidth );
				fLabelName->Resize( 250, 20 );
			}
			fLabelAddress = new TGLabel( this, Form( "[%02X]", pCbcRegItem->Address() ) );
			AddFrame( fLabelAddress, fLayoutAddress ); 
			fLabelAddress->ChangeOptions( fLabelAddress->GetOptions() | kFixedWidth );
			fLabelAddress->Resize( 25, 20 );

			fLabelValue = new TGLabel( this, Form( "%02X", pCbcRegItem->Value() ) );
			AddFrame( fLabelValue, fLayoutValue ); 
			fLabelValue->ChangeOptions( fLabelValue->GetOptions() | kFixedWidth );
			fLabelValue->Resize( 22, 20 );

			fNumberEntry = new TGNumberEntry( this, pCbcRegItem->Value0(), 2, ++gNumberEntryId, 
					TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative, TGNumberFormat::kNELLimitMinMax, 0, 0xFF  );
			fNumberEntry->Connect( "ValueSet(Long_t)", "CbcDaq::CbcRegWidget", this, "AddUpdateItem()" );
			Bool_t cWriteFailed = fCbcRegItem->WriteFailed();
			if( cWriteFailed ) {
				ULong_t color(0);	
				gClient->GetColorByName( "red", color );
				fNumberEntry->GetNumberEntry()->SetBackgroundColor(color);
				gClient->NeedRedraw( fNumberEntry->GetNumberEntry() );
				gClient->NeedRedraw( fNumberEntry );
			}

			AddFrame( fNumberEntry, fLayoutNumberEntry ); 
			fNumberEntry->ChangeOptions( fNumberEntry->GetOptions() | kFixedWidth );
			fNumberEntry->Resize( 38, 20 );

			MapSubwindows();
			MapWindow();
			Layout();
		}

	void CbcRegWidget::AddUpdateItem(){

		fCbcRegFrames->AddCbcRegUpdateItem( fCbcRegItem, (UInt_t)fNumberEntry->GetNumber() );

		ULong_t color(0);	
		gClient->GetColorByName( "orange", color );
		fNumberEntry->GetNumberEntry()->SetBackgroundColor( color );
		fNumberEntry->SetForegroundColor( color );
		gClient->NeedRedraw( fNumberEntry );
		gClient->NeedRedraw( fNumberEntry->GetNumberEntry() );
	}

	void CbcRegWidget::Update(){

		ULong_t color(0);	
		Bool_t cWriteFailed = fCbcRegItem->WriteFailed();
		if( cWriteFailed ){
			gClient->GetColorByName( "red", color );
		}
		else{
			gClient->GetColorByName( "white", color );
		}

		fNumberEntry->SetNumber( fCbcRegItem->Value0() );
		fNumberEntry->GetNumberEntry()->SetBackgroundColor(color);
		gClient->NeedRedraw( fNumberEntry->GetNumberEntry() );
		gClient->NeedRedraw( fNumberEntry );

		fLabelValue->SetText( Form( "%02X", fCbcRegItem->Value() ) );
		gClient->NeedRedraw( fLabelValue );
		//		Emit( "Message( const char *)", Form( " FE: %d, CBC: %d, Page: %d, Addr: %02X, Value: %02X is set.", 
		//					fCbcRegItem->FeId(), fCbcRegItem->CbcId(), fCbcRegItem->Page(), fCbcRegItem->Address(), fCbcRegItem->Value() ) );
	}

	CbcRegRWFileCommandFrame::CbcRegRWFileCommandFrame( TGCompositeFrame *pFrame, const CbcRegList &pList, GUIFrame *pGUIFrame ):
		TGHorizontalFrame( pFrame, gMainFrameWidth, 20 ){

			pFrame->AddFrame( this, gLCenterX ); 

			fFeId = pList.FeId();
			fCbcId = pList.CbcId();
			fGUIFrame = pGUIFrame;

			fLabel = new TGLabel( this, "File name :" );
			AddFrame( fLabel, gLNormal );

			fTextEntry =  new TGTextEntry( this, pList.GetFileName().c_str()  );
			AddFrame( fTextEntry, gLNormal );
			fTextEntry->Resize( 400 );

			fButtonLoadAndSet = new TGTextButton( this, "&LoadAndSet" );
			fButtonLoadAndSet->Connect( "Clicked()", "CbcDaq::CbcRegRWFileCommandFrame", this, "ReConfigureCbc()" );
			AddFrame( fButtonLoadAndSet, gLNormal );

			fButtonSave = new TGTextButton( this, "&Save" );
			fButtonSave->Connect( "Clicked()", "CbcDaq::CbcRegRWFileCommandFrame", this, "Save()" );
			AddFrame( fButtonSave, gLNormal );

			pFrame->MapSubwindows();
			pFrame->Layout();
		}

	void CbcRegRWFileCommandFrame::ReConfigureCbc(){

		const char *cName = fTextEntry->GetText();	

		fGUIFrame->SetCbcRegFileName( fFeId, fCbcId, cName );	

		DAQController *pController = fGUIFrame->GetDAQController();
		pController->SetCbcRegSettingFileName( fFeId, fCbcId, cName ); 
		pController->ReConfigureCbc( fFeId, fCbcId );
	}

	void CbcRegRWFileCommandFrame::Save(){

		const char *cName = fTextEntry->GetText();	
		DAQController *pController = fGUIFrame->GetDAQController();
		pController->SetCbcRegSettingFileName( fFeId, fCbcId, cName ); 
		pController->SaveCbcRegInfo( fFeId, fCbcId );
	}

	CbcRegFrame::CbcRegFrame( const CbcRegList &pCRL, TGCompositeFrame *pFrame, GUIFrame *pGUIFrame, CbcRegFrames *pCbcRegFrames ): 
		TGTab( pFrame, gMainFrameWidth, gMainFrameHeight ),
		fMotherFrame( pFrame ),
		fGUIFrame( pGUIFrame ),
		fCbcRegFrames( pCbcRegFrames ){

			fMotherFrame->AddFrame( this, gLHVexpand );

			for( UInt_t cPage=0; cPage < 2; cPage++ ){

				TGCompositeFrame *cPageFrame = AddTab( Form( "Page%u", cPage+1 ) );
				TGCompositeFrame *cTableFrame = new TGCompositeFrame( cPageFrame, gMainFrameWidth, 500 );
				cPageFrame->AddFrame( cTableFrame , new TGLayoutHints( kLHintsExpandX | kLHintsExpandY ) );

				UInt_t cNrows(0), cNcols(0);
				if( cPage == 0 ){ cNrows = 16; cNcols = 4; }
				else{ cNrows = 16; cNcols = 16; }

				TGTableLayout *cTableLayout = new TGTableLayout( cTableFrame, cNrows, cNcols, kFALSE, 5, kLHintsExpandX | kLHintsExpandY );
				cTableFrame->SetLayoutManager( cTableLayout );

				UInt_t cPCount(0);
				for( UInt_t i=0; i<pCRL.size(); i++ ){
					UInt_t cItemPage = pCRL[i]->Page();	
					if( cItemPage != cPage ) continue; 
					UInt_t cWidth( 370 );
					if( cPage == 1 ) cWidth = 100; 
					TGCompositeFrame *cItemFrame = new TGCompositeFrame( cTableFrame, cWidth, 20 ); 
					TGTableLayoutHints *cLHints = new TGTableLayoutHints( cPCount/cNrows, cPCount/cNrows + 1, cPCount%cNrows, cPCount%cNrows+1, kLHintsCenterX | kLHintsCenterY ); 
					cTableFrame->AddFrame( cItemFrame, cLHints );
					CbcRegWidget *cCbcRegWidget = new CbcRegWidget( cItemFrame, pCRL[i], fGUIFrame, fCbcRegFrames ); 
					fCbcRegFrames->fCbcRegGUIMap.insert( CbcRegGUIPair( pCRL[i], cCbcRegWidget ) );
					cPCount++;
				}
				cTableFrame->MapSubwindows();
				cTableFrame->Layout();
			}
		}

	CbcRegFrame::~CbcRegFrame(){

	}

	CbcRegFrames::CbcRegFrames( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ): 
		TGCompositeFrame( pFrame, gMainFrameWidth, gMainFrameHeight ),
		fMotherFrame( pFrame ),
		fGUIFrame( pGUIFrame ),
		fTabFrame(0),
		fCommandFrame(0),
		fCbcRegUpdateButton(0),
		fCbcRegUpdateResetButton(0){

			fMotherFrame->AddFrame( this, gLHexpandTop );

			fCommandFrame = new TGHorizontalFrame( this, gMainFrameWidth, 20 );
			AddFrame( fCommandFrame, gLCenterXTop );

			fCbcRegUpdateButton = new TGTextButton( fCommandFrame, "&UpdateCbcRegisters" ); 
			fCbcRegUpdateButton->Connect( "Clicked()", "CbcDaq::CbcRegFrames", this, "UpdateCbcRegisters()" );
			fCommandFrame->AddFrame( fCbcRegUpdateButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );

			fCbcRegUpdateResetButton = new TGTextButton( fCommandFrame, "&Reset" ); 
			fCbcRegUpdateResetButton->Connect( "Clicked()", "CbcDaq::CbcRegFrames", this, "ResetCbcRegUpdateList()" );
			fCommandFrame->AddFrame( fCbcRegUpdateResetButton, new TGLayoutHints( kLHintsCenterX, 5, 5, 3, 4 ) );

		}

	void CbcRegFrames::RenewCbcRegFrames(){

		if( fTabFrame ){
			RemoveFrame( fTabFrame );
			fTabFrame->DestroyWindow();
			delete fTabFrame;
		}
		fTabFrame = new TGTab( this, gMainFrameWidth, gMainFrameHeight );
		AddFrame( fTabFrame, gLHVexpand );
		fCbcRegGUIMap.clear();

		const CbcRegMap &cCbcRegSetting = fGUIFrame->GetDAQController()->GetCbcRegSetting();
		CbcRegMap::const_iterator cItCRS = cCbcRegSetting.begin();

		for( ; cItCRS != cCbcRegSetting.end(); cItCRS++ ){

			UInt_t cFE = cItCRS->first;

			TGCompositeFrame *cFeFrame = fTabFrame->AddTab( Form( "FE%u", cFE ) );
			TGTab *cFeTabFrame = new TGTab( cFeFrame, gMainFrameWidth, gMainFrameHeight );
			cFeFrame->AddFrame( cFeTabFrame, gLHVexpand );

			const FeCbcRegMap &cFeMap = cItCRS->second;	
			FeCbcRegMap::const_iterator cItFeMap = cFeMap.begin();
			for( ; cItFeMap != cFeMap.end(); cItFeMap++ ){

				UInt_t cCBC = cItFeMap->first;
				const CbcRegList &cCRL = cItFeMap->second;

				TGCompositeFrame *cCbcFrame = cFeTabFrame->AddTab( Form( "CBC%u", cCBC ) );
				TGVerticalFrame *cCbcVFrame = new TGVerticalFrame( cCbcFrame, gMainFrameWidth, gMainFrameHeight );
				cCbcFrame->AddFrame( cCbcVFrame, gLHVexpand );


				new CbcRegRWFileCommandFrame( cCbcVFrame, cCRL, fGUIFrame );
				new CbcRegFrame( cCRL, cCbcVFrame, fGUIFrame, this ); 

				//			cCbcFrame->MapSubwindows();
				//			cCbcFrame->Layout();
			}
			//		cFeFrame->MapSubwindows();
			//		cFeFrame->Layout();
		}
		//	fTabFrame->MapSubwindows();
		//	fTabFrame->Layout();
		MapSubwindows();
		Layout();
		fMotherFrame->MapSubwindows();
		fMotherFrame->Layout();
	}

	void CbcRegFrames::AddCbcRegUpdateItem( const CbcRegItem *pCbcRegItem, UInt_t cValue ){

		CbcRegUpdateList::iterator cIt = fCbcRegUpdateList.find( pCbcRegItem ); 
		if( cIt == fCbcRegUpdateList.end() ) fCbcRegUpdateList.insert( CbcRegUpdateItem( pCbcRegItem, cValue ) ); 
		else cIt->second = cValue;
	}

	void CbcRegFrames::Update( std::vector<const CbcRegItem *> pList ){

		for( UInt_t i=0; i<pList.size(); i++ ){
			CbcRegGUIMap::const_iterator cIt = fCbcRegGUIMap.find( pList.at(i) );
			if( cIt == fCbcRegGUIMap.end() ){
				Emit( "Message( const char * )", Form( "Error Invalid CbcRegItem %u %u %u %02X\n", pList.at(i)->FeId(), pList.at(i)->CbcId(), pList.at(i)->Page(), pList.at(i)->Address() ) );
				continue;
			}
			CbcRegWidget *cWidget = cIt->second;	
			cWidget->Update();
		}
	}

	void CbcRegFrames::UpdateCbcRegisters(){

		CbcRegUpdateList::iterator cIt = fCbcRegUpdateList.begin(); 
		for( ; cIt != fCbcRegUpdateList.end(); cIt++ ){

			const CbcRegItem *cCbcRegItem = cIt->first;
			UInt_t cValue = cIt->second;
			fGUIFrame->GetDAQController()->AddCbcRegUpdateItem( cCbcRegItem, cValue );
			Emit( "Message( const char *)", Form( " FE: %d, CBC: %d, Page: %d, Addr: %02X, Value: %02X is sent.", 
						cCbcRegItem->FeId(), cCbcRegItem->CbcId(), cCbcRegItem->Page(), cCbcRegItem->Address(), cValue ) );
		}
		fGUIFrame->GetDAQController()->UpdateCbcRegValues();
		fCbcRegUpdateList.clear();
	}

	void CbcRegFrames::ResetCbcRegUpdateList(){

		std::vector<const CbcRegItem *> cList;
		CbcRegUpdateList::iterator cIt = fCbcRegUpdateList.begin(); 
		for( ; cIt != fCbcRegUpdateList.end(); cIt++ ){
			cList.push_back( cIt->first );
		}
		fCbcRegUpdateList.clear();
		Update( cList ); 
	}
}

