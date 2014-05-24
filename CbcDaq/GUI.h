#ifndef __GUI_H__
#define __GUI_H__

#include <TROOT.h>
#include <TGTableLayout.h>
#include <TGFrame.h>
#include <TGTextEntry.h>
#include <TGNumberEntry.h>

void PrintLog( const char *pStr );

namespace CbcDaq{

	TString getStringId( UInt_t pFe, UInt_t pCbc );
	void getId( const char *pName, UInt_t &pFe, UInt_t &pCbc );

	extern UInt_t gMainFrameWidth;
	extern UInt_t gMainFrameHeight;
	extern UInt_t gNumberEntryId;
	extern TGLayoutHints *gLHVexpand;	
	extern TGLayoutHints *gLHexpand; 
	extern TGLayoutHints *gLHexpandTop; 
	extern TGLayoutHints *gLCenterX;  
	extern TGLayoutHints *gLCenterXTop;  
	extern TGLayoutHints *gLNormal;
	extern TGLayoutHints *gLLeft;
	extern TGLayoutHints *gLLeftTop;

	class GUITextEntry : public TGTextEntry {

		public:
			GUITextEntry(const TGWindow* parent = 0, const char* text = 0, Int_t id = -1);
			GUITextEntry(const TString& contents, const TGWindow* parent, Int_t id = -1);
			GUITextEntry(const TGWindow* p, TGTextBuffer* text, Int_t id = -1, GContext_t norm = GetDefaultGC()(), 
					FontStruct_t font = GetDefaultFontStruct(), UInt_t option = kSunkenFrame|kDoubleBorder, Pixel_t back = GetWhitePixel());
			~GUITextEntry(){}
			void SetChangedBackgroundColor();
			void SetErrorBackgroundColor();
			ClassDef( GUITextEntry, 0 );

	};

	class GUINumberEntry : public TGNumberEntry {

		public:
			GUINumberEntry(const TGWindow* parent = 0, Double_t val = 0, Int_t digitwidth = 5, Int_t id = -1, TGNumberFormat::EStyle style = kNESReal, 
					TGNumberFormat::EAttribute attr = kNEAAnyNumber, TGNumberFormat::ELimit limits = kNELNoLimits, Double_t min = 0, Double_t max = 1); 
			~GUINumberEntry(){}
			void SetChangedBackgroundColor();
			void SetErrorBackgroundColor();
			ClassDef( GUINumberEntry, 0 );
	};
}
#endif

