#include <TFile.h>
#include <TH1F.h>
#include <TF1.h>
#include <TLatex.h>

void PlotScurve( Char_t *fname, UInt_t pBeId = 0, UInt_t pFeId = 0, UInt_t pCbc = 0, UInt_t color = 4 ){
	
	double m(0), sum_m(0), sum_m2(0);
	double w(0), sum_w(0), sum_w2(0);
	TFile *fin = new TFile( fname );
	for( UInt_t i=0; i<254; i++ ){
		TString hname = Form( "h_%02d_%02d_%02d_%03d", pBeId, pFeId, pCbc, i );
		TH1F *h = (TH1F *) fin->Get( hname );
		h->GetXaxis()->SetRangeUser( 100, 170 );
		h->SetLineColor(1);
		h->SetMarkerColor(1);
		h->SetTitle( Form( "FE(00),CBC(%02d); VCth; Rate", pCbc ) );
		TF1 *func = h->GetFunction( Form("f_%s", hname.Data() ) );
		if( i == 0 ) h->Draw("e0");
		else h->Draw("samee0");
		m = func->GetParameter( 0 );
		sum_m += m;
		sum_m2 += m * m;
		w = func->GetParameter( 1 );
		sum_w += w;
		sum_w2 += w * w;
	}

	double m = sum_m/254;
	double em = sum_m2/254 - m*m;
	TLatex *l = new TLatex( 124, 0.6, Form( "<mid-point> = %.2f#pm%.2f", m, em) );
	l->Draw();
	double w = sum_w/254;
	double ew = sum_w2/254 - w*w;
	l = new TLatex( 124, 0.4, Form( "<width> = %.2f#pm%.2f", w, ew) );
	l->Draw();

}
