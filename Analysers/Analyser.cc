#include "Analyser.h"
#include "TString.h"
#include "TCanvas.h"
#include <TFile.h>
#include <TLegend.h>
#include <TThread.h>
#include "Cbc/CbcRegInfo.h"
#include "TGFrame.h"
#include "TGTextView.h"

namespace Analysers{

	Analyser::Analyser( UInt_t pBeId, UInt_t pNFe, UInt_t pNCbc, 
			const CbcRegMap *pCbcRegMap, 
			Bool_t pNegativeLogicCbc,
			const char *pOutputDir,
			GUIFrame *pGUIFrame ):
		fBeId(pBeId), fNFe(pNFe), fNCbc(pNCbc), 
		fCbcRegMap(pCbcRegMap), 
		fNegativeLogicCbc(pNegativeLogicCbc),
		fOutputDir( pOutputDir ),
		fGUIFrame( pGUIFrame ),
		fShowDataStream( kFALSE ){

		}

	Analyser::~Analyser(){
		HistGroupMap::iterator cIt = fHistGroupMap.begin();
		for( ; cIt != fHistGroupMap.end(); cIt++ ){
			delete cIt->second;
		}
	}
	void Analyser::Initialise(){

		for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

				Int_t cId = getHistGroupId( cFeId, cCbcId );
				HistGroupMap::iterator cIt = fHistGroupMap.find( cId );
				if( cIt != fHistGroupMap.end() ){
					delete cIt->second;
					fHistGroupMap.erase(cIt);
				}
				HistGroup * cHistGroup = new HistGroup();
				cHistGroup->SetHistograms( fBeId, cFeId, cCbcId ); 
				fHistGroupMap.insert( std::pair<UInt_t, HistGroup *>( cId, cHistGroup ) );
			}
		}
#ifdef __CBCDAQ_DEV__
			std::cout << "Analyser::Initialise() done" << std::endl;
#endif
	}

	UInt_t Analyser::Analyse( const Event *pEvent, bool pFillDataStream, int pErrorType ){

		if( !pFillDataStream || !fShowDataStream ) return 0;

		int cHits(0);
		for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

			const FeEvent *cFeEvent = pEvent->GetFeEvent( cFeId );
			if( cFeEvent == 0 ) continue; 
			for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

				const CbcEvent *cCbcEvent = cFeEvent->GetCbcEvent( cCbcId );
				if( cCbcEvent == 0 ) continue;

				Int_t cId = getHistGroupId( cFeId, cCbcId );
				HistGroupMap::iterator cIt = fHistGroupMap.find( cId );
				if( cIt == fHistGroupMap.end() ) continue;

				HistGroup *cHistGroup = cIt->second;

				cHistGroup->fDataStream.first->Reset();
				for( UInt_t cId = 0; cId < CbcEvent::NSENSOR; cId++ ){ 

					bool cBitValue = cCbcEvent->DataBit( cId );
					if( cBitValue ) {
						cHits++;
						cHistGroup->fDataStream.first->Fill( cId );
					}
				}
			}
		}
		return cHits;
	}
	void Analyser::SetHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad ){	

		Int_t cId = getHistGroupId( pFeId, pCbcId );
		HistGroupMap::iterator cIt = fHistGroupMap.find( cId );
		HistGroup *cGroup = cIt->second;
		cGroup->fDataStream.second = pPad;
	}

	Int_t Analyser::getHistGroupId( UInt_t pFeId, UInt_t pCbcId ){

		return fNCbc * pFeId + pCbcId;

	}
	void Analyser::DrawHists(){

		if( !( fGUIFrame && fShowDataStream ) ) return;
		TPad *cPad(0);
		HistGroupMap::iterator cIt = fHistGroupMap.begin();
		for( ; cIt != fHistGroupMap.end(); cIt++ ){

			std::pair<TH1F *, TPad *> cDataStream = cIt->second->fDataStream;

			cPad = (TPad *)cDataStream.second->cd();
			cDataStream.first->Draw();
			cPad->Update();
		}
	}
	void Analyser::DrawText(){

		if( !fGUIFrame ) return;

		fTextView->Clear();
		for( UInt_t cFeId = 0; cFeId < fNFe; cFeId++ ){

			for( UInt_t cCbcId=0; cCbcId < fNCbc; cCbcId++ ){

				fTextView->AddText( new TGText( Form( "FE%uCBC%u\n", cFeId, cCbcId )  ) );
			}
		}
	}
	void HistGroup::SetHistograms( UInt_t pBeId, UInt_t pFeId, UInt_t pCbcId ){

		TString cHname = Form( "hDataStreamBE%uFE%uCBC%u", pBeId, pFeId, pCbcId );
		TObject *cObj = gROOT->FindObject( cHname ); 
		if( cObj ) delete cObj;

		TH1F *cHist = new TH1F( cHname, 
				Form( ";;Data stream BE: %u FE:%u CBC:%u", pBeId, pFeId, pCbcId ), 
				CbcEvent::NSENSOR, -0.5, CbcEvent::NSENSOR-0.5 ); 

		cHist->GetYaxis()->SetTitle( "DataStream" );
		cHist->GetYaxis()->SetTitleOffset( 0.10 );
		cHist->GetYaxis()->SetLabelSize( 0.10 );
		cHist -> SetNdivisions( 101, "y" );

		fDataStream = std::pair<TH1F *, TPad *>( cHist, 0 );
	}

	HistGroup::~HistGroup(){

		//delete fDataStream.first;
	}
}

