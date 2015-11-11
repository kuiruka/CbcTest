#include "ScurveAnalyser.h"
#include "TString.h"
#include "TCanvas.h"
#include "TF1.h"
#include <TH1F.h>
#include <TFile.h>
#include <TBinomialEfficiencyFitter.h>
#include <TLegend.h>
#include <TThread.h>
#include <TGraphErrors.h>
#include "Cbc/CbcRegInfo.h"
#include "Cbc/TestGroup.h"
#include "Analysers.h"
#include <cmath>

namespace Analysers{

	const TGraphErrors *CalibrationCbcData::GetGraphVplusVCth0( UInt_t pGroupId )const{

		std::map<UInt_t, TGraphErrors *>::const_iterator cIt = fGraphVplusVCth0.find( pGroupId ); 
		if( cIt == fGraphVplusVCth0.end() ) return 0;
		return cIt->second;
	}

	TGraphErrors *CalibrationCbcData::GetGraphVplusVCth0( UInt_t pGroupId ){
		std::map<UInt_t, TGraphErrors *>::iterator cIt = fGraphVplusVCth0.find( pGroupId ); 
		if( cIt == fGraphVplusVCth0.end() ) return 0;
		return cIt->second;
	}
	void CalibrationCbcData::AddGraphVplusVCth0( UInt_t pGroup, TGraphErrors *pGraph ){

		fGraphVplusVCth0.insert( std::pair< UInt_t, TGraphErrors *>( pGroup, pGraph ) );

	}
	ScurveAnalyser::ScurveAnalyser( 
			UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
			CalibrationTestGroupMap *pGroupMap, const CbcRegMap *pCbcRegMap, 
			Bool_t pNegativeLogicCbc, UInt_t pTargetVCth, const char *pOutputDir, GUIFrame *pGUIFrame ):
		Analyser( pBeId, pNFe, pNCbc, pCbcRegMap, pNegativeLogicCbc, pOutputDir, pGUIFrame ),
		fGroupMap(pGroupMap),  
		fTargetVCth(pTargetVCth), 
		fCurrentTargetBit(8),
		fChannelList(0){

		}
	void ScurveAnalyser::Initialise(){

		Analyser::Initialise();
		for( UInt_t i=0; i < fChannelList.size(); i++ ){

			delete fChannelList[i];
		}
		fChannelList.clear();
		CalibrationTestGroupMap::iterator cIt = fGroupMap->begin(); 
		for( ; cIt != fGroupMap->end(); cIt++ ){

			CalibrationTestGroup &cTestGroup = cIt->second;

			cTestGroup.ClearChannelList();
		}
		fResult.clear();
		fGUIData.clear();
		fNthVplusPoint = 0;	

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

					CalibrationTestGroup &cTestGroup = cIt->second;

					for( UInt_t i=0; i<cTestGroup.size(); i++ ){

						UInt_t cChannelId = cTestGroup[i];
						TH1F *h = createScurveHistogram( cFeId, cCbcId, cChannelId );
						CalibrationChannelInfo *cChannel = new CalibrationChannelInfo( cFeId, cCbcId, cChannelId );
						CalibrationChannelData cCalibChannelData;
						cCalibChannelData.SetHist(h);
						cChannel->SetData( cCalibChannelData );
						fChannelList.push_back( cChannel );
						fResult.AddChannel( cFeId, cCbcId, cChannelId, cChannel );
						if( fGUIFrame ){
							Channel<GUIChannelData> *cGUIChannel = new Channel<GUIChannelData>( cFeId, cCbcId, cChannelId );
							fGUIData.AddChannel( cFeId, cCbcId, cChannelId, cGUIChannel );
						}
						cTestGroup.AddChannel( cChannel ); 
					}

					TString cName = Form("gBe%uFe%uCbc%uGroup%u", fBeId, cFeId, cCbcId, cIt->first );
					delete gROOT->FindObject( cName );
					TGraphErrors *cG = new TGraphErrors( cTestGroup.size() * 10 );
					cG->SetName( cName );
					CalibrationCbcInfo *cCbcInfo = fResult.GetCbcInfo( cFeId, cCbcId );	
					cCbcInfo->GetData().AddGraphVplusVCth0( cIt->first, cG );
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

	void ScurveAnalyser::SetOffsets(){

		for( UInt_t i=0; i<fChannelList.size(); i++ ){
			UInt_t cFeId = fChannelList[i]->FeId();
			UInt_t cCbcId = fChannelList[i]->CbcId();
			UInt_t cChannel = fChannelList[i]->ChannelId();
			UInt_t cOffset = fCbcRegMap->GetReadValue( cFeId, cCbcId, 1, cChannel+1 );
			//std::cout << "Fe " << cFeId << ", Cbc " << cCbcId << ", Offset " << cOffset << std::endl;
			fChannelList[i]->GetData().SetOffset( cOffset );
			fChannelList[i]->GetData().SetNextOffset( cOffset );
		}
	}
	void ScurveAnalyser::SetOffsets( UInt_t pInitalOffset ){

		for( UInt_t i=0; i < fChannelList.size(); i++ ){
			if( fChannelList[i] == 0 ) continue;
			fChannelList[i]->GetData().SetOffset( pInitalOffset );
			fChannelList[i]->GetData().SetNextOffset( pInitalOffset );
		}
	}
	void ScurveAnalyser::Configure( Int_t pType, Int_t pOffsetTargetBit ){

		fScanType = pType;
		resetHistograms();
		if( -1 <= pOffsetTargetBit && pOffsetTargetBit <= 7 ) updateOffsets();
		fCurrentTargetBit = pOffsetTargetBit;

	}
	int ScurveAnalyser::FillGraphVplusVCth0(){

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return -1;

		CalibrationResult::iterator cItFe = fResult.begin();
		for( ; cItFe != fResult.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			CalibrationFeInfo &cFeInfo = cItFe->second;
			CalibrationFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				CalibrationCbcInfo &cCbcInfo = cItCbc->second;

				TGraphErrors *cG = cCbcInfo.GetData().GetGraphVplusVCth0(cGroupId);

				const std::vector<CalibrationChannelInfo *> *cChannelList = cTestGroup->GetChannelList();
				UInt_t j=0;
				for( UInt_t i=0; i < cChannelList->size(); i++ ){
					CalibrationChannelInfo *cChannel = cChannelList->at(i);
					if( !( cChannel->FeId() == cFeId && cChannel->CbcId() == cCbcId ) )continue; 
					UInt_t cVplus = fCbcRegMap->GetReadValue( cFeId, cCbcId, 0, 0x0B );
					cG->SetPoint( fNthVplusPoint+j, cChannel->GetData().VCth0(), cVplus ); 
					cG->SetPointError( fNthVplusPoint+j, cChannel->GetData().VCth0Error(), 0 ); 
					//					std::cout << "SetPoint [" << fNthVplusPoint+j << "] for Channel " << cChannel->ChannelId() << " Vplus " << cVplus << " VCth0 = " << cChannel->VCth0() << std::endl;
					j++;
				}
			}
		}
		fNthVplusPoint += cTestGroup->size();
		return 0;
	}
	void ScurveAnalyser::DrawVplusVCth0(){

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		CalibrationResult::iterator cItFe = fResult.begin();
		for( ; cItFe != fResult.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			CalibrationFeInfo &cFeInfo = cItFe->second;
			CalibrationFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				CalibrationCbcInfo &cCbcInfo = cItCbc->second;
				TPad *cPad = fGUIData.GetCbcInfo( cFeId, cCbcId )->GetData().GetVplusVsVCth0GraphDisplayPad();	
				if( cPad ){

					cPad->cd();

					TGraphErrors *cG = cCbcInfo.GetData().GetGraphVplusVCth0(cGroupId);
					TString cOption( "P" );
					TString cHistName = Form( "VplusScanBE%uFE%uCBC%u", fBeId, cFeId, cCbcId);

					if( ! gROOT->FindObject( cHistName )  ) {
						TH1F *h = new TH1F( cHistName, Form( "VplusScan BE %u FE %u CBC %u;VCth;Vplus", fBeId, cFeId, cCbcId ), 1, 0, 260 ); 
						h->SetMaximum( 260 );
						h->GetXaxis()->SetLabelSize( 0.05 );
						h->GetYaxis()->SetLabelSize( 0.04 );
						h->GetXaxis()->SetTitleSize( 0.05 );
						h->GetYaxis()->SetTitleSize( 0.05 );
						h->GetXaxis()->SetTitleOffset( 1.0 );
						h->GetYaxis()->SetTitleOffset( 1.0 );
					}
					if( cGroupId == 0 ){
						cPad->Clear();
						TH1F *h = (TH1F *) gROOT->FindObject( cHistName );
						h->Draw();
					}
					cG->SetMarkerStyle(20);
					cG->SetMarkerColor( cGroupId + 1 );
					cG->SetLineColor( cGroupId + 1 );
					cG->Draw( cOption );
					cPad->Update();
				}
			}
		}
	}
	void ScurveAnalyser::PrintVplusVsVCth0DisplayPads(){

		GUIData::iterator cItFe = fGUIData.begin();
		for( ; cItFe != fGUIData.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			GUIFeInfo &cFeInfo = cItFe->second;
			GUIFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				GUICbcInfo &cCbcInfo = cItCbc->second;
				TPad *cPad = cCbcInfo.GetData().GetVplusVsVCth0GraphDisplayPad();	
				if( cPad ){
					cPad->cd();
					//cPad->Update();
					cPad->Print( Form( "%s/FE%uCBC%u_VplusScan.eps", fOutputDir.Data(), cFeId, cCbcId ) );
				}
			}
		}
	}
	int ScurveAnalyser::FillHists( UInt_t pVcth, const Event *pEvent ){

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return -1;

		UInt_t cNhit(0);
		UInt_t cFeId(0), cCbcId(0), cChannelId(0);

		const std::vector<CalibrationChannelInfo *> *cChannelList = cTestGroup->GetChannelList(); 
		for( UInt_t k=0; k < cChannelList->size(); k++ ){

			CalibrationChannelInfo *cChannel = cChannelList->at(k);

			cChannel->Id( cFeId, cCbcId, cChannelId );
			TH1F *h = cChannel->GetData().Hist();

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
	void ScurveAnalyser::FitHists( UInt_t pMin, UInt_t pMax ){
		
		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		GUIData::iterator cItGUIFe = fGUIData.begin();
		for( ; cItGUIFe != fGUIData.end(); cItGUIFe++ ){

			GUIFeInfo &cFeInfo = cItGUIFe->second;
			GUIFeInfo::iterator cItGUICbc = cFeInfo.begin();
			for( ; cItGUICbc != cFeInfo.end(); cItGUICbc++ ){

				GUICbcInfo &cCbcInfo = cItGUICbc->second;
//				TPad *cPad = cCbcInfo.GetData().GetDummyPad( cItGUIFe->first, cItGUICbc->first );
				TPad *cPad = cCbcInfo.GetData().GetDummyPad();
				if( cPad ){
					cPad->cd();
					//cCbcInfo.GetDummyPad()->Clear();
					fDummyHist->Draw();
				}
			}
		}

		TFile fout( Form( "%s/%s.root", fOutputDir.Data(), getScanId().Data() ), "UPDATE" );
		fMinVCth0 = 0xFF;
		fMaxVCth0 = 0x0;
		UInt_t cFeId(0), cCbcId(0), cChannelId(0);

		const std::vector<CalibrationChannelInfo *> *cChannelList = cTestGroup->GetChannelList(); 
		for( UInt_t k=0; k < cChannelList->size(); k++ ){

			CalibrationChannelInfo *cChannel = cChannelList->at(k);
			if( cChannel == 0 ) continue;

			cChannel->Id( cFeId, cCbcId, cChannelId );
			TH1F *h = cChannel->GetData().Hist();
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
			if( fGUIFrame ){	
			//			TPad *cPad = fGUIData.GetCbcInfo( cFeId, cCbcId )->GetData().GetDummyPad( cFeId, cCbcId );
				TPad *cPad = fGUIData.GetCbcInfo( cFeId, cCbcId )->GetData().GetDummyPad();
				if( cPad ){
					cPad->cd();
				}
			}
			//Option S is for TFitResultPtr
			//TFitResultPtr cFitR = h->Fit( fname, "RSLQ0" ); 
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
			cChannel->GetData().SetVCth0( (float)cVCth0, (float) cVCth0Error );
			fMinVCth0 = cVCth0 < (double)fMinVCth0 ? (UInt_t)cVCth0 : fMinVCth0;
			fMaxVCth0 = (double)fMaxVCth0 < cVCth0 ? (UInt_t)cVCth0 : fMaxVCth0;
			h->Write(h->GetName(), TObject::kOverwrite );
		}
		fHtotal->Write (fHtotal->GetName(), TObject::kOverwrite );
		fHtotal->Divide( fHtotal, fHtotal, 1.0, 1.0, "B" );
		fout.Close();
		setNextOffsets();

		cItGUIFe = fGUIData.begin();
		for( ; cItGUIFe != fGUIData.end(); cItGUIFe++ ){

			GUIFeInfo &cFeInfo = cItGUIFe->second;
			GUIFeInfo::iterator cItGUICbc = cFeInfo.begin();
			for( ; cItGUICbc != cFeInfo.end(); cItGUICbc++ ){

				GUICbcInfo &cCbcInfo = cItGUICbc->second;
				//				TPad *cPad = cCbcInfo.GetData().GetDummyPad( cItGUIFe->first, cItGUICbc->first );
				TPad *cPad = cCbcInfo.GetData().GetDummyPad();
				if(cPad) cPad->Update();
			}
		}
	}
	void ScurveAnalyser::DrawHists(){

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		CalibrationResult::iterator cItFe = fResult.begin();
		for( ; cItFe != fResult.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			CalibrationFeInfo &cFeInfo = cItFe->second;
			CalibrationFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				CalibrationCbcInfo &cCbcInfo = cItCbc->second;
				TPad *cPad = fGUIData.GetCbcInfo( cFeId, cCbcId )->GetData().GetScurveHistogramDisplayPad( cGroupId );	
				if( cPad ){
					cPad->cd();
					cPad->Clear();
					//TString option("e0p0");
					TString option("hist");
					TString cHtotalName =  Form( "h_FE%dCBC%dG%03d%s", cFeId, cCbcId, cGroupId, getScanId().Data() );
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
						CalibrationCbcInfo::iterator cItChannel = cCbcInfo.find( cChannelId );
						if( cItChannel == cCbcInfo.end() ) continue;

						CalibrationChannelInfo *cChannel = cItChannel->second;
						TH1F *h = cChannel->GetData().Hist(); 
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
						UInt_t cOffset = cChannel->GetData().Offset();
						TString label( Form( "[%02X] %05.1f#pm%5.2f   %02X", cChannelId, cVCth0, cVCthError, cOffset )); 
						TLegend *cL = cChannelId %2 ? cLegend0 : cLegend1;
						cL->AddEntry( h, label, "p" ); 
					}
					TString cStringTotalVCth0;
					if( Nch ){
						cTotalVCth0 /= Nch;
						cTotalVCth0Error = sqrt( ( cTotalVCth0Error - Nch * cTotalVCth0*cTotalVCth0 ) / Nch );
						cStringTotalVCth0 = Form( "  VCth mid. point = %05.1f#pm%5.2f (0x%02X#pm%02X)", cTotalVCth0, cTotalVCth0Error, (Int_t)cTotalVCth0, (Int_t)cTotalVCth0Error );
					}
					UInt_t cVplus = fCbcRegMap->GetReadValue( cFeId, cCbcId, 0, 0x0B );
					TString cHtotalTitle =  Form( "FE(%d) CBC(%d) Group(%03X) Vplus = 0x%02X Target VCth = 0x%02X %s%s; VCth", 
							cFeId, cCbcId, cGroupId, cVplus, fTargetVCth, getScanType().Data(), cStringTotalVCth0.Data() );
					cHtotal->SetTitle( cHtotalTitle );

					cLegend0->Draw();
					cLegend1->Draw();
					cPad->Update();
				}
			}
		}
	}
	void ScurveAnalyser::PrintScurveHistogramDisplayPads(){

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		GUIData::iterator cItFe = fGUIData.begin();
		for( ; cItFe != fGUIData.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			GUIFeInfo &cFeInfo = cItFe->second;
			GUIFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				GUICbcInfo &cCbcInfo = cItCbc->second;
				TPad *cPad = cCbcInfo.GetData().GetScurveHistogramDisplayPad( cGroupId );	
				if( cPad ){
					cPad->cd();
					TString cRegInfo;
					if( fScanType == SINGLEVCTHSCAN ){

						UInt_t cVplus = fCbcRegMap->GetReadValue( cFeId, cCbcId, 0, 0x0B );
						UInt_t cPage(0x00), cAddr(0x0F);
						UInt_t tmp = fCbcRegMap->GetReadValue( cFeId, cCbcId, cPage, cAddr );
						UInt_t cMask = 1 << 6;
						tmp &= cMask;
						UInt_t cTPEnable = tmp == 0 ? 0 : 1;
						UInt_t cTPPot = fCbcRegMap->GetReadValue( cFeId, cCbcId, 0, 0x0D );
						cRegInfo = Form( "Vplus%02XTPEnable%dPot%02X", cVplus, cTPEnable, cTPPot );
					}
					cPad->Print( Form( "%s/%s_FE%uCBC%uTestGroup%d%s.eps", fOutputDir.Data(), getScanId().Data(), cFeId, cCbcId, cGroupId, cRegInfo.Data() ) );
				}
			}
		}
	}
	UInt_t ScurveAnalyser::GetOffset( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannelId ){ 

		CalibrationChannelInfo *cC = fResult.GetChannel( pFeId, pCbcId, pChannelId ); 
#ifdef __CBCDAQ_DEV__
		std::cout << "ChannelTest [" << cC->ChannelId() << "] " << pChannelId << "  Offset = " << cC->GetData().Offset() << std::endl; 
#endif
		if( cC == 0 ) return 0;
		return cC->GetData().Offset();
	}
	UInt_t ScurveAnalyser::GetVplus( UInt_t pFeId, UInt_t pCbcId ){

		UInt_t cVplus(0);
		CalibrationCbcInfo *cCbcInfo = fResult.GetCbcInfo( pFeId, pCbcId );	
		if( cCbcInfo ){
			cVplus = cCbcInfo->GetData().Vplus();
		}	
		return cVplus;
	}
	void ScurveAnalyser::DumpResult(){

		for( UInt_t cFeId=0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId< fNCbc; cCbcId++ ){

				CalibrationCbcInfo *cCbcInfo = fResult.GetCbcInfo( cFeId, cCbcId );	
				if( cCbcInfo == 0 ) return;

				printf( "FE: %d CBC: %d Vpluse: %u\n", cFeId, cCbcId, cCbcInfo->GetData().Vplus() );

				CalibrationCbcInfo::const_iterator cIt = cCbcInfo->begin();
				for( ; cIt != cCbcInfo->end(); cIt++ ){
					CalibrationChannelInfo *cChannel = cIt->second;
					printf( "FE: %d, CBC: %d Channel: %03d Offset: %2X VCth0: %6.2f VCth0Error: %7.3f\n", 
							cFeId, cCbcId, cChannel->ChannelId(), cChannel->GetData().Offset(), cChannel->GetData().VCth0(), cChannel->GetData().VCth0Error() );
				}
			}
		}
	}
	void ScurveAnalyser::SetVplus(){

		for( UInt_t cFeId=0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId< fNCbc; cCbcId++ ){
				UInt_t cVplus(0);
				Int_t cN(0);
				Float_t cSum(0);
				CalibrationCbcInfo *cCbcInfo = fResult.GetCbcInfo( cFeId, cCbcId );	
				std::map<UInt_t, TGraphErrors *> *cMap = cCbcInfo->GetData().GetGraphVplusVCth0();	
				std::map<UInt_t, TGraphErrors *>::iterator cIt = cMap->begin();
				for( ; cIt != cMap->end(); cIt++ ){
					TGraphErrors *cG = cIt->second; 
					//cG->Fit( "pol1", "Q0" );
					cG->Fit( "pol1", "Q", "", 1, 0xFF );
					cSum += cG->GetFunction( "pol1" )->Eval( fTargetVCth ); 
					cN++;
				}
				if( cN != 0 ) cVplus = (UInt_t) ( cSum / cN );
				cCbcInfo->GetData().SetVplus( cVplus );
			}
		}
	}
	void ScurveAnalyser::SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){

		UInt_t cNpad = fGroupMap->size()+1;
		UInt_t cNcol = 3;
		UInt_t cNrow = cNpad/cNcol;
		if( cNpad%cNcol ) cNrow++; 
		pPad->Divide( cNcol, cNrow );

		UInt_t i(0);
		CalibrationTestGroupMap::iterator cIt = fGroupMap->begin();
		for( ; cIt != fGroupMap->end(); cIt++ ){
			UInt_t cGroupId = cIt->first;
			fGUIData.GetCbcInfo( pFeId, pCbcId )->GetData().SetScurveHistogramDisplayPad( cGroupId, (TPad *)pPad->GetPad(++i) );
		}
		fGUIData.GetCbcInfo( pFeId, pCbcId )->GetData().SetDummyPad( (TPad *) pPad->GetPad(cNcol*cNrow) );
	}
	void ScurveAnalyser::SetVplusVsVCth0GraphDisplayPad( UInt_t pFeId, TPad *pPad ){

		GUIData::const_iterator cItFe = fGUIData.find( pFeId );

		const GUIFeInfo &cFeInfo = cItFe->second;
		UInt_t cNcbc = cFeInfo.size();
		if( cNcbc == 2 ){
			pPad->Divide( 2, 1 );
		}
		GUIFeInfo::const_iterator cItCbc = cFeInfo.begin();
		UInt_t i(0);
		for( ; cItCbc != cFeInfo.end(); cItCbc++ ){
			UInt_t cCbcId = cItCbc->first;
			fGUIData.GetCbcInfo( cItFe->first, cCbcId )->GetData().SetVplusVsVCth0GraphDisplayPad( (TPad *)pPad->GetPad( ++i ) );
		}
	}
	TH1F * ScurveAnalyser::createScurveHistogram( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannelId ){

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
	void ScurveAnalyser::resetHistograms(){

		fHtotal->Reset();

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		const std::vector<CalibrationChannelInfo *> *cChannelList = cTestGroup->GetChannelList(); 
		for( UInt_t k=0; k < cChannelList->size(); k++ ){

			CalibrationChannelInfo *cChannel = cChannelList->at(k);
			cChannel->GetData().ResetHist();
		}
	}
	void ScurveAnalyser::updateOffsets(){
		for( UInt_t i=0; i < fChannelList.size(); i++ ){
			if( fChannelList[i] == 0 ) continue;
			fChannelList[i]->GetData().UpdateOffset();
		}
	}

	TString ScurveAnalyser::getScanType(){

		TString cType;
		if( fScanType == OFFSETCALIB ){

			TString cScanType;
			if( fCurrentTargetBit == 8 ) cScanType = "Initial offsets = 0x00";	
			else if( fCurrentTargetBit == -1 ) cScanType = "Result";
			else cScanType = Form( "Offset bit%u #rightarrow 1", fCurrentTargetBit );

			cType = Form( "OffsetCalib%s", cScanType.Data() ); 
		}
		else if( fScanType == VPLUSSEARCH ){

			UInt_t cVplus = fCbcRegMap->GetReadValue( 0, 0, 0, 0x0B );
			cType = Form( "Vplus = 0x%02X", cVplus );
		}
		else if( fScanType == SINGLEVCTHSCAN ){

			cType = Form( "Single scan" );
		}
		return cType;
	}
	TString ScurveAnalyser::getScanId(){

		TString cType;
		if( fScanType == OFFSETCALIB ){

			TString cScanType;
			if( fCurrentTargetBit == 8 ) cScanType = "Offsets00";	
			else if( fCurrentTargetBit == -1 ) cScanType = "Result";
			else cScanType = Form( "Bit%uIs1", fCurrentTargetBit );

			cType = Form( "OffsetCalib%s", cScanType.Data() ); 
		}
		else if( fScanType == VPLUSSEARCH ){

			UInt_t cVplus = fCbcRegMap->GetReadValue( 0, 0, 0, 0x0B );
			cType = Form( "Vplus%02X", cVplus );
		}
		else if( fScanType == SINGLEVCTHSCAN ){

			UInt_t tmp = fCbcRegMap->GetReadValue( 0, 0, 0, 0x0F );
			UInt_t cMask = 1 << 6;
			tmp &= cMask;
			UInt_t cTPEnable = tmp == 0 ? 0 : 1;
			UInt_t cTPPot = fCbcRegMap->GetReadValue( 0, 0, 0, 0x0D );
			cType = Form( "SingleScanTPEnable%dTPPot%02X", cTPEnable, cTPPot );
		}
		return cType;
	}
	void ScurveAnalyser::setNextOffsets(){

		if( fScanType != OFFSETCALIB ) return;	

		UInt_t cGroupId(0);
		CalibrationTestGroup *cTestGroup = fGroupMap->GetActivatedGroup(cGroupId);
		if( cTestGroup == 0 ) return;

		UInt_t cFeId(0), cCbcId(0), cChannelId(0);

		const std::vector<CalibrationChannelInfo *> *cChannelList = cTestGroup->GetChannelList(); 
		for( UInt_t k=0; k < cChannelList->size(); k++ ){

			CalibrationChannelInfo *cChannel = cChannelList->at(k);
			if( cChannel == 0 ) continue;

			cChannel->Id( cFeId, cCbcId, cChannelId );
			//std::cout << "GroupId = " << cGroupId << ", Channel id = " << cChannelId << std::endl;
			TH1F *h = cChannel->GetData().Hist();
			if(h==0) continue;

			TF1 *cFunc = (TF1 *) h->GetListOfFunctions()->Last();	
			if( !cFunc ) continue;

			double cVCth0 = cFunc->GetParameter(0);
			UInt_t cOffset = cChannel->GetData().Offset();
			if( fTargetVCth < cVCth0 && fCurrentTargetBit != 8 ){
				cOffset &= ~( 1 << fCurrentTargetBit );
			}
			if( fCurrentTargetBit > 0 )cOffset |= 1 << ( fCurrentTargetBit - 1 );
			cChannel->GetData().SetNextOffset( cOffset );
			//if( cCbcId == 0 && cChannelId == 0 ){
			//	if( cOffset & 0x40000000 ){
			//		std::cout << cVCth0 << " " << fCurrentTargetBit << std::endl; 
			//	}
			//}
		}
	}
}
