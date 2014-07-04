#ifndef __ANALYSEDDATA_H__
#define __ANALYSEDDATA_H__

#include <map>
#include <TROOT.h>
#include <TH1F.h>
#include "DataContainer.h"

class TGraphErrors;
namespace ICCalib{

	class SignalShape {

		public:
			SignalShape():fGraph(0){}

			SignalShape( const SignalShape &pS ): fGraph(pS.fGraph){}

			~SignalShape(){}

			TGraphErrors *Graph(){ return fGraph; }
			private:
			TGraphErrors *fGraph;

	};

	class CalibrationChannelData {
		
		public:
			CalibrationChannelData(): fOffset(0), fNextOffset(0), fHist(0), fVCth0(0), fVCth0Error(0){}
			CalibrationChannelData( const CalibrationChannelData &pC ): fOffset(pC.fOffset), fNextOffset(pC.fNextOffset), fHist(pC.fHist), fVCth0(pC.fVCth0), fVCth0Error(pC.fVCth0Error) {}
			~CalibrationChannelData(){}

			UInt_t Offset()const{ return fOffset; }
			TH1F * Hist(){ return fHist; }
			float VCth0()const{ return fVCth0; }
			float VCth0Error()const{ return fVCth0Error; }

			void SetOffset( UInt_t pOffset ){ fOffset = pOffset; }
			void SetNextOffset( UInt_t pOffset ){ fNextOffset = pOffset; }
			void SetHist( TH1F *pH ){ fHist = pH; }
			void SetVCth0( float pVCth0, float pVCth0Error ){ fVCth0 = pVCth0; fVCth0Error = pVCth0Error; }

			void UpdateOffset(){ fOffset = fNextOffset; }
			void ResetHist(){ if( fHist ) fHist->Reset(); }

		private:
			UInt_t fOffset;
			UInt_t fNextOffset;
			TH1F * fHist;
			float fVCth0;
			float fVCth0Error;
	};

	class CalibrationCbcData {

		public:
			CalibrationCbcData(){}
			CalibrationCbcData( const CalibrationCbcData &pC ):	
				fGraphVplusVCth0(pC.fGraphVplusVCth0), 
				fVplus(pC.fVplus){}

			std::map<UInt_t, TGraphErrors *> * GetGraphVplusVCth0(){ return &fGraphVplusVCth0; }
			TGraphErrors *GetGraphVplusVCth0( UInt_t pGroupId );
			UInt_t Vplus()const{ return fVplus; }

			void AddGraphVplusVCth0( UInt_t pGroup, TGraphErrors *pGraph );
			void SetVplus( UInt_t pValue ){ fVplus = pValue; }

		private:
			std::map<UInt_t, TGraphErrors *> fGraphVplusVCth0;
			UInt_t                           fVplus;
	};

	typedef DataContainer<CalibrationChannelData, CalibrationCbcData> CalibrationResult; 
	typedef FeInfo<CalibrationChannelData, CalibrationCbcData>        CalibrationFeInfo; 
	typedef CbcInfo<CalibrationChannelData, CalibrationCbcData>       CalibrationCbcInfo; 
	typedef Channel<CalibrationChannelData>                       CalibrationChannelInfo; 
}
#endif

