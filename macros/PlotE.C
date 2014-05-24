void PlotE( Char_t *fname, UInt_t pBeId = 0, UInt_t pFeId = 0, UInt_t pCbc = 0, UInt_t color = 4 ){


	TH1F *h0 = new TH1F("Noise", ";% ;count", 100, 0, 0.2 );
	TFile *fin = new TFile( fname );
	TFile *fped = new TFile( "/Users/kirika/Work/CMS_UPGRADE/eps_calib/VCthScanCalibrationResult.root" );
	for( UInt_t i=0; i<254; i++ ){
		TString hname = Form( "h_%02d_%02d_%02d_%03d", pBeId, pFeId, pCbc, i );
		TH1F *h = (TH1F *) fin->Get( hname );
		TF1 *func = h->GetFunction( Form("f_%s", hname.Data() ) );
		h = (TH1F *) fped->Get( hname );
		TF1 *func0 = h->GetFunction( Form("f_%s", hname.Data() ) );
		h0->Fill( func->GetParameter(1) / ( func0->GetParameter(0) - func->GetParameter(0) ) );
	}
	h0->Draw();
}
