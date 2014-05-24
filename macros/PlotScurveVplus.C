void PlotScurveVplus( Char_t *VplusValues, UInt_t pBeId = 0, UInt_t pFeId = 0, UInt_t pCbc = 0, UInt_t color = 4 ){

	TString cVplusValues( VplusValues );
	TObjArray *Strings = cVplusValues.Tokenize( ":" );
	if( Strings->GetEntriesFast()){
		TIter iString(Strings);
		TObjString *os=0;
		Int_t j=0;
		while( (os = (TObjString *) iString() ) ){

			TFile *fin = new TFile( Form( "Vplus%s.root", os->GetString().Data() ) );

			for( UInt_t i=0; i<254; i++ ){
				TString hname = Form( "h_%02d_%02d_%02d_%03d", pBeId, pFeId, pCbc, i );
				TH1F *h = (TH1F *) fin->Get( hname );
				h->GetXaxis()->SetRangeUser( 50, 200 );
				h->SetLineColor(1);
				h->SetMarkerColor(1);
				h->SetTitle( Form( "FE(00),CBC(%02d); VCth; Rate", pCbc ) );
				TF1 *func = h->GetFunction( Form("f_%s", hname.Data() ) );
				func->SetLineColor(j+2);
				if( j == 0 && i == 0 ) h->Draw();
				else h->Draw("same");
			}
			j++;
		}
	}
}

