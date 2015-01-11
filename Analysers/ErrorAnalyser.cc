#include "ErrorAnalyser.h"
#include "TString.h"
#include "TCanvas.h"
#include <math.h>
#include "TMath.h"
#include <TFile.h>
#include <TLegend.h>
#include <TThread.h>
#include "Cbc/CbcRegInfo.h"
#include "TGFrame.h"
#include "TGTextView.h"
#include "TH2F.h"
#include <TStyle.h>

namespace Analysers{

	ErrorAnalyser::ErrorAnalyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
			const CbcRegMap *pCbcRegMap, 
			Bool_t pNegativeLogicCbc,
			const char *pOutputDir,
			GUIFrame *pGUIFrame ):
			Analyser( pBeId, pNFe, pNCbc, pCbcRegMap, pNegativeLogicCbc, pOutputDir, pGUIFrame ),
			fErrorTypeHist(0), fLiveTime(0){
				gStyle->SetTextSize(5);
		}
	ErrorAnalyser::~ErrorAnalyser(){
/*
		ErrorHistGroupMap::iterator cIt = fErrorHistGroupMap.begin();
		for( ; cIt != fErrorHistGroupMap.end(); cIt++ ){
			delete cIt->second;
		}
		*/
	}

	void ErrorAnalyser::Initialise(){

		TString cHname = Form( "hErrorTypeHist" ); 
		TObject *cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;
		fErrorTypeHist = new TH1F( cHname, "Error Type", BeController::OTHER_ERROR+1, 0-0.5, BeController::OTHER_ERROR + 0.5 ); 

		fResult.clear();

		for( UInt_t cFeId=0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId< fNCbc; cCbcId++ ){

				fResult.AddCbcInfo( cFeId, cCbcId, ErrorAnalyserCbcInfo( cFeId, cCbcId ) );
				ErrorAnalyserCbcInfo *cCbcInfo = fResult.GetCbcInfo( cFeId, cCbcId );	
				cCbcInfo->GetData().SetHistograms( fBeId, cFeId, cCbcId );
			}
		}
#ifdef __CBCDAQ_DEV__
		std::cout << "ErrorAnalyser::Initialise() done" << std::endl;
#endif
	}

	void ErrorAnalyser::Reset(){
		
		fErrorDetected = false; //added after SEU test
		ResetHist();
	}
	void ErrorAnalyser::ResetHist(){

		fErrorTypeHist->Reset();

		fNUnExpectedBits = 0;
	
		ErrorAnalyserResult::iterator cItFe = fResult.begin();
		for( ; cItFe != fResult.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			ErrorAnalyserFeInfo &cFeInfo = cItFe->second;
			ErrorAnalyserFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){
				UInt_t cCbcId = cItCbc->first;
				ErrorHistGroup &cData =  fResult.GetCbcInfo( cFeId, cCbcId )->GetData();
				TH1F *cHist = cData.fDataStream.first;
				cHist->Reset();
				cHist = cData.fData.first;
				cHist->Reset();
				cHist = cData.fError.first;
				cHist->Reset();
				cHist = cData.fPipelineAddress0.first;
				cHist->Reset();
				cHist = cData.fPipelineAddress1.first;
				cHist->Reset();
				cHist = cData.fPipelineAddress2.first;
				cHist->Reset();
				cHist = cData.fPipelineAddress3.first;
				cHist->Reset();

				unsigned int cTriggerLatency = fCbcRegMap->GetReadValue( cFeId, cCbcId, "TriggerLatency" );
				std::cout << "cTriggerLatency : " << std::dec << cTriggerLatency << std::endl;
				std::cout << "fL1Pointer      : " << std::dec << fL1APointer     << std::endl;
				int cTmp = ( fL1APointer - cTriggerLatency ) % 256;
				if( cTmp < 0 ) cTmp += 256; 
				cData.fExpectedPipelineAddress = cTmp;
				int cAddress = cTmp;
				fExpectedPipelineAddressHist = (TH2F *) cData.fPipelineAddress0.first;
				if( 64 <= cAddress && cAddress < 128 ) fExpectedPipelineAddressHist = (TH2F *)cData.fPipelineAddress1.first; 
				else if( 128 <= cAddress && cAddress < 192 ) fExpectedPipelineAddressHist = (TH2F *)cData.fPipelineAddress2.first; 
				else if( 192 <= cAddress && cAddress < 256 ) fExpectedPipelineAddressHist = (TH2F *)cData.fPipelineAddress3.first; 
			}
		}
	}
	UInt_t ErrorAnalyser::Analyse( const Event *pEvent, bool pFillDataStream, int pErrorType ){

		int cHits(0);
		fErrorDetected = false;
		fEventErrorStr = "";

		fErrorTypeHist->Fill( pErrorType );	

		if( pEvent == 0 ){

			fErrorDetected = true;
			ErrorAnalyserResult::iterator cItFe = fResult.begin();
			for( ; cItFe != fResult.end(); cItFe++ ){

				ErrorAnalyserFeInfo &cFeInfo = cItFe->second;
				ErrorAnalyserFeInfo::iterator cItCbc = cFeInfo.begin();
				for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

					ErrorAnalyserCbcInfo &cCbcInfo = cItCbc->second;

					ErrorHistGroup &cErrorHistGroup = cCbcInfo.GetData();
					cErrorHistGroup.fData.first->Fill( CbcEvent::NSENSOR );
				}

			}
			return cHits;
		}

		ErrorAnalyserResult::iterator cItFe = fResult.begin();
		for( ; cItFe != fResult.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			ErrorAnalyserFeInfo &cFeInfo = cItFe->second;
			ErrorAnalyserFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				ErrorAnalyserCbcInfo &cCbcInfo = cItCbc->second;


				const FeEvent *cFeEvent = pEvent->GetFeEvent( cFeId );
				if( cFeEvent == 0 ) continue; 
				const CbcEvent *cCbcEvent = cFeEvent->GetCbcEvent( cCbcId );
				if( cCbcEvent == 0 ) continue;

				ErrorHistGroup &cErrorHistGroup = cCbcInfo.GetData();
				TH1F *cHist = cErrorHistGroup.fError.first;
				cHist->Fill( cCbcEvent->Error() );
				if( cCbcEvent->Error() != 0 ){
					fErrorDetected = true;
					fEventErrorStr = Form( "[FE]%2d [CBC]%2d [Error] %1d\n", cFeId, cCbcId, cCbcEvent->Error() );
				}
				UInt_t cExpectedAddress = cErrorHistGroup.fExpectedPipelineAddress;
				fExpectedPipelineAddressHist->Fill( cExpectedAddress, 0.80 );

				UInt_t cAddress = cCbcEvent->PipelineAddress();	
				//				std::cout << "Pipeline Address is " << cAddress << std::endl;
				TH2F *cPhist = (TH2F *) cErrorHistGroup.fPipelineAddress0.first;
				if( 64 <= cAddress && cAddress < 128 ) cPhist = (TH2F *)cErrorHistGroup.fPipelineAddress1.first; 
				else if( 128 <= cAddress && cAddress < 192 ) cPhist = (TH2F *)cErrorHistGroup.fPipelineAddress2.first; 
				else if( 192 <= cAddress && cAddress < 256 ) cPhist = (TH2F *)cErrorHistGroup.fPipelineAddress3.first; 
				cPhist->Fill( cAddress, 0.30 );
				if( cCbcEvent->Error() == 0 && cAddress != cExpectedAddress ){
					fErrorDetected = true; 
					fEventErrorStr = Form( "[FE]%2d [CBC]%2d [Error] Pipeline address is different from the expected value.\n", cFeId, cCbcId  );
				}
				bool cUnExpectedValues( false );
				for( UInt_t cId = 0; cId < CbcEvent::NSENSOR; cId++ ){ 

					bool cBitValue = cCbcEvent->DataBit( cId );
					if( cBitValue ) {
						cErrorHistGroup.fData.first->Fill( cId );
						cHits++;
						if( fExpectedDataValue == 0 ){
							fErrorDetected = true;
							cUnExpectedValues = true;
							fNUnExpectedBits += 1;
						}
					}
					else{
						if( fExpectedDataValue == 1 ){
							fErrorDetected = true;
							cUnExpectedValues = true;
							fNUnExpectedBits += 1;
						}
					}

				}
				if( cUnExpectedValues ) fEventErrorStr += Form( "\tSome channels have unexpected value." );

				cErrorHistGroup.fData.first->Fill( CbcEvent::NSENSOR );

				if( pFillDataStream ){
					cErrorHistGroup.fDataStream.first->Reset();
					for( UInt_t cId = 0; cId < CbcEvent::NSENSOR; cId++ ){ 

						bool cBitValue = cCbcEvent->DataBit( cId );
						if( cBitValue ) {

							cErrorHistGroup.fDataStream.first->Fill( cId );
						}
					}
				}
			}
		}
		return cHits;
	}

	void ErrorAnalyser::SetHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){	

		pPad->Divide( 1, 5 );
		//		TPad *cPad = (TPad *)pPad->GetPad(1);
		//		cPad->Divide( 2, 1 );

		ErrorHistGroup &cGroup = fResult.GetCbcInfo( pFeId, pCbcId )->GetData();
		cGroup.fDataStream.second = (TPad *)pPad->GetPad(5);
		cGroup.fData.second = (TPad *)pPad->GetPad(5);
		//		cGroup->fError.second = (TPad *)cPad->GetPad(1);
		cGroup.fPipelineAddress0.second = (TPad *)pPad->GetPad(1);
		cGroup.fPipelineAddress1.second = (TPad *)pPad->GetPad(2);
		cGroup.fPipelineAddress2.second = (TPad *)pPad->GetPad(3);
		cGroup.fPipelineAddress3.second = (TPad *)pPad->GetPad(4);
	}
	void ErrorAnalyser::DrawHists(){

		if( !fGUIFrame ) return;
		TPad *cPad(0);
		ErrorAnalyserResult::iterator cItFe = fResult.begin();
		for( ; cItFe != fResult.end(); cItFe++ ){

			ErrorAnalyserFeInfo &cFeInfo = cItFe->second;
			ErrorAnalyserFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				ErrorAnalyserCbcInfo &cCbcInfo = cItCbc->second;
				ErrorHistGroup &cErrorHistGroup = cCbcInfo.GetData();

				std::pair<TH1F *, TPad *> cDataStream = cErrorHistGroup.fDataStream;
				std::pair<TH1F *, TPad *> cData       = cErrorHistGroup.fData;
				std::pair<TH1F *, TPad *> cError      = cErrorHistGroup.fError;
				std::pair<TH1F *, TPad *> cPipelineAddress0      = cErrorHistGroup.fPipelineAddress0;
				std::pair<TH1F *, TPad *> cPipelineAddress1      = cErrorHistGroup.fPipelineAddress1;
				std::pair<TH1F *, TPad *> cPipelineAddress2      = cErrorHistGroup.fPipelineAddress2;
				std::pair<TH1F *, TPad *> cPipelineAddress3      = cErrorHistGroup.fPipelineAddress3;

				if( fShowDataStream ){ 
					cPad = (TPad *)cDataStream.second->cd();
					cDataStream.first->Draw();
					cPad->Update();
				}
				else{
					cPad = (TPad *)cData.second->cd();
					cData.first->Draw();
					cPad->Update();
				}
				/*
				   cPad = (TPad *)cError.second->cd();
				   cError.first->Draw();
				   cPad->Update();
				 */

				cPad = (TPad *)cPipelineAddress0.second->cd();
				( (TH2F *) cPipelineAddress0.first)->Draw("TEXT");
				cPad->Update();

				cPad = (TPad *)cPipelineAddress1.second->cd();
				( (TH2F *) cPipelineAddress1.first)->Draw("TEXT");
				cPad->Update();

				cPad = (TPad *)cPipelineAddress2.second->cd();
				( (TH2F *) cPipelineAddress2.first)->Draw("TEXT");
				cPad->Update();

				cPad = (TPad *)cPipelineAddress3.second->cd();
				( (TH2F *) cPipelineAddress3.first)->Draw("TEXT");
				cPad->Update();
			}
		}
	}
	TString ErrorAnalyser::Dump(){

		TString cStr;

		ErrorAnalyserResult::iterator cItFe = fResult.begin();
		for( ; cItFe != fResult.end(); cItFe++ ){

			UInt_t cFeId = cItFe->first;
			ErrorAnalyserFeInfo &cFeInfo = cItFe->second;
			ErrorAnalyserFeInfo::iterator cItCbc = cFeInfo.begin();
			for( ; cItCbc != cFeInfo.end(); cItCbc++ ){

				UInt_t cCbcId = cItCbc->first;
				ErrorAnalyserCbcInfo &cCbcInfo = cItCbc->second;
				ErrorHistGroup &cErrorHistGroup = cCbcInfo.GetData();

				cStr += Form( "==========================================================================================================================================================\n" );
				cStr += Form( "FE%uCBC%u\n", cFeId, cCbcId );
				cStr += Form( "                                                                               %18s %18s %18s %18s %18s\n", "00", "01", "10", "11", "Unexpected bits" );
				cStr += Form( "----------------------------------------------------------------------------------------------------------------------------------------------------------\n" );

				UInt_t cNe00 = (UInt_t) cErrorHistGroup.fError.first->GetBinContent(1); 
				UInt_t cNe01 = (UInt_t) cErrorHistGroup.fError.first->GetBinContent(2); 
				UInt_t cNe10 = (UInt_t) cErrorHistGroup.fError.first->GetBinContent(3); 
				UInt_t cNe11 = (UInt_t) cErrorHistGroup.fError.first->GetBinContent(4); 
				UInt_t cN    = (UInt_t) cErrorHistGroup.fData.first->GetBinContent( cErrorHistGroup.fData.first->GetNbinsX() + 1 );

				Float_t cP00 = cNe00 / cN;
				Float_t cP01 = cNe01 / cN;
				Float_t cP10 = cNe10 / cN;
				Float_t cP11 = cNe11 / cN;

				cStr += Form( "#(%%) of ERROR                                                                : %8u(%8.3f) %8u(%8.3f) %8u(%8.3f) %8u(%8.3f) %18u\n", 
						cNe00, cP00, cNe01, cP01, cNe10, cP10, cNe11, cP11, fNUnExpectedBits ); 	

				Float_t cTime_0 = fNclock1Cycle * 25. * cN; 
				fLiveTime = cTime_0;
				Float_t cRe00_0 = cNe00 / cTime_0;
				Float_t cRe01_0 = cNe01 / cTime_0;
				Float_t cRe10_0 = cNe10 / cTime_0;
				Float_t cRe11_0 = cNe11 / cTime_0;
				Float_t cReNbits_0 = (float)fNUnExpectedBits / cTime_0;

				Float_t cTime_1 = fNclockFRtoL1A * 25. * cN; 
				Float_t cRe00_1 = cNe00 / cTime_1;
				Float_t cRe01_1 = cNe01 / cTime_1;
				Float_t cRe10_1 = cNe10 / cTime_1;
				Float_t cRe11_1 = cNe11 / cTime_1;
				Float_t cReNbits_1 = (float)fNUnExpectedBits / cTime_1;

				cStr += Form( "Rate of ERROR 1E-9/sec [Live time : FR to FR(L1A) %8.1f(%8.1f) sec]    : %8.3f(%8.3f) %8.3f(%8.3f) %8.3f(%8.3f) %8.3f(%8.3f) %8.3f(%8.3f)\n",
						cTime_0*1e-9, cTime_1*1e-9, cRe00_0, cRe00_1, cRe01_0, cRe01_1, cRe10_0, cRe10_1, cRe11_0, cRe11_1, cReNbits_0, cReNbits_1 ); 	

				double cTimeIntensity = cTime_1 * (double)fBeamIntensity;
				double cRe00 = cNe00 / cTimeIntensity;
				double cRe01 = cNe01 / cTimeIntensity;
				double cRe10 = cNe10 / cTimeIntensity;
				double cRe11 = cNe11 / cTimeIntensity;
				double cReNbits = (float)fNUnExpectedBits / cTimeIntensity;
				cStr += Form( "Rate of error 1e-9/(sec * intensity unit)                                    : %18.3f %18.3f %18.3f %18.3f %18.3f\n", cRe00, cRe01, cRe10, cRe11, cReNbits ); 	

				cStr += Form( "----------------------------------------------------------------------------------------------------------------------------------------------------------\n" );


				Float_t cNFifoNotInWriteData = fErrorTypeHist->GetBinContent( BeController::FIFO_NOT_IN_WRITE_DATA +1 );	
				Float_t cNSramNotFull        = fErrorTypeHist->GetBinContent( BeController::SRAM_NOT_FULL          +1 );
				Float_t cNSramNotNonFull     = fErrorTypeHist->GetBinContent( BeController::SRAM_NOT_NONFULL       +1 );
				double cReFifoNotInWriteData = cNFifoNotInWriteData / cTimeIntensity;
				double cReSramNotFull        = cNSramNotFull        / cTimeIntensity;
				double cReSramNotNonFull     = cNSramNotNonFull     / cTimeIntensity;
				cStr += Form( "#(rate) of error with no data 1e-9/(sec * intensity unit)                    : [FIFO_NOT_IN_WRITE_DATA]%3.0f(%8.3f),  [SRAM_NOT_FULL] %3.0f(%8.3f),  [SRAM_NOT_NONFULL] %3.0f(%8.3f)\n", 
						cNFifoNotInWriteData, cReFifoNotInWriteData, cNSramNotFull, cReSramNotFull, cNSramNotNonFull, cReSramNotNonFull ); 	

				float cLiveTimeRate_0 = cTime_0 / fRealTime * 100;
				float cLiveTimeRate_1 = cTime_1 / fRealTime * 100;
				cStr += Form( "Live time / Real time %%                                                      : [Live time : FR to FR ] %4.1f;\t[Live time : FR to L1A] %4.1f\n", cLiveTimeRate_0, cLiveTimeRate_1 ); 

			}
			cStr += Form( "==========================================================================================================================================================\n" );
		}
		return cStr;
	}
	void ErrorAnalyser::DrawText(){

		if( !fGUIFrame ) return;

		fTextView->Clear();
		fTextView->LoadBuffer( Dump().Data() );
	}
	void ErrorAnalyser::DrawText( TString &pText ){

		if( !fGUIFrame ) return;

		fTextView->Clear();
		fTextView->LoadBuffer( pText.Data() );
	}

	void ErrorHistGroup::SetHistograms( UInt_t pBeId, UInt_t pFeId, UInt_t pCbcId ){

		double cTitleSize(0.12);
		double cLabelSize(0.10);

		gStyle->SetLabelSize(cLabelSize, "X" );
		gStyle->SetTitleSize(cTitleSize, "Y" );

		HistGroup::SetHistograms( pBeId, pFeId, pCbcId );
		fDataStream.first->GetYaxis()->SetTitle( "DataStream" );
		fDataStream.first->GetYaxis()->SetTitleOffset( 0.10 );
		fDataStream.first->GetYaxis()->SetLabelSize( 0.10 );
		fDataStream.first -> SetNdivisions( 101, "y" );

		TString cHname = Form( "hDataBE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
		TObject *cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;

		TH1F *cHist = new TH1F( cHname, 
				Form( ";;Data BE: %u FE:%u CBC:%u", pBeId, pFeId, pCbcId ), 
				CbcEvent::NSENSOR, -0.5, CbcEvent::NSENSOR-0.5 ); 

		cHist->GetYaxis()->CenterTitle();
		cHist->GetYaxis()->SetTitleOffset( 0.10 );
		cHist->GetYaxis()->SetTitle( "Data" );
		cHist->GetYaxis()->SetLabelSize( 0.1 );
		cHist -> SetNdivisions( 101, "y" );

		fData = std::pair<TH1F *, TPad *>( cHist, 0 );


		cHname = Form( "hErrorBE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
		cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;

		cHist = new TH1F( cHname, 
				Form( "Error BE: %u FE:%u CBC:%u", pBeId, pFeId, pCbcId ), 
				4, -0.5, 4-0.5 ); 

		fError = std::pair<TH1F *, TPad *>( cHist, 0 );


		int cNch(64);
		//double cTitleOffset(0.05);
		//gStyle->SetTitleOffset( cTitleOffset, "t" );
		double cMarkerSize(5.0);
		int cNdivisions(907);
		int cNdivisionsY(0);
		cHname = Form( "hPipeline0BE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
		cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;

		TH2F *htmp = new TH2F( cHname, 
				Form( ";;Pipeline %3d-%3d", 0, cNch-1  ), 
				cNch, -0.5, cNch-0.5, 4, 0, 1 ); 
		htmp->GetYaxis()->SetTitleSize( cTitleSize );
		htmp->GetYaxis()->SetTitleOffset( 0.05 );
		htmp -> SetLabelSize(cLabelSize);
		htmp -> SetMarkerSize(cMarkerSize );
		htmp -> SetNdivisions( cNdivisions );
		htmp -> SetNdivisions( cNdivisionsY, "y" );
		htmp->GetYaxis()->CenterTitle();
		cHist = (TH1F *) htmp;
		fPipelineAddress0 = std::pair<TH1F *, TPad *>( cHist, 0 );

		cHname = Form( "hPipeline1BE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
		cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;

		htmp = new TH2F( cHname, 
				Form( ";;Pipeline %3d-%3d", cNch, cNch*2-1  ), 
				cNch, cNch-0.5, cNch*2-0.5, 4, 0, 1 ); 
		htmp -> SetLabelSize(cLabelSize);
		htmp -> SetMarkerSize(cMarkerSize );
		htmp -> SetNdivisions( cNdivisions );
		htmp -> SetNdivisions( cNdivisionsY, "y" );
		htmp->SetTitleSize( cTitleSize, "y" );
		htmp->GetYaxis()->SetTitleOffset( 0.05 );
		htmp->GetYaxis()->CenterTitle();
		cHist = (TH1F *) htmp;
		fPipelineAddress1 = std::pair<TH1F *, TPad *>( cHist, 0 );

		cHname = Form( "hPipeline2BE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
		cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;

		htmp = new TH2F( cHname, 
				Form( ";;Pipeline %3d-%3d", cNch*2, cNch*3-1  ), 
				cNch, cNch*2-0.5, cNch*3-0.5, 4, 0, 1 ); 
		htmp -> SetLabelSize(cLabelSize);
		htmp -> SetMarkerSize(cMarkerSize );
		htmp -> SetNdivisions( cNdivisions );
		htmp -> SetNdivisions( cNdivisionsY, "y" );
		htmp->SetTitleSize( cTitleSize, "y" );
		htmp->GetYaxis()->SetTitleOffset( 0.05 );
		htmp->GetYaxis()->CenterTitle();
		cHist = (TH1F *) htmp;
		fPipelineAddress2 = std::pair<TH1F *, TPad *>( cHist, 0 );

		cHname = Form( "hPipeline3BE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
		cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;

		htmp = new TH2F( cHname, 
				Form( ";;Pipeline %3d-%3d", cNch*3, cNch*4-1  ), 
				cNch, cNch*3-0.5, cNch*4-0.5, 4, 0, 1 ); 
		htmp -> SetLabelSize(cLabelSize);
		htmp -> SetMarkerSize(cMarkerSize );
		htmp -> SetNdivisions( cNdivisions );
		htmp -> SetNdivisions( cNdivisionsY, "y" );
		htmp->SetTitleSize( cTitleSize, "y" );
		htmp->GetYaxis()->SetTitleOffset( 0.05 );
		htmp->GetYaxis()->CenterTitle();
		cHist = (TH1F *) htmp;
		fPipelineAddress3 = std::pair<TH1F *, TPad *>( cHist, 0 );
	}
	ErrorHistGroup::~ErrorHistGroup(){
		/*
		   delete fData.first;
		   delete fError.first;
		   delete fPipelineAddress.first;
		 */
	}
}
