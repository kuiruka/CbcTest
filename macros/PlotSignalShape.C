#include <TFile.h>
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
#include <TGraphErrors.h>
#include <TStyle.h>
#include <TLegend.h>

void PlotSignalShape( Char_t *pPrefix, Char_t *pIpre1Values, UInt_t pBeId = 0, UInt_t pFeId = 0, UInt_t pCbc = 0, UInt_t pChannel = 0, UInt_t color = 4 ){
	
	gStyle->SetOptStat(0);
	TH1F *h = new TH1F( "htmp", ";time [ns];VCTH register value", 1, -30, 360 );
	h->SetMaximum( 140 );
	h->SetMinimum( 80 );
	h->Draw();
	gPad->SetGrid();

	TLegend *cLegend0 = new TLegend( 0.85, 0.75, 0.98, 0.95);
	cLegend0->SetHeader( "Ipre1" );
//	cLegend0->SetFillColor(0);


	TString cIpre1Values( pIpre1Values );
	TObjArray *Strings = cIpre1Values.Tokenize( ":" );
	if( Strings->GetEntriesFast()){
		TIter iString(Strings);
		TObjString *os=0;
		Int_t j=0;
		while( (os = (TObjString *) iString() ) ){

			unsigned int cIpre1(0);
			std::cout << os->GetString() << std::endl;
			TFile *fin = new TFile( Form( "%s%s.root", pPrefix, os->GetString().Data() ) );
			sscanf( os->GetString().Data(), "%2X", &cIpre1 );
			TString hname = Form( "g_%02d_%02d_%02d_%03d", pBeId, pFeId, pCbc, pChannel );
			TGraphErrors *g = (TGraphErrors *) fin->Get( hname );
			j++;
			g->SetMarkerColor(j);
			g->SetLineColor(j);
			g->Draw("P");
			TString label( Form( "0x%02X", cIpre1 )); 
			cLegend0->AddEntry( g, label, "p" ); 
		}
	}
	cLegend0->Draw();
}

