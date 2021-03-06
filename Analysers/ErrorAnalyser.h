#ifndef __ERRORANALYSER_H__
#define __ERRORANALYSER_H__
#include "Strasbourg/Data.h"
#include "Strasbourg/BeController.h"
#include "TH1F.h"
#include "TH2F.h"
#include <vector>
#include <set>
#include <TString.h>
#include "Analysers/Analyser.h"
#include "DataContainer.h"

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
			void   Initialise();
			void   Configure(){;}
			void   Reset();
			void   SetL1APointer   ( UInt_t pValue )        { fL1APointer = pValue; }
			void   SetNclockFRtoL1A( UInt_t pNclockFRtoL1A ){ fNclockFRtoL1A = pNclockFRtoL1A; }
			void   SetNclock1Cycle ( UInt_t pNclock1Cycle  ){ fNclock1Cycle  = pNclock1Cycle;  }
			void   SetBeamIntensity( UInt_t pBeamIntensity ){ fBeamIntensity = pBeamIntensity;  }
			void   SetExpectedDataValue( UInt_t pExpectedDataValue ){ fExpectedDataValue = pExpectedDataValue; }
			void   ResetHist();
			UInt_t Analyse( const Event *pEvent, bool pFillDataStream = true, int pErrorType = BeController::OTHER_ERROR );
			void   DrawHists();
			TString Dump();
			void   DrawText();
			void   DrawText( TString &pText );
			void   SetHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );	
			Float_t GetLiveTime()const{ return fLiveTime; }
			UInt_t                      Nclock1Cycle()const{ return fNclock1Cycle; }
			void SetRealTime( double pRealTime ){ fRealTime = pRealTime; }
			bool ErrorDetected()const{ return fErrorDetected; }
			TString DumpEventError()const{return fEventErrorStr;}
		private:
			UInt_t                      fL1APointer;
			UInt_t                      fNclockFRtoL1A;
			UInt_t                      fNclock1Cycle;
			UInt_t                      fBeamIntensity;
			UInt_t						fExpectedDataValue;
			ErrorAnalyserResult         fResult;
			TH1F						*fErrorTypeHist;
			Float_t						fLiveTime;
			Float_t                     fRealTime;
			TH2F                        *fExpectedPipelineAddressHist;
			bool						fErrorDetected;
			TString                     fEventErrorStr;
			UInt_t						fNUnExpectedBits;
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

