#include "GUIData.h"


namespace ICCalib{


	void GUICbcData::SetVplusVsVCth0GraphDisplayPad( TPad *pPad ){

		fVplusVsVCth0GraphDisplayPad = pPad;
	}
	void GUICbcData::SetScurveHistogramDisplayPad( UInt_t pGroupId, TPad *pPad ){ 

		fScurveHistogramDisplayPad.insert( std::pair< UInt_t, TPad *>( pGroupId, pPad ) );
	}
}

