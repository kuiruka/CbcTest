#ifndef __GUICBCREGFRAME_H__
#define __GUICBCREGFRAME_H__

#include <TGFrame.h>
#include <TGTab.h>
#include <map>

class TGTextEntry;
class TGNumberEntry;
class TGLabel;
class TGCompositeFrame;

namespace Cbc{

	class CbcRegItem;
	class CbcRegList;
}
namespace CbcDaq{

	class GUIFrame;
	class CbcRegFrames;

	class CbcRegWidget : public TGHorizontalFrame {
		public:
			static TGLayoutHints *fLayout;
			static TGLayoutHints *fLayoutName;
			static TGLayoutHints *fLayoutAddress;
			static TGLayoutHints *fLayoutValue;
			static TGLayoutHints *fLayoutNumberEntry;
			static TGLayoutHints *LayoutThis();
			static TGLayoutHints *LayoutName();
			static TGLayoutHints *LayoutAddress();
			static TGLayoutHints *LayoutValue();
			static TGLayoutHints *LayoutNumberEntry();

		public:
			CbcRegWidget( TGCompositeFrame *pFrame, const CbcRegItem *pCbcRegItem, GUIFrame *pGUIFrame, CbcRegFrames *pCbcRegFrames );
			virtual ~CbcRegWidget(){}
			void AddUpdateItem();
			void Update();
			ClassDef( CbcRegWidget, 0 );
		private:
			const CbcRegItem *fCbcRegItem;
			GUIFrame *fGUIFrame;
			CbcRegFrames *fCbcRegFrames;
			TGLabel *fLabelName;
			TGLabel *fLabelAddress;
			TGLabel *fLabelValue;
			TGNumberEntry *fNumberEntry;
	};

	typedef std::pair<const CbcRegItem *, CbcRegWidget *> CbcRegGUIPair;
	typedef std::map<const CbcRegItem *, CbcRegWidget *> CbcRegGUIMap;

	class CbcRegRWFileCommandFrame : public TGHorizontalFrame{

		public:
			CbcRegRWFileCommandFrame( TGCompositeFrame *pFrame, const CbcRegList &pList, GUIFrame *pGUIFrame );
			virtual ~CbcRegRWFileCommandFrame(){;}
			void ReConfigureCbc();	
			void Save();	
			ClassDef( CbcRegRWFileCommandFrame, 0 );
		private:
			UInt_t fFeId;
			UInt_t fCbcId;
			GUIFrame *fGUIFrame;
			TGLabel *fLabel;	
			TGTextEntry *fTextEntry;
			TGTextButton *fButtonLoadAndSet;
			TGTextButton *fButtonSave;
	};
	class CbcRegFrames;
	class CbcRegFrame : public TGTab {

		public:
			CbcRegFrame( const CbcRegList &pCRL, TGCompositeFrame *pFrame, GUIFrame *pGUIFrame, CbcRegFrames *pCbcRegFrames ); 
			virtual ~CbcRegFrame();
			ClassDef( CbcRegFrame, 0 );
			bool ConfigureFailed()const{ return fConfigureFailed; }

		private:
			TGCompositeFrame *fMotherFrame;
			GUIFrame         *fGUIFrame;
			CbcRegFrames     *fCbcRegFrames;
			bool             fConfigureFailed;
	};

	typedef std::pair<const CbcRegItem *, UInt_t> CbcRegUpdateItem;
	typedef std::map<const CbcRegItem *, UInt_t> CbcRegUpdateList;

	class CbcRegFrames : public TGCompositeFrame{

		friend class CbcRegFrame;	

		public:
		CbcRegFrames( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
		virtual ~CbcRegFrames(){;}
		void RenewCbcRegFrames();

		const CbcRegGUIMap &GetCbcRegGUIMap()const{ return fCbcRegGUIMap; }

		void AddCbcRegUpdateItem( const CbcRegItem *pCbcRegItem, UInt_t cValue );
		void Update( std::vector<const CbcRegItem *> pList );
		void UpdateCbcRegisters();
		void ResetCbcRegUpdateList();
		bool ConfigureFailed()const{ return fConfigureFailed; }

		ClassDef( CbcRegFrames, 0 );

		private:
		TGCompositeFrame *fMotherFrame;
		GUIFrame *fGUIFrame;
		TGTab *fTabFrame;
		TGHorizontalFrame *fCommandFrame;
		TGTextButton *fCbcRegUpdateButton;
		TGTextButton *fCbcRegUpdateResetButton;

		CbcRegGUIMap                    fCbcRegGUIMap;
		CbcRegUpdateList                fCbcRegUpdateList;
		bool             fConfigureFailed;
	};
}
#endif

