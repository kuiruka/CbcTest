#include <TGraph.h>
#include <TString.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <iostream>
#include <TStyle.h>
#include <TPad.h>

void PlotGain2( Char_t *TPValues, Bool_t pMode = 1, UInt_t pBeId = 0, UInt_t pFeId = 0, UInt_t pCbc = 0 ){

	TGraph *g[254];
	for( UInt_t i=0; i<254; i++ ){
		g[i] = new TGraph(5);
	}
	TString cTPValues( TPValues );
	TObjArray *Strings = cTPValues.Tokenize( ":" );
	if( Strings->GetEntriesFast()){
		TIter iString(Strings);
		TObjString *os=0;
		Int_t j=0;
		while( (os = (TObjString *) iString() ) ){

			double m(0), sum_m(0), sum_m2(0);
			unsigned int tp;
			TFile *fin = new TFile( Form( "SingleScan%s.root", os->GetString().Data() ) );
			sscanf( os->GetString().Data(), "%X", &tp );
			std::cout << os->GetString() << std::endl;
			std::cout << tp << std::endl;
			for( UInt_t i=0; i<254; i++ ){
				TString hname = Form( "h_%02d_%02d_%02d_%03d", pBeId, pFeId, pCbc, i );
				TH1F *h = (TH1F *) fin->Get( hname );
				TF1 *func = h->GetFunction( Form("f_%s", hname.Data() ) );
				m = func->GetParameter(0);
				g[i]->SetPoint( j, tp, m );
			}
			j++;
		}
	}
	TH1F *h;
	gStyle->SetOptStat(0);
	if( pMode == 1 ){
		h = new TH1F( "h", ";Test pulse register value; S-curve mid-point VCTH register value", 1, 170, 256 ); 
		h->SetMinimum( 0 );
		h->SetMaximum( 130 );
	}
	else{
		h = new TH1F( "h", ";Test pulse register value; S-curve mid-point VCTH register value", 1, -1, 86 ); 
		h->SetMinimum( 110 );
		h->SetMaximum( 256 );
	}
	h->Draw();
	for( UInt_t i=0; i<254; i++ ){
		g[i] ->Draw("same");
	}
	gPad->Print( "Gain.png" );
	gPad->Clear();
	h->Draw();
	for( UInt_t i=0; i<10; i++ ){
		g[i] ->Draw("same");
		g[i] ->SetLineColor(i+1);
	}
	gPad->Print( "Gain10.png" );
}

