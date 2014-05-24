#include "GUI.h"
#include <iostream>
#include <TGClient.h>

void PrintLog( const char *pStr ){
	//if( !strcmp( pStr, "" ) );
	std::cout << pStr << std::endl;
}

namespace CbcDaq{

	TString getStringId( UInt_t pFe, UInt_t pCbc ){

		return Form( "%1d%1d", pFe, pCbc );
	}

	void getId( const char *pName, UInt_t &pFe, UInt_t &pCbc ){

		TString cName( pName );
		char cCharFe[2], cCharCbc[2];
		cCharFe[1] = 0;
		cCharCbc[1] = 0;
		strncpy( cCharFe, cName.Data(), 1 );
		pFe = atoi( cCharFe );
		strncpy( cCharCbc, &cName.Data()[1], 1 );
		pCbc = atoi( cCharCbc );
	}

	//UInt_t gMainFrameWidth=gClient->GetDisplayWidth();
	//UInt_t gMainFrameHeight=gClient->GetDisplayHeight();
	UInt_t gMainFrameWidth=1400;
	UInt_t gMainFrameHeight=1000;
	UInt_t gNumberEntryId = 0;
	TGLayoutHints *gLHVexpand = new TGLayoutHints( kLHintsExpandX | kLHintsExpandY, 1,1,1,1);
	TGLayoutHints *gLHexpand = new TGLayoutHints( kLHintsExpandX | kLHintsCenterY, 1,1,1,1);	
	TGLayoutHints *gLHexpandTop = new TGLayoutHints( kLHintsExpandX | kLHintsTop, 1,1,1,1);	
	TGLayoutHints *gLCenterX =  new TGLayoutHints( kLHintsCenterX, 1, 1, 1, 1 ); 
	TGLayoutHints *gLCenterXTop =  new TGLayoutHints( kLHintsCenterX | kLHintsTop, 1, 1, 1, 1 ); 
	TGLayoutHints *gLNormal =  new TGLayoutHints( kLHintsNormal, 5, 5, 5, 5 ); 
	TGLayoutHints *gLLeft =  new TGLayoutHints( kLHintsLeft | kLHintsCenterY, 5, 5, 5, 5 ); 
	TGLayoutHints *gLLeftTop =  new TGLayoutHints( kLHintsLeft | kLHintsTop, 5, 5, 5, 5 ); 
	
	GUITextEntry::GUITextEntry( const TString& contents, const TGWindow* parent, Int_t id):
		TGTextEntry(contents,parent,id){}

	GUITextEntry::GUITextEntry(const TGWindow* parent, const char* text, Int_t id):
		TGTextEntry(parent, text, id){}

	GUITextEntry::GUITextEntry(const TGWindow* p, TGTextBuffer* text, Int_t id, GContext_t norm, 
			FontStruct_t font, UInt_t option, Pixel_t back):
		TGTextEntry(p, text, id, norm, font, option, back ){}

	void GUITextEntry::SetChangedBackgroundColor(){

		SetBackgroundColor( 0xFFCC00 );	
	}
	void GUITextEntry::SetErrorBackgroundColor(){

		SetBackgroundColor( 0xFF0000 );	
	}

	GUINumberEntry::GUINumberEntry(const TGWindow* parent, Double_t val, Int_t digitwidth, Int_t id, TGNumberFormat::EStyle style, 
                    		TGNumberFormat::EAttribute attr, TGNumberFormat::ELimit limits, Double_t min, Double_t max): 
							TGNumberEntry(parent, val, digitwidth, id, style, attr, limits, min, max){}

	void GUINumberEntry::SetChangedBackgroundColor(){

		GetNumberEntry()->SetBackgroundColor( 0xFFCC00 );	
		gClient->NeedRedraw( GetNumberEntry() );
	}
	void GUINumberEntry::SetErrorBackgroundColor(){

		GetNumberEntry()->SetBackgroundColor( 0xFF0000 );	
		gClient->NeedRedraw( GetNumberEntry() );
	}
}

