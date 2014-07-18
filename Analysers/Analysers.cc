#include "Analysers.h"

#include "TMath.h"
#include <math.h>

namespace Analysers{

	Double_t MyErf( Double_t *x, Double_t *par ){
		Double_t x0 = par[0];
		Double_t width = par[1];
		Double_t fitval(0);
		if( x[0] < x0 ) fitval = 0.5 * TMath::Erfc( ( x0 - x[0] )/width );
		else fitval = 0.5 + 0.5 * TMath::Erf( ( x[0] - x0 )/width );
		return fitval;
	}
}

