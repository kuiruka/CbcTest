#ifndef __SIGNALSHAPEANALYSER_H__
#define __SIGNALSHAPEANALYSER_H__

#include <vector>
#include <set>
#include <TH1F.h>
#include <TString.h>
#include "GUIData.h"
#include "Strasbourg/Data.h"
#include "Cbc/TestGroup.h"
#include "CbcDaq/Analyser.h"


namespace Analysers{

	class SignalShapeAnalyser;

	class SignalShapeChannelData {

		friend class SignalShapeAnalyser;

		public:
			SignalShapeChannelData(): fOffset(0), fGraph(0), fHist(0), fVCth0(0), fVCth0Error(0), fVCth0Width(0), fVCth0WidthError(0){}

			SignalShapeChannelData( const SignalShapeChannelData &pS ): fOffset(pS.fOffset), fGraph(pS.fGraph), fHist(pS.fHist), 
			fVCth0(pS.fVCth0), fVCth0Error(pS.fVCth0Error), fVCth0Width(pS.fVCth0Width), fVCth0WidthError(pS.fVCth0WidthError){}

			~SignalShapeChannelData(){}

			UInt_t Offset()const{ return fOffset; }
			const TGraphErrors *Graph()const{ return fGraph; }
			const TH1F * Hist()const{ return fHist; }
			float VCth0()const{ return fVCth0; }
			float VCth0Error()const{ return fVCth0Error; }
			float VCth0Width()const{ return fVCth0Width; }
			float VCth0WidthError()const{ return fVCth0WidthError; }

		private:
			TGraphErrors *Graph(){ return fGraph; }
			TH1F * Hist(){ return fHist; }
			void SetOffset( UInt_t pOffset ){ fOffset = pOffset; }
			void SetGraph( TGraphErrors *pG ){ fGraph = pG; }
			void SetHist( TH1F *pH ){ fHist = pH; }
			void SetVCth0( float pVCth0, float pVCth0Error ){ fVCth0 = pVCth0; fVCth0Error = pVCth0Error; }
			void SetVCth0Width( float pVCth0Width, float pVCth0WidthError ){ fVCth0Width = pVCth0Width; fVCth0WidthError = pVCth0WidthError; }

			void ResetHist(){ if( fHist ) fHist->Reset(); }

			UInt_t fOffset;
			TGraphErrors *fGraph;
			TH1F * fHist;
			float fVCth0;
			float fVCth0Error;
			float fVCth0Width;
			float fVCth0WidthError;
	};

	class SignalShapeCbcData {

		public:
			SignalShapeCbcData(){}
			~SignalShapeCbcData(){}

		private:

	};

	typedef DataContainer<SignalShapeChannelData, SignalShapeCbcData> SignalShapeData; 
	typedef FeInfo<SignalShapeChannelData, SignalShapeCbcData>        SignalShapeFeInfo; 
	typedef CbcInfo<SignalShapeChannelData, SignalShapeCbcData>       SignalShapeCbcInfo; 
	typedef Channel<SignalShapeChannelData>                           SignalShapeChannelInfo; 
}

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

