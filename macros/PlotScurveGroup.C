void PlotScurveGroup( Char_t *fname, UInt_t pBeId = 0, UInt_t pFeId = 0, UInt_t pCbc = 0, UInt_t pGroup = 0, UInt_t color = 4 ){

	TFile *fin = new TFile( fname );
	UInt_t cCh1(0);
	for( int i=0; i < 16; i++ ){
		cCh1 = i * 16 + pGroup*2;
		for( int j = cCh1; j < cCh1 + 2; j++ ){  
			if( j < 254 ){

				TString hname = Form( "h_%02d_%02d_%02d_%03d", pBeId, pFeId, pCbc, j );
				TH1F *h = (TH1F *) fin->Get( hname );
				h->GetXaxis()->SetRangeUser( 50, 170 );
				h->SetLineColor(1);
				h->SetMarkerColor(1);
				h->SetTitle( Form( "FE(00),CBC(%02d); VCth; Rate", pCbc ) );
				TF1 *func = h->GetFunction( Form("f_%s", hname.Data() ) );
				if( i == 0 ) h->Draw();
				else h->Draw("same");
			}
		}
	}
	TString cPadName( fname );
	cPadName.ReplaceAll( ".root", Form( "G%d.png", pGroup ) );
	gPad->Print( cPadName ); 
}

