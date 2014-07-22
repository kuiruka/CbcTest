#include <fstream>
#include <iostream>
#include <TString.h>
#include "Strasbourg/Data.h"

const unsigned int gNFe = 1;
const unsigned int gNeventPerAcq = 1;

int main( int argc, char *argv[] ){

	TString cFileName( "RawData.dat" );
	if( argc >= 2 ){
		cFileName = argv[1];
	}
	std::ifstream *cDataFile = new std::ifstream( cFileName.Data(), std::ifstream::binary );
//	std::ifstream *cDataFile = new std::ifstream( cFileName.Data() );

	Strasbourg::Data *cData = new Strasbourg::Data();
	cData->Clear();
	//Initialising Data object
	for( unsigned int cFe=0; cFe < gNFe; cFe++ ){
		cData->AddFe( cFe );
	}
	if( gNFe == 1 ) cData->AddFe( 1, true );
	std::cout << "Initialising" << std::endl;
	cData->Initialise( gNeventPerAcq );

	std::cout << "size in int32 = " << cData->GetEventSize32() << std::endl;
	unsigned int cBufSize = cData->GetBufSize();
	std::cout << "size in byte = " << cBufSize << std::endl;
	char *cDataBuffer = (char *)malloc( cBufSize );

	TString cCommand;	

	while( !cDataFile->eof() ){

		cDataFile->read( cDataBuffer, cBufSize );
/*
		std::cout <<std::hex;
		for( UInt_t i=0; i< cBufSize; i++){
			std::cout <<std::uppercase<<std::setw(2)<<std::setfill('0')<< (cDataBuffer[i]&0xFF);
			if( ( i % (42*4) ) == 42*4-1 ) std::cout << std::endl;
		}
		std::cout << std::endl;
*/
		cData->CopyBuffer( cDataBuffer );

		const Strasbourg::Event * cEvent = cData->GetNextEvent();

		while( cEvent ){
			std::cout << "Type [N/n] for the next event" << std::endl;
			std::cin >> cCommand;
			if( cCommand == "n" || cCommand == "N" ){
				std::cout << cEvent->HexString() << std::endl;
				std::cout << "Bunch      = " << cEvent->GetBunch()         << std::endl; 
				std::cout << "EventCount = " << cEvent->GetEventCountCBC() << std::endl; 

				const Strasbourg::FeEvent *cFeEvent = cEvent->GetFeEvent( 0 );
				const Strasbourg::CbcEvent *cCbcEvent = cFeEvent->GetCbcEvent( 0 );
				std::cout << "DATASTRING : " << cCbcEvent->DataHexString() << std::endl; 

				cEvent = cData->GetNextEvent();

			}
		}
	}
}

