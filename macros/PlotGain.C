void PlotGain( Char_t *fname, Char_t *hname0, Char_t *option, UInt_t pCbc = 0, UInt_t color = 4 ){

	TH1F *h0 = new TH1F( hname0, ";V_{CTH} middle point pedestal subtracted;count", 180, -80, 0 );
	TFile *fin = new TFile( fname );
	TFile *fped = new TFile( "/Users/kirika/Work/CMS_UPGRADE/eps_calib/VCthScanCalibrationResult.root" );
	for( UInt_t i=0; i<254; i++ ){
		TString hname = Form( "h_00_%02d_%03d", pCbc, i );
		TH1F *h = (TH1F *) fin->Get( hname );
		TF1 *func = h->GetFunction( Form("f_%s", hname.Data() ) );
		h = (TH1F *) fped->Get( hname );
		TF1 *func0 = h->GetFunction( Form("f_%s", hname.Data() ) );
		h0->Fill( func->GetParameter(0) - func0->GetParameter(0) );
	}
	h0->Draw( option );
}
