#ifndef __ANALYSER_H__
#define __ANALYSER_H__
#include "../Strasbourg/Data.h"
#include "TH1F.h"
#include <vector>
#include <set>
#include <TString.h>

class TGTextView;

namespace Cbc{
	class CbcRegMap;
}

using namespace Cbc;
using namespace Strasbourg;

class TPad;

namespace CbcDaq{
	
	class GUIFrame;
}
using namespace CbcDaq;

namespace Analysers{

	class HistGroup;
	typedef std::map<Int_t, HistGroup*> HistGroupMap;


	class Analyser {

		public:
			Analyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
					const CbcRegMap *pMap, 
					Bool_t pNegativeLogicCbc,
					const char *pOutputDir,
					GUIFrame *pGUIFrame = 0 );
			virtual ~Analyser();
			virtual void Initialise();
			virtual void   Configure(){;}
			virtual void ConfigureRun(){}
			virtual UInt_t   Analyse(const Event *pEvent, bool pFillDataStream = true );
			virtual void DrawHists();
			virtual void DrawText();
			virtual void FinishRun(){}
			virtual void SetHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );	
			virtual void SetTextView( TGTextView *pTextView ){ fTextView = pTextView; }
			void ShowDataStream( Bool_t pShow ){ fShowDataStream = pShow; }
			Bool_t ShowDataStream(){ return fShowDataStream; }

		protected:
			virtual Int_t getHistGroupId( UInt_t pFeId, UInt_t pCbcId );

			UInt_t                  fBeId;
			UInt_t                  fNFe;
			UInt_t                  fNCbc;
			const CbcRegMap         *fCbcRegMap;
			bool                    fNegativeLogicCbc;
			TString                 fOutputDir;
			GUIFrame                *fGUIFrame;
			Bool_t                  fShowDataStream;
			TGTextView              *fTextView;
			HistGroupMap            fHistGroupMap;
	};

	class HistGroup {
		friend class Analyser;
		public:
		HistGroup(){}
		~HistGroup();
		void SetHistograms( UInt_t pBeId, UInt_t pFeId, UInt_t pCbcId );
		protected:
		std::pair<TH1F *, TPad *> fDataStream;
	};
}
#endif

