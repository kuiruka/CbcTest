#ifndef __GUIDATA_H__
#define __GUIDATA_H__

#include <TCanvas.h>
#include "DataContainer.h"

class TPad;
namespace ICCalib{

	class GUIChannelData {

		public:
			GUIChannelData(){}
			GUIChannelData( const GUIChannelData &pC ){}

	};

	class GUICbcData {

		public:
			GUICbcData(): fVplusVsVCth0GraphDisplayPad(0), fDummyPad(0){}
			GUICbcData( const GUICbcData &pC ):
				fScurveHistogramDisplayPad(pC.fScurveHistogramDisplayPad),
				fVplusVsVCth0GraphDisplayPad(pC.fVplusVsVCth0GraphDisplayPad),
				fDummyPad(pC.fDummyPad){}

			TPad *GetVplusVsVCth0GraphDisplayPad(){ return fVplusVsVCth0GraphDisplayPad; }
			TPad *GetScurveHistogramDisplayPad( UInt_t pGroupId ){ return fScurveHistogramDisplayPad.find( pGroupId )->second; }
			TPad *GetDummyPad( UInt_t pFeId, UInt_t pCbcId ){ if( fDummyPad == 0 ) fDummyPad =
				new TCanvas( Form( "cFE%dCBC%d", pFeId, pCbcId ), Form( "cFE%dCBC%d", pFeId, pCbcId ), 100, 100 ); return fDummyPad; }

			void SetVplusVsVCth0GraphDisplayPad( TPad *pPad );
			void SetScurveHistogramDisplayPad( UInt_t pGroupId, TPad *pPad ); 
			void SetDummyPad( TPad *pPad ){ fDummyPad = pPad; }

		private:
			std::map<UInt_t, TPad *>         fScurveHistogramDisplayPad;
			TPad                             *fVplusVsVCth0GraphDisplayPad;
			TPad                             *fDummyPad;
	};

	typedef DataContainer<GUIChannelData, GUICbcData> GUIData; 
	typedef FeInfo<GUIChannelData, GUICbcData>        GUIFeInfo;
	typedef CbcInfo<GUIChannelData, GUICbcData>       GUICbcInfo;
}

#endif

