#ifndef __SIGNALSHAPEANALYSER_H__
#define __SIGNALSHAPEANALYSER_H__

#include <vector>
#include <set>
#include <TH1F.h>
#include <TString.h>
#include "AnalysedData.h"
#include "GUIData.h"
#include "Strasbourg/Data.h"
#include "Cbc/TestGroup.h"
#include "CbcDaq/Analyser.h"


namespace Cbc{

	class CbcRegMap;
	template <class T> class _TestGroup;
	template <class T> class _TestGroupMap;
	typedef _TestGroup<UInt_t>                    TestGroup;
	typedef _TestGroupMap<UInt_t>                 TestGroupMap;
	typedef _TestGroup<Analysers::Channel<Analysers::SignalShapeChannelData> >   SignalShapeTestGroup;
	typedef _TestGroupMap<Analysers::Channel<Analysers::SignalShapeChannelData> > SignalShapeTestGroupMap;
}

using namespace Cbc;
using namespace Strasbourg;
using namespace CbcDaq;

namespace Analysers{

	class SignalShapeAnalyser : public Analyser{

		public:
			SignalShapeAnalyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
					SignalShapeTestGroupMap *pGroupMap, const CbcRegMap *pMap, 
					Bool_t pNegativeLogicCbc, const char *pSignalType, const char *pOutputDir, GUIFrame *pGUIFrame );
			virtual ~SignalShapeAnalyser(){}
			void   Initialise();
			void   SetOffsets();
			void   Configure( Double_t pCurrentTime );
			int    FillHists( UInt_t pVcth, const Event *pEvent );
			void   FitHists( UInt_t pMin, UInt_t pMax );
			UInt_t GetMinVCth0(){ return fMinVCth0; }
			UInt_t GetMaxVCth0(){ return fMaxVCth0; }
			void   DrawHists();
			void   DrawGraphs();
			void   SetSignalShapeGraphDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );
			void   SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );
			UInt_t GetOffset( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannel );
			void   SaveSignalShape();
		private:
			TH1F  *createScurveHistogram( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannel );
			TGraphErrors *createSignalShapeGraph( UInt_t pFeID, UInt_t pCbcId, UInt_t pChannel );
			void  resetHistograms();

		private:
			TString                               fSignalType;	
			SignalShapeTestGroupMap               *fGroupMap;
			std::vector<SignalShapeChannelInfo *> fChannelList;
			SignalShapeData                       fSignalShapeData;
			GUIData                               fGUIData;
			TH1F                                  *fHtotal;
			TH1F                                  *fDummyHist;
			UInt_t                                fMinVCth0;
			UInt_t                                fMaxVCth0;
			Double_t                              fCurrentTime;

	};
}


#endif

