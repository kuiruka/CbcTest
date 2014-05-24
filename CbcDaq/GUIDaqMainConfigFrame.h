#ifndef __GUIDAQMAINCONFIGFRAME_H__
#define __GUIDAQMAINCONFIGFRAME_H__

#include <TGFrame.h>
#include <TGTextEntry.h>

class TGNumberEntry;

namespace CbcDaq{

	class GUIFrame;

	typedef std::pair<std::string, TGNumberEntry *> GlibItemEntry;
	typedef std::map<std::string, TGNumberEntry *> GlibSettingEntries;

	typedef std::pair<TString, TGTextEntry *> CbcRegFileNameEntry;
	typedef std::map<TString, TGTextEntry *> CbcRegFileNameEntries;

	typedef std::pair<std::string, TGNumberEntry *> RunItemEntry;
	typedef std::map<std::string, TGNumberEntry *> RunSettingEntries;

	class GlibMainConfigurationFrame : public TGCompositeFrame {

		public:

			GlibMainConfigurationFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			~GlibMainConfigurationFrame();
			void RenewFrame();
			void SetColors( ULong_t pColor ); 
			const char *GetUhalConfigFileName()const{ return fTextEntryUhalConfigFile->GetText(); }
			const GlibSettingEntries &GetGlibSettingEntries()const{ return fGlibSettingEntries; }
			ClassDef( GlibMainConfigurationFrame, 0 );
		private:
			void AddGlibSettingItem( const char *pName, UInt_t pValue, UInt_t pRowId, TGCompositeFrame *pFrame );
			TGCompositeFrame *fMotherFrame;
			GUIFrame *fGUIFrame;
			TGVerticalFrame *fFrame;
			TGTextEntry                     *fTextEntryUhalConfigFile;
			TGTextEntry						*fTextEntryBoardId;
			GlibSettingEntries              fGlibSettingEntries;
	};

	class CbcRegFileNameFrame : public TGCompositeFrame{

		public:
			CbcRegFileNameFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			virtual ~CbcRegFileNameFrame(){;}
			void RenewFrame();
			const CbcRegFileNameEntries &GetCbcRegFileNameEntries()const;
			void SetColors( ULong_t pColor ); 
			void SetCbcRegFileName( UInt_t pFeId, UInt_t pCbcId, const char *pName );
			ClassDef( CbcRegFileNameFrame, 0 );
		private:
			TGCompositeFrame                *fMotherFrame;
			GUIFrame                        *fGUIFrame;
			TGVerticalFrame                 *fFrame;
			CbcRegFileNameEntries           fCbcRegFileNameEntries;
	};

	class RunConfigurationFrame : public TGCompositeFrame{

		public:
			RunConfigurationFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			virtual ~RunConfigurationFrame(){}
			void RenewFrame();
			void SetColors( ULong_t pColor ); 
			void AddRunSettingItem( const char *pName, UInt_t pValue, UInt_t pRowId, TGCompositeFrame *pFrame );
			const RunSettingEntries &GetRunSettingEntries()const{ return fRunSettingEntries; }
			ClassDef( RunConfigurationFrame, 0 );
		private:
			TGCompositeFrame                *fMotherFrame;
			GUIFrame                        *fGUIFrame;
			TGGroupFrame                    *fFrame;
			RunSettingEntries              fRunSettingEntries;
	};

	class DaqMainConfigurationFrame : public TGHorizontalFrame {

		public:
			DaqMainConfigurationFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			~DaqMainConfigurationFrame();
			void RenewGlibMainConfigurationFrame();
			void RenewCbcRegFileNameFrame(); 
			void RenewRunConfigurationFrame();
			void SetGlibConfigColors( ULong_t pColor ); 
			void SetRunConfigColors( ULong_t pColor ); 
			void SetCbcRegFileNameColors( ULong_t pColor );

			const char *GetUhalConfigFileName()const{ return fGlibMainConfigFrame->GetUhalConfigFileName(); }
			const GlibSettingEntries &GetGlibSettingEntries()const{ return fGlibMainConfigFrame->GetGlibSettingEntries(); }
			const CbcRegFileNameEntries &GetCbcRegFileNameEntries()const;
			void SetCbcRegFileName( UInt_t pFeId, UInt_t pCbcId, const char *pName ){ fCbcRegFileNameFrame->SetCbcRegFileName( pFeId, pCbcId, pName ); }
			const RunSettingEntries &GetRunSettingEntries()const{ return fRunConfigFrame->GetRunSettingEntries(); }

			GlibMainConfigurationFrame *GetGlibMainConfigurationFrame(){ return fGlibMainConfigFrame; }
			CbcRegFileNameFrame *GetCbcRegFileNameFrame(){ return fCbcRegFileNameFrame; }
			ClassDef( DaqMainConfigurationFrame, 0 );
		private:
			TGCompositeFrame *fMotherFrame;
			GUIFrame *fGUIFrame;
			GlibMainConfigurationFrame        *fGlibMainConfigFrame;
			CbcRegFileNameFrame               *fCbcRegFileNameFrame;
			RunConfigurationFrame             *fRunConfigFrame;
	};
}
#endif

