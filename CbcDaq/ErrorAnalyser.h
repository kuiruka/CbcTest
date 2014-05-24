#ifndef __ERRORANALYSER_H__
#define __ERRORANALYSER_H__
#include "../Strasbourg/Data.h"
#include "TH1F.h"
#include <vector>
#include <set>
#include <TString.h>
#include "Analyser.h"

class TGTextView;

namespace Cbc{
	class CbcRegMap;
}

using namespace Cbc;
using namespace Strasbourg;

class TPad;

namespace CbcDaq{
	
	class ErrorHistGroup;
	typedef std::map<Int_t, ErrorHistGroup*> ErrorHistGroupMap;

	class GUIFrame;

	class ErrorAnalyser : public Analyser{

		public:
			ErrorAnalyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
					const CbcRegMap *pMap, 
					Bool_t pNegativeLogicCbc,
					const char *pOutputDir,
					GUIFrame *pGUIFrame = 0 );
			virtual ~ErrorAnalyser();
			void  Initialise();
			void   Configure(){;}
			void ConfigureRun();
			void ResetHist();
			UInt_t   Analyse( const Event *pEvent, bool pFillDataStream = true );
			void DrawHists();
			void DrawText();
			void SetHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );	

		private:
			ErrorHistGroupMap       fErrorHistGroupMap;
	};

	class ErrorHistGroup : public HistGroup{
		friend class ErrorAnalyser;
		public:
		ErrorHistGroup( UInt_t pBeId, UInt_t pFeId, UInt_t pCbcId );
		~ErrorHistGroup();
		private:
		std::pair<TH1F *, TPad *> fData;
		std::pair<TH1F *, TPad *> fError;
		std::pair<TH1F *, TPad *> fPipelineAddress;
	};
}
#endif

