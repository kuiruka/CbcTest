void PlotOffsets( Char_t *fname ){

	ifstream in;
	in.open( fname );

	char line[256];
	char name[256];
	int page, address, defv, val; 
	TH1F *h = new TH1F( "offsets", "; Offset register value;", 256, -0.5, 255.5 );
	while(!in.eof()){
		in.getline( line, 256 );
		if( line[0] != '*' && line[0] != ''){

			sscanf( line, "%s\t0x%02X\t0x%02X\t0x%02X\t0x%02X", name, &page, &address, &defv, &val );
			TString Name(name);
			if( !Name.Contains( "Mask" ) && !Name.Contains( "Dummy" ) && Name.Contains( "Channel" ) ){
				h->Fill( val );
			}
		}
	}
	in.close();
	h->SetLineWidth(2);
	h->Draw();
	gPad->Print( "Offset.png" );
}

