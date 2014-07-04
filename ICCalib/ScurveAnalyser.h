#ifndef __SCURVEANALYSER_H__
#define __SCURVEANALYSER_H__
#include "AnalysedData.h"
#include "GUIData.h"
#include "../Strasbourg/Data.h"
#include "TH1F.h"
#include <vector>
#include <set>
#include <TString.h>
#include "TestGroup.h"
#include "CbcDaq/Analyser.h"

namespace Cbc{
	class CbcRegMap;
}

using namespace Cbc;
using namespace Strasbourg;
using namespace CbcDaq;

namespace ICCalib{

	typedef _TestGroup<CalibrationChannelData>    CalibrationTestGroup;
	typedef _TestGroupMap<CalibrationChannelData> CalibrationTestGroupMap;

	class ScurveAnalyser : public Analyser{

		public:
			enum { OFFSETCALIB, VPLUSSEARCH, SINGLEVCTHSCAN };
		public:
			ScurveAnalyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
					CalibrationTestGroupMap *pGroupMap, const CbcRegMap *pMap,
					Bool_t pNegativeLogicCbc, UInt_t pTargetVCth, const char *pOutputDir, GUIFrame *pGUIFrame ); 
			virtual ~ScurveAnalyser(){}
			void   Initialise();
			void   SetOffsets();
			void   SetOffsets( UInt_t pInitalOffset ); 
			void   Configure( Int_t pType, Int_t pOffsetTargetBit=8 );
			int    FillGraphVplusVCth0();
			void   DrawVplusVCth0();
			void   PrintVplusVsVCth0DisplayPads();
			int    FillHists( UInt_t pVcth, const Event *pEvent );
			void   FitHists( UInt_t pMin, UInt_t pMax );
			void   DrawHists();
			void   PrintScurveHistogramDisplayPads();
			UInt_t GetOffset( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannel );
			UInt_t GetVplus( UInt_t pFeId, UInt_t pCbcId );
			UInt_t GetMinVCth0(){ return fMinVCth0; }
			UInt_t GetMaxVCth0(){ return fMaxVCth0; }
			void   DumpResult();
			const  CalibrationResult &GetResult()const{ return fResult; }
			std::vector<CalibrationChannelInfo *>   *GetChannelList(){ return &fChannelList; }
			void   SetVplus();
			void   SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );
			void   SetVplusVsVCth0GraphDisplayPad( UInt_t pFeId, TPad *pPad );

		private:
			TH1F  *createScurveHistogram( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannel );
			void  resetHistograms();
			void  updateOffsets();
			TString getScanId();
			TString getScanType();
			void  setNextOffsets();

			CalibrationTestGroupMap           *fGroupMap;
			UInt_t                 fTargetVCth;
			Int_t                  fScanType;
			Int_t                  fCurrentTargetBit;
			Int_t                  fNthVplusPoint;
			std::vector<CalibrationChannelInfo *> fChannelList;
			CalibrationResult      fResult;
			GUIData                fGUIData;
			TH1F                   *fHtotal;
			TH1F                   *fDummyHist;
			UInt_t                 fMinVCth0;
			UInt_t                 fMaxVCth0;
	};

}
#endif

