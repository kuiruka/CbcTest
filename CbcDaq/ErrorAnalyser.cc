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

namespace CbcDaq{

	ErrorAnalyser::ErrorAnalyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
			const CbcRegMap *pCbcRegMap, 
			Bool_t pNegativeLogicCbc,
			const char *pOutputDir,
			GUIFrame *pGUIFrame ):
			Analyser( pBeId, pNFe, pNCbc, pCbcRegMap, pNegativeLogicCbc, pOutputDir, pGUIFrame ){
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

		for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

				Int_t cId = getHistGroupId( cFeId, cCbcId );
				ErrorHistGroupMap::iterator cIt = fErrorHistGroupMap.find( cId );
				if( cIt != fErrorHistGroupMap.end() ){
					delete cIt->second;
					fErrorHistGroupMap.erase(cIt);
				}
				ErrorHistGroup * cErrorHistGroup = new ErrorHistGroup( fBeId, cFeId, cCbcId ); 
				fErrorHistGroupMap.insert( std::pair<UInt_t, ErrorHistGroup *>( cId, cErrorHistGroup ) );
			}
		}
#ifdef __CBCDAQ_DEV__
			std::cout << "ErrorAnalyser::Initialise() done" << std::endl;
#endif
	}

	void ErrorAnalyser::ConfigureRun(){

		ResetHist();
	}
	void ErrorAnalyser::ResetHist(){

		ErrorHistGroupMap::iterator cIt = fErrorHistGroupMap.begin();
		for( ; cIt != fErrorHistGroupMap.end(); cIt++ ){
			TH1F *cHist = cIt->second->fDataStream.first;
			cHist->Reset();
			cHist = cIt->second->fData.first;
			cHist->Reset();
			cHist = cIt->second->fError.first;
			cHist->Reset();
			cHist = cIt->second->fPipelineAddress.first;
			cHist->Reset();
		}
	}
	UInt_t ErrorAnalyser::Analyse( const Event *pEvent, bool pFillDataStream ){


		int cHits(0);

		for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

			const FeEvent *cFeEvent = pEvent->GetFeEvent( cFeId );
			if( cFeEvent == 0 ) continue; 
			for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

				const CbcEvent *cCbcEvent = cFeEvent->GetCbcEvent( cCbcId );
				if( cCbcEvent == 0 ) continue;

				Int_t cId = getHistGroupId( cFeId, cCbcId );
				ErrorHistGroupMap::iterator cIt = fErrorHistGroupMap.find( cId );
				if( cIt == fErrorHistGroupMap.end() ) continue;

				ErrorHistGroup *cErrorHistGroup = cIt->second;
				TH1F *cHist = cErrorHistGroup->fError.first;
				cHist->Fill( cCbcEvent->Error() );

				cHist = cErrorHistGroup->fPipelineAddress.first;
				cHist->Fill( cCbcEvent->PipelineAddress() );

				for( UInt_t cId = 0; cId < CbcEvent::NSENSOR; cId++ ){ 

					bool cBitValue = cCbcEvent->DataBit( cId );
					if( cBitValue ) {
						cErrorHistGroup->fData.first->Fill( cId );
						cHits++;
					}
				}
				cErrorHistGroup->fData.first->Fill( CbcEvent::NSENSOR );
				
				if( pFillDataStream ){
					cErrorHistGroup->fDataStream.first->Reset();
					for( UInt_t cId = 0; cId < CbcEvent::NSENSOR; cId++ ){ 

						bool cBitValue = cCbcEvent->DataBit( cId );
						if( cBitValue ) {

							cErrorHistGroup->fDataStream.first->Fill( cId );
						}
					}
				}
			}
		}
		return cHits;
	}

	void ErrorAnalyser::SetHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){	

		pPad->Divide( 1, 2 );
		TPad *cPad = (TPad *)pPad->GetPad(1);
		cPad->Divide( 2, 1 );

		Int_t cId = getHistGroupId( pFeId, pCbcId );
		ErrorHistGroupMap::iterator cIt = fErrorHistGroupMap.find( cId );
		ErrorHistGroup *cGroup = cIt->second;
		cGroup->fDataStream.second = (TPad *)pPad->GetPad(2);
		cGroup->fData.second = (TPad *)pPad->GetPad(2);
		cGroup->fError.second = (TPad *)cPad->GetPad(1);
		cGroup->fPipelineAddress.second = (TPad *)cPad->GetPad(2);
	}
	void ErrorAnalyser::DrawHists(){

		if( !fGUIFrame ) return;
		TPad *cPad(0);
		ErrorHistGroupMap::iterator cIt = fErrorHistGroupMap.begin();
		for( ; cIt != fErrorHistGroupMap.end(); cIt++ ){

			std::pair<TH1F *, TPad *> cDataStream = cIt->second->fDataStream;
			std::pair<TH1F *, TPad *> cData       = cIt->second->fData;
			std::pair<TH1F *, TPad *> cError      = cIt->second->fError;
			std::pair<TH1F *, TPad *> cPipelineAddress      = cIt->second->fPipelineAddress;

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
			cPad = (TPad *)cError.second->cd();
			cError.first->Draw();
			cPad->Update();

			cPad = (TPad *)cPipelineAddress.second->cd();
			cPipelineAddress.first->Draw();
			cPad->Update();
		}
	}
	void ErrorAnalyser::DrawText(){

		if( !fGUIFrame ) return;

		fTextView->Clear();
		for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

				fTextView->AddText( new TGText( Form( "FE%uCBC%u\n", cFeId, cCbcId )  ) );
				Int_t cId = getHistGroupId( cFeId, cCbcId );
				ErrorHistGroupMap::iterator cIt = fErrorHistGroupMap.find( cId );
				ErrorHistGroup *cErrorHistGroup = cIt->second;

				UInt_t cNe00 = (UInt_t) cErrorHistGroup->fError.first->GetBinContent(1); 
				UInt_t cNe01 = (UInt_t) cErrorHistGroup->fError.first->GetBinContent(2); 
				UInt_t cNe10 = (UInt_t) cErrorHistGroup->fError.first->GetBinContent(3); 
				UInt_t cNe11 = (UInt_t) cErrorHistGroup->fError.first->GetBinContent(4); 
				UInt_t cN    = (UInt_t) cErrorHistGroup->fData.first->GetBinContent( cErrorHistGroup->fData.first->GetNbinsX() + 1 );

				Float_t cRe00 = cNe00 / cN;
				Float_t cRe01 = cNe01 / cN;
				Float_t cRe10 = cNe10 / cN;
				Float_t cRe11 = cNe11 / cN;

				fTextView->AddText( new TGText( Form( "# of ERROR 00 : %5u", cNe00 ) ) ); 	
				fTextView->AddText( new TGText( Form( "# of ERROR 01 : %5u", cNe01 ) ) ); 	
				fTextView->AddText( new TGText( Form( "# of ERROR 10 : %5u", cNe10 ) ) ); 	
				fTextView->AddText( new TGText( Form( "# of ERROR 11 : %5u", cNe11 ) ) ); 	

				fTextView->AddText( new TGText( Form( "Rate of ERROR 00 : %5f", cRe00 ) ) ); 	
				fTextView->AddText( new TGText( Form( "Rate of ERROR 01 : %5f", cRe01 ) ) ); 	
				fTextView->AddText( new TGText( Form( "Rate of ERROR 10 : %5f", cRe10 ) ) ); 	
				fTextView->AddText( new TGText( Form( "Rate of ERROR 11 : %5f", cRe11 ) ) ); 	
			}
		}
	}

	ErrorHistGroup::ErrorHistGroup( UInt_t pBeId, UInt_t pFeId, UInt_t pCbcId ):
		HistGroup( pBeId, pFeId, pCbcId ){

			TString cHname = Form( "hDataBE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
			TObject *cObj = gROOT->FindObject( cHname ); 
			if( cObj ) delete cObj;

			TH1F *cHist = new TH1F( cHname, 
					Form( "Data BE: %u FE:%u CBC:%u", pBeId, pFeId, pCbcId ), 
					CbcEvent::NSENSOR, -0.5, CbcEvent::NSENSOR-0.5 ); 

			fData = std::pair<TH1F *, TPad *>( cHist, 0 );


			cHname = Form( "hErrorBE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
			cObj = gROOT->FindObject( cHname ); 
			if( cObj ) delete cObj;

			cHist = new TH1F( cHname, 
					Form( "Error BE: %u FE:%u CBC:%u", pBeId, pFeId, pCbcId ), 
					4, -0.5, 4-0.5 ); 

			fError = std::pair<TH1F *, TPad *>( cHist, 0 );


			cHname = Form( "hPipelineBE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
			cObj = gROOT->FindObject( cHname ); 
			if( cObj ) delete cObj;

			cHist = new TH1F( cHname, 
					Form( "Pipeline BE: %u FE:%u CBC:%u", pBeId, pFeId, pCbcId ), 
					256, -0.5, 256-0.5 ); 

			fPipelineAddress = std::pair<TH1F *, TPad *>( cHist, 0 );
		}
	ErrorHistGroup::~ErrorHistGroup(){
/*
		delete fData.first;
		delete fError.first;
		delete fPipelineAddress.first;
		*/
	}
}
