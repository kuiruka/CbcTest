#ifndef __ERRORANALYSER_H__
#define __ERRORANALYSER_H__
#include "../Strasbourg/Data.h"
#include "TH1F.h"
#include <vector>
#include <set>
#include <TString.h>
#include "Analyser.h"
#include "Analysers/DataContainer.h"

class TGTextView;

namespace Cbc{
	class CbcRegMap;
}

using namespace Cbc;
using namespace Strasbourg;

class TPad;

namespace Analysers{
	
	class ErrorHistGroup;
	class A{};
	typedef DataContainer<class A, ErrorHistGroup> ErrorAnalyserResult;
	typedef FeInfo<class A, ErrorHistGroup>        ErrorAnalyserFeInfo; 
	typedef CbcInfo<class A, ErrorHistGroup>       ErrorAnalyserCbcInfo; 
	typedef Channel<class A>                       ErrorAnalyserChannelInfo; 
}
namespace CbcDaq{
	class GUIFrame;
}

using namespace CbcDaq;

namespace Analysers{

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
			void SetL1APointer( UInt_t pValue ){ fL1APointer = pValue; }
			void ResetHist();
			UInt_t   Analyse( const Event *pEvent, bool pFillDataStream = true );
			void DrawHists();
			void DrawText();
			void SetHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );	

		private:
			UInt_t                      fL1APointer;
			ErrorAnalyserResult         fResult;
	};

	class ErrorHistGroup : public HistGroup{
		friend class ErrorAnalyser;
		public:
		ErrorHistGroup(){;}
		~ErrorHistGroup();
		void SetHistograms(UInt_t pBeId, UInt_t pFeId, UInt_t pCbcId );
		private:
		std::pair<TH1F *, TPad *> fData;
		std::pair<TH1F *, TPad *> fError;
		std::pair<TH1F *, TPad *> fPipelineAddress0;
		std::pair<TH1F *, TPad *> fPipelineAddress1;
		std::pair<TH1F *, TPad *> fPipelineAddress2;
		std::pair<TH1F *, TPad *> fPipelineAddress3;
		UInt_t fExpectedPipelineAddress;
	};
}
#endif

