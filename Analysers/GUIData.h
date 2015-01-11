#ifndef __GUIDATA_H__
#define __GUIDATA_H__

#include <TCanvas.h>
#include "DataContainer.h"

class TPad;
namespace Analysers{

	class GUIChannelData {

		public:
			GUIChannelData(){}
			GUIChannelData( const GUIChannelData &pC ){}

	};

	class GUICbcData {

		public:
			GUICbcData(): fDummyPad(0), fVplusVsVCth0GraphDisplayPad(0){}
			GUICbcData( const GUICbcData &pC ):
				fScurveHistogramDisplayPad(pC.fScurveHistogramDisplayPad),
				fDummyPad(pC.fDummyPad),
				fVplusVsVCth0GraphDisplayPad(pC.fVplusVsVCth0GraphDisplayPad),
				fSignalShapeGraphDisplayPad( pC.fSignalShapeGraphDisplayPad ){}

			TPad *GetVplusVsVCth0GraphDisplayPad(){ return fVplusVsVCth0GraphDisplayPad; }
			TPad *GetScurveHistogramDisplayPad( UInt_t pGroupId ){ return fScurveHistogramDisplayPad.find( pGroupId )->second; }
			TPad *GetSignalShapeGraphDisplayPad( UInt_t pGroupId ){ return fSignalShapeGraphDisplayPad.find( pGroupId )->second; }
			/*
			TPad *GetDummyPad( UInt_t pFeId, UInt_t pCbcId ){ if( fDummyPad == 0 ) fDummyPad =
				new TCanvas( Form( "cFE%dCBC%d", pFeId, pCbcId ), Form( "cFE%dCBC%d", pFeId, pCbcId ), 100, 100 ); return fDummyPad; }
				*/
			TPad *GetDummyPad(){ return fDummyPad; }

			void SetScurveHistogramDisplayPad( UInt_t pGroupId, TPad *pPad ); 
			void SetVplusVsVCth0GraphDisplayPad( TPad *pPad );
			void SetSignalShapeGraphDisplayPad( UInt_t pGroupId, TPad *pPad ); 
			void SetDummyPad( TPad *pPad ){ fDummyPad = pPad; }

		private:
			std::map<UInt_t, TPad *>         fScurveHistogramDisplayPad;
			TPad                             *fDummyPad;
			TPad                             *fVplusVsVCth0GraphDisplayPad;
			std::map<UInt_t, TPad *>         fSignalShapeGraphDisplayPad;
	};

	typedef DataContainer<GUIChannelData, GUICbcData> GUIData; 
	typedef FeInfo<GUIChannelData, GUICbcData>        GUIFeInfo;
	typedef CbcInfo<GUIChannelData, GUICbcData>       GUICbcInfo;
}

#endif

