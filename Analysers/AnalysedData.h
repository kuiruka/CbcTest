#ifndef __ANALYSEDDATA_H__
#define __ANALYSEDDATA_H__

#include <map>
#include <TROOT.h>
#include <TH1F.h>
#include "DataContainer.h"

class TGraphErrors;
namespace Analysers{

	class SignalShapeChannelData {

		public:
			SignalShapeChannelData(): fOffset(0), fGraph(0), fHist(0), fVCth0(0), fVCth0Error(0), fVCth0Width(0), fVCth0WidthError(0){}

			SignalShapeChannelData( const SignalShapeChannelData &pS ): fOffset(pS.fOffset), fGraph(pS.fGraph), fHist(pS.fHist), 
			fVCth0(pS.fVCth0), fVCth0Error(pS.fVCth0Error), fVCth0Width(pS.fVCth0Width), fVCth0WidthError(pS.fVCth0WidthError){}

			~SignalShapeChannelData(){}

			UInt_t Offset()const{ return fOffset; }
			TGraphErrors *Graph(){ return fGraph; }
			TH1F * Hist(){ return fHist; }
			float VCth0()const{ return fVCth0; }
			float VCth0Error()const{ return fVCth0Error; }
			float VCth0Width()const{ return fVCth0Width; }
			float VCth0WidthError()const{ return fVCth0WidthError; }

			void SetOffset( UInt_t pOffset ){ fOffset = pOffset; }
			void SetGraph( TGraphErrors *pG ){ fGraph = pG; }
			void SetHist( TH1F *pH ){ fHist = pH; }
			void SetVCth0( float pVCth0, float pVCth0Error ){ fVCth0 = pVCth0; fVCth0Error = pVCth0Error; }
			void SetVCth0Width( float pVCth0Width, float pVCth0WidthError ){ fVCth0Width = pVCth0Width; fVCth0WidthError = pVCth0WidthError; }

			void ResetHist(){ if( fHist ) fHist->Reset(); }

		private:
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

	typedef DataContainer<SignalShapeChannelData, SignalShapeCbcData> SignalShapeData; 
	typedef FeInfo<SignalShapeChannelData, SignalShapeCbcData>        SignalShapeFeInfo; 
	typedef CbcInfo<SignalShapeChannelData, SignalShapeCbcData>       SignalShapeCbcInfo; 
	typedef Channel<SignalShapeChannelData>                           SignalShapeChannelInfo; 

	typedef DataContainer<CalibrationChannelData, CalibrationCbcData> CalibrationResult; 
	typedef FeInfo<CalibrationChannelData, CalibrationCbcData>        CalibrationFeInfo; 
	typedef CbcInfo<CalibrationChannelData, CalibrationCbcData>       CalibrationCbcInfo; 
	typedef Channel<CalibrationChannelData>                       CalibrationChannelInfo; 
}


#endif

