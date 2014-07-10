#include "AnalysedData.h"

namespace Analysers{

	TGraphErrors *CalibrationCbcData::GetGraphVplusVCth0( UInt_t pGroupId ){
		std::map<UInt_t, TGraphErrors *>::iterator cIt = fGraphVplusVCth0.find( pGroupId ); 
		if( cIt == fGraphVplusVCth0.end() ) return 0;
		return cIt->second;
	}
	void CalibrationCbcData::AddGraphVplusVCth0( UInt_t pGroup, TGraphErrors *pGraph ){

		fGraphVplusVCth0.insert( std::pair< UInt_t, TGraphErrors *>( pGroup, pGraph ) );

	}
}

