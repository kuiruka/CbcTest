#include "SignalShapeAnalyser.h"
#include "Cbc/CbcRegInfo.h"
#include "Analysers.h"
#include <TGraphErrors.h>
#include "TF1.h"
#include <TH1F.h>
#include <TLegend.h>
#include <TGraphErrors.h>
#include <TFile.h>
#include <cmath>

namespace Analysers{

	SignalShapeAnalyser::SignalShapeAnalyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
			SignalShapeTestGroupMap *pGroupMap, const CbcRegMap *pCbcRegMap, 
			Bool_t pNegativeLogicCbc, const char *pSignalType, const char *pOutputDir, GUIFrame *pGUIFrame ):
		Analyser( pBeId, pNFe, pNCbc, pCbcRegMap, pNegativeLogicCbc, pOutputDir, pGUIFrame ),
		fSignalType( pSignalType ), 
		fGroupMap( pGroupMap ),
		fChannelList(0){

		}
	void   SignalShapeAnalyser::Initialise(){

		Analyser::Initialise();
		for( UInt_t i=0; i < fChannelList.size(); i++ ){

			delete fChannelList[i];
		}
		fChannelList.clear();
		SignalShapeTestGroupMap::iterator cIt = fGroupMap->begin(); 
		for( ; cIt != fGroupMap->end(); cIt++ ){

			SignalShapeTestGroup &cTestGroup = cIt->second;
			cTestGroup.ClearChannelList();
		}
		fSignalShapeData.clear();
		fGUIData.clear();
		TString cName = Form( "totalBE%u", fBeId ); 
		TObject *cObj = gROOT->FindObject( cName );
		if( cObj ) delete cObj;
		fHtotal = new TH1F( cName, "total", 256, -0.5, 255.5 ); 
		fHtotal->Sumw2();
		fHtotal->SetLineColor(kSpring);
		fHtotal->SetMarkerStyle( 20 );

		for( UInt_t cFeId=0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId< fNCbc; cCbcId++ ){

				cIt = fGroupMap->begin(); 
				for( ; cIt != fGroupMap->end(); cIt++ ){

					SignalShapeTestGroup &cTestGroup = cIt->second;
					for( UInt_t i=0; i<cTestGroup.size(); i++ ){

						UInt_t cChannelId = cTestGroup[i];
						TH1F *h = createScurveHistogram( cFeId, cCbcId, cChannelId );
						TGraphErrors *g = createSignalShapeGraph( cFeId, cCbcId, cChannelId );
						SignalShapeChannelInfo *cChannel = new SignalShapeChannelInfo( cFeId, cCbcId, cChannelId );
						SignalShapeChannelData cSignalShapeChannelData;
						cSignalShapeChannelData.SetHist(h);
						cSignalShapeChannelData.SetGraph(g);
						cChannel->SetData( cSignalShapeChannelData );
						fChannelList.push_back( cChannel );
						fSignalShapeData.AddChannel( cFeId, cCbcId, cChannelId, cChannel );

						if( fGUIFrame ){
							Channel<GUIChannelData> *cGUIChannel = new Channel<GUIChannelData>( cFeId, cCbcId, cChannelId );
							fGUIData.AddChannel( cFeId, cCbcId, cChannelId, cGUIChannel );
						}
						cTestGroup.AddChannel( cChannel ); 
					}
				}
			}
		}
		TString cHistName( "hDummyHist" );
		cObj = gROOT->FindObject( cHistName );
		if( cObj ) delete cObj;
		fDummyHist = new TH1F( cHistName, "Fit; VCth; Rate", 256, -0.5, 255.5 );
		fDummyHist->SetMinimum(0);
		fDummyHist->SetMaximum(1);
#ifdef __CBCDAQ_DEV__
		std::cout << "ScurveAnalyser::Initialise() done" << std::endl;
#endif
	}
	void   SignalShapeAnalyser::Configure( Double_t pCurrentTime ){

		fCurrentTime = pCurrentTime;
		resetHistograms();
	}
	int    SignalShapeAnalyser::FillHists( UInt_t pVcth, const Event *pEvent ){

		UInt_t cGroupId(0);
		SignalShapeTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return -1;

		UInt_t cNhit(0);
		UInt_t cFeId(0), cCbcId(0), cChannelId(0);

		const std::vector<SignalShapeChannelInfo *> *cChannelList = cTestGroup->GetChannelList(); 
		for( UInt_t k=0; k < cChannelList->size(); k++ ){

			SignalShapeChannelInfo *cChannel = cChannelList->at(k);

			cChannel->Id( cFeId, cCbcId, cChannelId );
			TH1F *h = cChannel->getData().Hist();

			const FeEvent *cFeEvent = pEvent->GetFeEvent( cFeId );
			if( cFeEvent == 0 ) return cNhit; 
			const CbcEvent *cCbcEvent = cFeEvent->GetCbcEvent( cCbcId );
			if( cCbcEvent == 0 ) return cNhit;
			bool cBitValue = cCbcEvent->DataBit( cChannelId );

			if( cBitValue ) {
				h->Fill( pVcth );
				cNhit++;
			}
		}
		fHtotal->Fill( pVcth );

		return cNhit;
	}
	void   SignalShapeAnalyser::FitHists( UInt_t pMin, UInt_t pMax ){

		UInt_t cGroupId(0);
		SignalShapeTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		GUIData::iterator cItGUIFe = fGUIData.begin();
		for( ; cItGUIFe != fGUIData.end(); cItGUIFe++ ){

			GUIFeInfo &cFeInfo = cItGUIFe->second;
			GUIFeInfo::iterator cItGUICbc = cFeInfo.begin();
			for( ; cItGUICbc != cFeInfo.end(); cItGUICbc++ ){

				GUICbcInfo &cCbcInfo = cItGUICbc->second;
				cCbcInfo.getData().GetDummyPad( cItGUIFe->first, cItGUICbc->first )->cd();
				//cCbcInfo.GetDummyPad()->Clear();
				fDummyHist->Draw();
			}
		}

		fMinVCth0 = 0xFF;
		fMaxVCth0 = 0x0;
		UInt_t cFeId(0), cCbcId(0), cChannelId(0);

		const std::vector<SignalShapeChannelInfo *> *cChannelList = cTestGroup->GetChannelList(); 
		for( UInt_t k=0; k < cChannelList->size(); k++ ){

			SignalShapeChannelInfo *cChannel = cChannelList->at(k);
			if( cChannel == 0 ) continue;

			cChannel->Id( cFeId, cCbcId, cChannelId );

			TH1F *h = cChannel->getData().Hist();
			if(h==0) continue;

			h->Divide( h, fHtotal, 1.0, 1.0, "B" );
			double cFirstNon0(0);
			double cFirst1(0);
			if( fNegativeLogicCbc ){
				for( Int_t cBin = 1; cBin <= h->GetNbinsX(); cBin++ ){
					double cContent = h->GetBinContent( cBin );
					if( !cFirstNon0	){
						if( cContent ) cFirstNon0 = h->GetBinCenter(cBin);
					}
					else if( cContent == 1 ) {
						cFirst1 = h->GetBinCenter(cBin); 
						break;
					}
				}
			}
			else{
				for( Int_t cBin = h->GetNbinsX(); cBin >=1; cBin-- ){
					double cContent = h->GetBinContent( cBin );
					if( !cFirstNon0	){
						if( cContent ) cFirstNon0 = h->GetBinCenter(cBin);
					}
					else if( cContent == 1 ) {
						cFirst1 = h->GetBinCenter(cBin); 
						break;
					}
				}
			}
			double cMid = ( cFirst1 + cFirstNon0 ) * 0.5;
			double cWidth = ( cFirst1 - cFirstNon0 ) * 0.5;

			TString fname = Form( "f_%s", h->GetName() ); 
			TObject *cObj = gROOT->FindObject( fname );
			if( cObj ) delete cObj;
			TF1 *f = new TF1( fname, MyErf, pMin, pMax, 2 ); 
			f->SetParameters( cMid, cWidth );
			//Option S is for TFitResultPtr
			//TFitResultPtr cFitR = h->Fit( fname, "RSLQ0" ); 
			fGUIData.getCbcInfo( cFeId, cCbcId )->getData().GetDummyPad( cFeId, cCbcId )->cd();
			TFitResultPtr cFitR = h->Fit( fname, "RSLQ", "same" ); 
			//			TFitResultPtr cFitR = h->Fit( fname, "RSLQ" ); 
			int cStatus = int( cFitR ); 
			if( cStatus != 0 ) {
				std::cout << "Fit failed for FE " <<  cChannel->FeId() << " CBC " << cChannel->CbcId() 
					<< " Channel " << cChannel->ChannelId() << " status " << cStatus << " "
					<< gDirectory->GetName() << " " << h->GetName() << std::endl;
			}

			double cVCth0 = f->GetParameter( 0 ); 
			double cVCth0Error = f->GetParError( 0 ); 
			double cVCth0Width = f->GetParameter( 1 ); 
			double cVCth0WidthError = f->GetParError( 1 ); 
			cChannel->getData().SetVCth0( (float) cVCth0, (float) cVCth0Error );
			cChannel->getData().SetVCth0Width( (float) cVCth0Width, (float) cVCth0WidthError );
			fMinVCth0 = cVCth0 < (double)fMinVCth0 ? (UInt_t)cVCth0 : fMinVCth0;
			fMaxVCth0 = (double)fMaxVCth0 < cVCth0 ? (UInt_t)cVCth0 : fMaxVCth0;

			TGraphErrors *g = cChannel->getData().Graph();
			Int_t cNpoints = g->GetN();	
			g->SetPoint( cNpoints, fCurrentTime, cChannel->GetData().VCth0() ); 
			g->SetPointError( cNpoints, 0, cChannel->GetData().VCth0Width() ); 
		}
		fHtotal->Divide( fHtotal, fHtotal, 1.0, 1.0, "B" );
		
		cItGUIFe = fGUIData.begin();
		for( ; cItGUIFe != fGUIData.end(); cItGUIFe++ ){

			GUIFeInfo &cFeInfo = cItGUIFe->second;
			GUIFeInfo::iterator cItGUICbc = cFeInfo.begin();
			for( ; cItGUICbc != cFeInfo.end(); cItGUICbc++ ){

				GUICbcInfo &cCbcInfo = cItGUICbc->second;
				cCbcInfo.getData().GetDummyPad( cItGUIFe->first, cItGUICbc->first )->Update();
			}
		}

	}
	TH1F  *SignalShapeAnalyser::createScurveHistogram( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannelId ){

		unsigned int color= pChannelId/16%8 + 2;
		unsigned int marker = pChannelId % 2 + pChannelId/128 * 4 + 20;
		TString cName = Form( "h_%02u_%02u_%02u_%03u", fBeId, pFeId, pCbcId, pChannelId );
		//gROOT->Delete( cName + ";*" ); 
		TObject *cObj = gROOT->FindObject( cName ); 
		if( cObj ) delete cObj;
		TH1F *h = new TH1F(  cName, 
				Form( "BE(%02u) FE(%02u) CBC(%02u) Channel(%03u); VCth; Count", fBeId, pFeId, pCbcId, pChannelId ), 
				256, -0.5, 255.5 );
		//fHistList.push_back( h );
		h->Sumw2();
		h->SetMarkerColor( color );
		h->SetLineColor( color );
		h->SetMarkerStyle( marker );
		//std::cout << "Created histogram = " << h << std::endl;
		return h;	
	}
	TGraphErrors  *SignalShapeAnalyser::createSignalShapeGraph( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannelId ){

		unsigned int color= pChannelId/16%8 + 2;
		unsigned int marker = pChannelId % 2 + pChannelId/128 * 4 + 20;
		TString cName = Form( "g_%02u_%02u_%02u_%03u", fBeId, pFeId, pCbcId, pChannelId );
		//gROOT->Delete( cName + ";*" ); 
		TObject *cObj = gROOT->FindObject( cName ); 
		if( cObj ) delete cObj;
		TGraphErrors *g = new TGraphErrors();  
		g->SetName( cName );
		g->SetMarkerColor( color );
		g->SetLineColor( color );
		g->SetMarkerStyle( marker );
		//std::cout << "Created graph = " << g << std::endl;
		return g;	
	}
	void  SignalShapeAnalyser::resetHistograms(){

		fHtotal->Reset();

		UInt_t cGroupId(0);
		SignalShapeTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		const std::vector<SignalShapeChannelInfo *> *cChannelList = cTestGroup->GetChannelList(); 
		for( UInt_t k=0; k < cChannelList->size(); k++ ){

			SignalShapeChannelInfo *cChannel = cChannelList->at(k);
			cChannel->getData().ResetHist();
		}
	}
	void SignalShapeAnalyser::DrawHists(){

		UInt_t cGroupId(0);
		SignalShapeTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		SignalShapeData::iterator cItFe = fSignalShapeData.begin();
		for( ; cItFe != fSignalShapeData.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			SignalShapeFeInfo &cFeInfo = cItFe->second;
			SignalShapeFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				SignalShapeCbcInfo &cCbcInfo = cItCbc->second;
				TPad *cPad = fGUIData.getCbcInfo( cFeId, cCbcId )->getData().GetScurveHistogramDisplayPad( cGroupId );	
				if( cPad ){
					cPad->cd();
					cPad->Clear();
					//TString option("e0p0");
					TString option("hist");
					TString cHtotalName =  Form( "h_FE%dCBC%dG%03d", cFeId, cCbcId, cGroupId );
					int cTotalColor = 1;

					TObject *cObj = gROOT->FindObject( cHtotalName );
					if( cObj ) delete cObj;
					TH1F *cHtotal = (TH1F *) fHtotal->Clone( cHtotalName );
					cHtotal->SetLineColor(cTotalColor);
					TString cYaxisTitle("Events");
					if( cHtotal->GetMaximum() < 1.5 ) cYaxisTitle = "Rate";
					cHtotal->GetXaxis()->SetLabelSize( 0.06 );
					cHtotal->GetYaxis()->SetLabelSize( 0.05 );
					cHtotal->GetXaxis()->SetTitleSize( 0.06 );
					cHtotal->GetYaxis()->SetTitleSize( 0.05 );
					cHtotal->GetXaxis()->SetTitleOffset( 0.85 );
					cHtotal->GetYaxis()->SetTitleOffset( 0.4 );
					cHtotal->GetYaxis()->SetTitle( cYaxisTitle );
					cHtotal->Draw( option );

					TLegend *cLegend0 = new TLegend( 0.85, 0.15, 0.98, 0.95);
					TLegend *cLegend1 = new TLegend( 0.72, 0.15, 0.85, 0.95);
					cLegend0->SetFillColor(0);
					cLegend1->SetFillColor(0);
					cLegend0->SetLineColor(0);
					cLegend1->SetLineColor(0);

					option = "samese0p0";

					UInt_t cChannelId(0);
					double cTotalVCth0(0);
					double cTotalVCth0Error(0);
					double Nch(0);

					for( UInt_t i=0; i < cTestGroup->size(); i++ ){

						cChannelId = cTestGroup->at(i);
						SignalShapeCbcInfo::iterator cItChannel = cCbcInfo.find( cChannelId );
						if( cItChannel == cCbcInfo.end() ) continue;

						SignalShapeChannelInfo *cChannel = cItChannel->second;
						TH1F *h = cChannel->getData().Hist(); 
						h->Draw( option );
						TF1 *cFunc = (TF1 *) h->GetListOfFunctions()->Last();
						double cVCth0(0);
						double cVCthError(0);
						if( cFunc ) {
							cVCth0 = cFunc->GetParameter(0);
							cVCthError = cFunc->GetParError(0);
							cTotalVCth0 += cVCth0;
							cTotalVCth0Error += cVCth0 * cVCth0;
							Nch++;
						}
						else{
							cVCth0 = cChannel->GetData().VCth0();
						}
						TString label( Form( "[%02X] %05.1f#pm%5.2f", cChannelId, cVCth0, cVCthError )); 
						TLegend *cL = cChannelId %2 ? cLegend0 : cLegend1;
						cL->AddEntry( h, label, "p" ); 
					}
					TString cStringTotalVCth0;
					if( Nch ){
						cTotalVCth0 /= Nch;
						cTotalVCth0Error = sqrt( ( cTotalVCth0Error - Nch * cTotalVCth0*cTotalVCth0 ) / Nch );
						cStringTotalVCth0 = Form( "  VCth mid. point = %05.1f#pm%5.2f (0x%02X#pm%02X)", cTotalVCth0, cTotalVCth0Error, (Int_t)cTotalVCth0, (Int_t)cTotalVCth0Error );
					}
					TString cHtotalTitle =  Form( "FE(%d) CBC(%d) Group(%03X) %s; VCth", 
							cFeId, cCbcId, cGroupId, cStringTotalVCth0.Data() );
					cHtotal->SetTitle( cHtotalTitle );

					cLegend0->Draw();
					cLegend1->Draw();
					cPad->Update();
				}
			}
		}
	}
	void SignalShapeAnalyser::DrawGraphs(){

		UInt_t cGroupId(0);
		SignalShapeTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		SignalShapeData::iterator cItFe = fSignalShapeData.begin();
		for( ; cItFe != fSignalShapeData.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			SignalShapeFeInfo &cFeInfo = cItFe->second;
			SignalShapeFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				SignalShapeCbcInfo &cCbcInfo = cItCbc->second;
				TPad *cPad = fGUIData.getCbcInfo( cFeId, cCbcId )->getData().GetSignalShapeGraphDisplayPad( cGroupId );	
				if( cPad ){
					cPad->cd();
					//TString option("e0p0");

					TString cHname =  Form( "hg_FE%dCBC%dG%03d", cFeId, cCbcId, cGroupId );
					TObject *cObj = gROOT->FindObject( cHname );
					if( cObj ) delete cObj;

					TH1F *h = new TH1F( cHname, ";Time [ns]; mid VCTH point", 1, -30, 360 );
					double cMin(0);
					double cMax(150);
					if( !fNegativeLogicCbc ){
						cMin = 100;
						cMax = 255;
					}
					h->SetMaximum( cMax );
					h->SetMinimum( cMin );
					h->Draw();
					gPad->SetGrid();
					//TString option("C*");
					TString option("P");

					UInt_t cChannelId(0);

					for( UInt_t i=0; i < cTestGroup->size(); i++ ){

						cChannelId = cTestGroup->at(i);
						SignalShapeCbcInfo::iterator cItChannel = cCbcInfo.find( cChannelId );
						if( cItChannel == cCbcInfo.end() ) continue;

						SignalShapeChannelInfo *cChannel = cItChannel->second;
						TGraphErrors *g = cChannel->getData().Graph(); 
						g->Draw( option );
					}
					cPad->Update();
				}
			}
		}
	}
	void SignalShapeAnalyser::SetOffsets(){

		for( UInt_t i=0; i<fChannelList.size(); i++ ){
			UInt_t cFeId = fChannelList[i]->FeId();
			UInt_t cCbcId = fChannelList[i]->CbcId();
			UInt_t cChannel = fChannelList[i]->ChannelId();
			UInt_t cOffset = fCbcRegMap->GetValue( cFeId, cCbcId, 1, cChannel+1 );
			//std::cout << "Fe " << cFeId << ", Cbc " << cCbcId << ", Offset " << cOffset << std::endl;
			fChannelList[i]->getData().SetOffset( cOffset );
		}
	}
	void   SignalShapeAnalyser::SetSignalShapeGraphDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){

		UInt_t cNpad = fGroupMap->size()+1;
		UInt_t cNcol = 3;
		UInt_t cNrow = cNpad/cNcol;
		if( cNpad%cNcol ) cNrow++; 
		pPad->Divide( cNcol, cNrow );

		UInt_t i(0);
		SignalShapeTestGroupMap::iterator cIt = fGroupMap->begin();
		for( ; cIt != fGroupMap->end(); cIt++ ){
			UInt_t cGroupId = cIt->first;
			fGUIData.getCbcInfo( pFeId, pCbcId )->getData().SetSignalShapeGraphDisplayPad( cGroupId, (TPad *)pPad->GetPad(++i) );
		}
	}
	void   SignalShapeAnalyser::SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){

		UInt_t cNpad = fGroupMap->size()+1;
		UInt_t cNcol = 3;
		UInt_t cNrow = cNpad/cNcol;
		if( cNpad%cNcol ) cNrow++; 
		pPad->Divide( cNcol, cNrow );

		UInt_t i(0);
		SignalShapeTestGroupMap::iterator cIt = fGroupMap->begin();
		for( ; cIt != fGroupMap->end(); cIt++ ){
			UInt_t cGroupId = cIt->first;
			fGUIData.getCbcInfo( pFeId, pCbcId )->getData().SetScurveHistogramDisplayPad( cGroupId, (TPad *)pPad->GetPad(++i) );
		}
		fGUIData.getCbcInfo( pFeId, pCbcId )->getData().SetDummyPad( (TPad *) pPad->GetPad(cNcol*cNrow) );
	}

	UInt_t SignalShapeAnalyser::GetOffset( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannelId ){ 

		const SignalShapeChannelInfo *cC = fSignalShapeData.GetChannel( pFeId, pCbcId, pChannelId ); 
#ifdef __CBCDAQ_DEV__
		std::cout << "ChannelTest [" << cC->ChannelId() << "] " << pChannelId << "  Offset = " << cC->GetData().Offset() << std::endl; 
#endif
		if( cC == 0 ) return 0;
		return cC->GetData().Offset();
	}
	void SignalShapeAnalyser::SaveSignalShape(){

		TFile fout( Form( "%s/SignalShape%s.root", fOutputDir.Data(), fSignalType.Data() ), "UPDATE" );

		SignalShapeData::iterator cItFe = fSignalShapeData.begin();
		for( ; cItFe != fSignalShapeData.end(); cItFe++ ){

			SignalShapeFeInfo &cFeInfo = cItFe->second;
			SignalShapeFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				SignalShapeCbcInfo &cCbcInfo = cItCbc->second;

				SignalShapeCbcInfo::iterator cItChannel = cCbcInfo.begin();
				for( ; cItChannel != cCbcInfo.end(); cItChannel++ ){

					SignalShapeChannelInfo *cChannelInfo = cItChannel->second;
					SignalShapeChannelData &cData = cChannelInfo->getData();
					TGraphErrors *cGraph = cData.Graph();

					cGraph->Write(cGraph->GetName(), TObject::kOverwrite );
				}
			}
		}
		fout.Close();
	}
}
