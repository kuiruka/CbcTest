#include "GUIData.h"
#include <iostream>

namespace Analysers{


	void GUICbcData::SetScurveHistogramDisplayPad( UInt_t pGroupId, TPad *pPad ){ 

		fScurveHistogramDisplayPad.insert( std::pair< UInt_t, TPad *>( pGroupId, pPad ) );
	}
	void GUICbcData::SetVplusVsVCth0GraphDisplayPad( TPad *pPad ){

		fVplusVsVCth0GraphDisplayPad = pPad;
	}
	void GUICbcData::SetSignalShapeGraphDisplayPad( UInt_t pGroupId, TPad *pPad ){ 

		fSignalShapeGraphDisplayPad.insert( std::pair< UInt_t, TPad *>( pGroupId, pPad ) );
	}
}

