#ifndef __SCURVEANALYSER_H__
#define __SCURVEANALYSER_H__
#include "GUIData.h"
#include "../Strasbourg/Data.h"
#include "TH1F.h"
#include <vector>
#include <set>
#include <TString.h>
#include "CbcDaq/Analyser.h"

namespace Analysers{

	class ScurveAnalyser;

	class CalibrationChannelData {

		friend class ScurveAnalyser;

		public:
			CalibrationChannelData(): fOffset(0), fNextOffset(0), fHist(0), fVCth0(0), fVCth0Error(0){}
			CalibrationChannelData( const CalibrationChannelData &pC ): fOffset(pC.fOffset), fNextOffset(pC.fNextOffset), fHist(pC.fHist), fVCth0(pC.fVCth0), fVCth0Error(pC.fVCth0Error) {}
			~CalibrationChannelData(){}

			const TH1F * GetHist()const{ return fHist; }
			UInt_t Offset()const{ return fOffset; }
			float VCth0()const{ return fVCth0; }
			float VCth0Error()const{ return fVCth0Error; }

		private:
			TH1F * Hist(){ return fHist; }
			void SetOffset( UInt_t pOffset ){ fOffset = pOffset; }
			void SetNextOffset( UInt_t pOffset ){ fNextOffset = pOffset; }
			void SetHist( TH1F *pH ){ fHist = pH; }
			void SetVCth0( float pVCth0, float pVCth0Error ){ fVCth0 = pVCth0; fVCth0Error = pVCth0Error; }

			void UpdateOffset(){ fOffset = fNextOffset; }
			void ResetHist(){ if( fHist ) fHist->Reset(); }
			UInt_t fOffset;
			UInt_t fNextOffset;
			TH1F * fHist;
			float fVCth0;
			float fVCth0Error;
	};

	class CalibrationCbcData {

		friend class ScurveAnalyser;

		public:
			CalibrationCbcData(){}
			CalibrationCbcData( const CalibrationCbcData &pC ):	
				fGraphVplusVCth0(pC.fGraphVplusVCth0), 
				fVplus(pC.fVplus){}

			std::map<UInt_t, TGraphErrors *> * GetGraphVplusVCth0(){ return &fGraphVplusVCth0; }
			const TGraphErrors *GetGraphVplusVCth0( UInt_t pGroupId )const;
			UInt_t Vplus()const{ return fVplus; }

		private:
			TGraphErrors *GetGraphVplusVCth0( UInt_t pGroupId );
			void AddGraphVplusVCth0( UInt_t pGroup, TGraphErrors *pGraph );
			void SetVplus( UInt_t pValue ){ fVplus = pValue; }

			std::map<UInt_t, TGraphErrors *> fGraphVplusVCth0;
			UInt_t                           fVplus;
	};

	typedef DataContainer<CalibrationChannelData, CalibrationCbcData> CalibrationResult; 
	typedef FeInfo<CalibrationChannelData, CalibrationCbcData>        CalibrationFeInfo; 
	typedef CbcInfo<CalibrationChannelData, CalibrationCbcData>       CalibrationCbcInfo; 
	typedef Channel<CalibrationChannelData>                       CalibrationChannelInfo; 
}

namespace Cbc{

	class CbcRegMap;
	template <class T> class _TestGroup;
	template <class T> class _TestGroupMap;
	typedef _TestGroup<UInt_t>                    TestGroup;
	typedef _TestGroupMap<UInt_t>                 TestGroupMap;
	typedef _TestGroup<Analysers::Channel<Analysers::CalibrationChannelData> >   CalibrationTestGroup;
	typedef _TestGroupMap<Analysers::Channel<Analysers::CalibrationChannelData> > CalibrationTestGroupMap;
}

using namespace Cbc;
using namespace Strasbourg;
using namespace CbcDaq;

namespace Analysers{


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

