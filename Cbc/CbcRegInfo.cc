#include "CbcRegInfo.h"
#include "utils/Exception.h"

using namespace CbcDaq;

namespace Cbc{

	void CbcRegItem::SetReadValue( UInt_t pReadValue ){ 

		fReadValue = pReadValue; 

		std::cout << "Name = " << fName << " WrittenValue = " << fWrittenValue << " ReadValue = " << fReadValue << std::endl;
		if( fReadValue != fWrittenValue ){ 
			std::cout << "Name = " << fName << " WrittenValue = " << fWrittenValue << " ReadValue = " << fReadValue << std::endl;
			if( fPage == 0 && fAddress == 0 ){
				unsigned int cMask(0x00000000);
				for( int i = 0; i < 7; i++ ) cMask |= (unsigned int) 1 << i;
				if( ( fReadValue & cMask ) != ( fWrittenValue & cMask ) ){
					fWriteFailed = true; 
				}
				else fWriteFailed = false;
			}
			else{
				fWriteFailed = true;
			}
		}
		else{
			fWriteFailed = false;
		}
	}
	const CbcRegItem *CbcRegList::GetRegItem( const std::string &pName )const{

		for( UInt_t i=0; i<size(); i++ ){
			if( at(i)->Name() == pName ) return at(i);
		}
		return 0;
	}
	const CbcRegItem *CbcRegList::GetRegItem( UInt_t pPage, UInt_t pAddr )const{

		for( UInt_t i=0; i<size(); i++ ){
			if( at(i)->Page() == pPage && at(i)->Address() == pAddr ) return at(i);
		}
		return 0;
	}
	UInt_t CbcRegList::GetValue0( const std::string &pName )const{

		const CbcRegItem *cItem = GetRegItem( pName );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for %s does not exist.", pName.c_str() ) );
		return cItem->Value0();
	}
	UInt_t CbcRegList::GetValue0( UInt_t pPage, UInt_t pAddr )const{

		const CbcRegItem *cItem = GetRegItem( pPage, pAddr );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for Page=%d, Address=0x%02X does not exist.", pPage, pAddr ) );
		return cItem->Value0();
	}
	UInt_t CbcRegList::GetWrittenValue( const std::string &pName )const{

		const CbcRegItem *cItem = GetRegItem( pName );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for %s does not exist.", pName.c_str() ) );
		return cItem->WrittenValue();
	}
	UInt_t CbcRegList::GetWrittenValue( UInt_t pPage, UInt_t pAddr )const{

		const CbcRegItem *cItem = GetRegItem( pPage, pAddr );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for Page=%d, Address=0x%02X does not exist.", pPage, pAddr ) );
		return cItem->WrittenValue();
	}
	UInt_t CbcRegList::GetReadValue( const std::string &pName )const{

		const CbcRegItem *cItem = GetRegItem( pName );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for %s does not exist.", pName.c_str() ) );
		return cItem->ReadValue();
	}
	UInt_t CbcRegList::GetReadValue( UInt_t pPage, UInt_t pAddr )const{

		const CbcRegItem *cItem = GetRegItem( pPage, pAddr );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for Page=%d, Address=0x%02X does not exist.", pPage, pAddr ) );
		return cItem->ReadValue();
	}
	void CbcRegList::Clear(){

		for( UInt_t i=0; i<size(); i++ ){
			delete at(i); 
		}
		clear();
	}
	void CbcRegList::SetValue0( const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getRegItem( pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for Name=%s.", pName.c_str() ) );
		cCRI->SetValue0(pValue );
	}
	void CbcRegList::SetValue0( UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getRegItem( pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for Page=0x%02X, Address=0x%02X.", pPage, pAddr ) );
		cCRI->SetValue0(pValue);
	}
	void CbcRegList::SetWrittenValue( const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getRegItem( pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for Name=%s.", pName.c_str() ) );
		cCRI->SetWrittenValue(pValue );
	}
	void CbcRegList::SetWrittenValue( UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getRegItem( pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for Page=0x%02X, Address=0x%02X.", pPage, pAddr ) );
		cCRI->SetWrittenValue(pValue);
	}
	void CbcRegList::SetReadValue( const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getRegItem( pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for Name=%s.", pName.c_str() ) );
		cCRI->SetReadValue(pValue );
	}
	void CbcRegList::SetReadValue( UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getRegItem( pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for Page=0x%02X, Address=0x%02X.", pPage, pAddr ) );
		cCRI->SetReadValue(pValue);
	}
	CbcRegItem *CbcRegList::getRegItem( const std::string &pName ){
		for( UInt_t i=0; i<size(); i++ ){
			if( at(i)->Name() == pName ) return at(i);
		}
		return 0;
	}
	CbcRegItem *CbcRegList::getRegItem( UInt_t pPage, UInt_t pAddr ){
		for( UInt_t i=0; i<size(); i++ ){
			if( at(i)->Page() == pPage && at(i)->Address() == pAddr ) return at(i);
		}
		return 0;
	}

	//FeCbcRegMap member functions
	const CbcRegList *FeCbcRegMap::GetCbcRegList( UInt_t pCbc )const{

		std::map<UInt_t, CbcRegList >::const_iterator cIt = this->find( pCbc );
		if( cIt == this->end() ) {
			return 0;
		}
		return &(cIt->second);
	}
	const CbcRegItem *FeCbcRegMap::GetCbcRegItem( UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const{

		const CbcRegList *cList = GetCbcRegList( pCbc );
		if( cList == 0 ) return 0;
		return cList->GetRegItem( pPage, pAddr );
	}
	const CbcRegItem *FeCbcRegMap::GetCbcRegItem( UInt_t pCbc, const std::string &pName )const{

		const CbcRegList *cList = GetCbcRegList( pCbc );
		if( cList == 0 ) return 0;
		return cList->GetRegItem( pName );
	}
	UInt_t FeCbcRegMap::GetReadValue( UInt_t pCbc, const std::string &pName )const{

		const CbcRegItem *cItem = GetCbcRegItem( pCbc, pName );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for CBC=%d, Name=%s does not exist.", pCbc, pName.c_str() ) ); 
		return cItem->ReadValue();
	}
	UInt_t FeCbcRegMap::GetReadValue( UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const{

		const CbcRegItem *cItem = GetCbcRegItem( pCbc, pPage, pAddr );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for CBC=%d, Page=%d, Address=0x%02X does not exist.", pCbc, pPage, pAddr ) ); 
		return cItem->ReadValue();
	}

	void FeCbcRegMap::Clear(){

		FeCbcRegMap::iterator cIt = begin();
		for( ; cIt != end(); cIt ++ ){
			cIt->second.Clear();
		}
		clear();
	}
	void FeCbcRegMap::AddCbc( UInt_t pCbc, CbcRegList pCC ){ 

		insert( std::pair<UInt_t, CbcRegList >( pCbc, pCC ) ); 
	}
	void FeCbcRegMap::AddItem( UInt_t pCbc, CbcRegItem *pItem ){ 

		CbcRegList *cCRL = getCbcRegList( pCbc );
		if(cCRL==0){
			AddCbc( pCbc, CbcRegList( fFeId, pCbc ) );
			cCRL = &(find(pCbc)->second);
		}
		cCRL->AddRegItem( pItem );
	}
	void FeCbcRegMap::SetValue0( UInt_t pCbc, const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pCbc, pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for CBC=%d, Name=%s.", pCbc, pName.c_str() ) );
		cCRI->SetValue0(pValue );
	}
	void FeCbcRegMap::SetValue0( UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pCbc, pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for CBC=%d, Page=0x%02X, Address=0x%02X.", pCbc, pPage, pAddr ) );
		cCRI->SetValue0(pValue);
	}
	void FeCbcRegMap::SetWrittenValue( UInt_t pCbc, const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pCbc, pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for CBC=%d, Name=%s.", pCbc, pName.c_str() ) );
		cCRI->SetWrittenValue(pValue );
	}
	void FeCbcRegMap::SetWrittenValue( UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pCbc, pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for CBC=%d, Page=0x%02X, Address=0x%02X.", pCbc, pPage, pAddr ) );
		cCRI->SetWrittenValue(pValue);
	}
	void FeCbcRegMap::SetReadValue( UInt_t pCbc, const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pCbc, pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for CBC=%d, Name=%s.", pCbc, pName.c_str() ) );
		cCRI->SetReadValue(pValue );
	}
	void FeCbcRegMap::SetReadValue( UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pCbc, pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for CBC=%d, Page=0x%02X, Address=0x%02X.", pCbc, pPage, pAddr ) );
		cCRI->SetReadValue(pValue);
	}
	CbcRegList *FeCbcRegMap::getCbcRegList( UInt_t pCbc ){

		CbcRegList *cCRL(0);
		std::map<UInt_t, CbcRegList >::iterator cIt = this->find( pCbc );
		if( cIt != this->end() ) {
			cCRL = &(cIt->second);
		}
		return cCRL;
	}
	CbcRegItem *FeCbcRegMap::getCbcRegItem( UInt_t pCbc, UInt_t pPage, UInt_t pAddr ){

		CbcRegList *cCRL = getCbcRegList( pCbc );
		if( cCRL == 0 ) return 0;  
		return cCRL->getRegItem( pPage, pAddr );
	}
	CbcRegItem *FeCbcRegMap::getCbcRegItem( UInt_t pCbc, const std::string &pName ){

		CbcRegList *cCRL = getCbcRegList( pCbc );
		if( cCRL == 0 ) return 0; 
		return cCRL->getRegItem( pName );
	}

	//CbcRegMap member functions
	const FeCbcRegMap *CbcRegMap::GetFeCbcRegMap( UInt_t pFe )const{

		std::map<UInt_t, FeCbcRegMap >::const_iterator cIt = this->find( pFe );
		if( cIt == end() ){
			return 0;
		}
		return &(cIt->second);
	}
	const CbcRegList *CbcRegMap::GetCbcRegList( UInt_t pFe, UInt_t pCbc )const{

		CbcRegMap::const_iterator cIt = find( pFe );
		if( cIt == end() ){
			return 0;
		}
		const FeCbcRegMap &cFeCbcRegMap = cIt->second;
		return cFeCbcRegMap.GetCbcRegList( pCbc );
	}
	const CbcRegItem *CbcRegMap::GetCbcRegItem( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const{

		const FeCbcRegMap *cMap = GetFeCbcRegMap(pFe);
		if( cMap == 0 ) return 0;	
		return cMap->GetCbcRegItem( pCbc, pPage, pAddr );
	}
	const CbcRegItem *CbcRegMap::GetCbcRegItem( UInt_t pFe, UInt_t pCbc, const std::string &pName )const{

		const FeCbcRegMap *cMap = GetFeCbcRegMap(pFe);
		if( cMap == 0 ) return 0;
		return cMap->GetCbcRegItem( pCbc, pName );
	}
	UInt_t CbcRegMap::GetValue0( UInt_t pFe, UInt_t pCbc, const std::string &pName )const{

		const CbcRegItem *cItem = GetCbcRegItem( pFe, pCbc, pName );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for FE=%d, CBC=%d, Name=%s does not exist.", pFe, pCbc, pName.c_str() ) ); 
		return cItem->Value0();
	}
	UInt_t CbcRegMap::GetValue0( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const{

		const CbcRegItem *cItem = GetCbcRegItem( pFe, pCbc, pPage, pAddr );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for FE=%d, CBC=%d, Page=%d, Address=0x%02X does not exist.", pFe, pCbc, pPage, pAddr ) ); 
		return cItem->Value0();
	}
	UInt_t CbcRegMap::GetWrittenValue( UInt_t pFe, UInt_t pCbc, const std::string &pName )const{

		const CbcRegItem *cItem = GetCbcRegItem( pFe, pCbc, pName );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for FE=%d, CBC=%d, Name=%s does not exist.", pFe, pCbc, pName.c_str() ) ); 
		return cItem->WrittenValue();
	}
	UInt_t CbcRegMap::GetWrittenValue( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const{

		const CbcRegItem *cItem = GetCbcRegItem( pFe, pCbc, pPage, pAddr );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for FE=%d, CBC=%d, Page=%d, Address=0x%02X does not exist.", pFe, pCbc, pPage, pAddr ) ); 
		return cItem->WrittenValue();
	}
	UInt_t CbcRegMap::GetReadValue( UInt_t pFe, UInt_t pCbc, const std::string &pName )const{

		const CbcRegItem *cItem = GetCbcRegItem( pFe, pCbc, pName );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for FE=%d, CBC=%d, Name=%s does not exist.", pFe, pCbc, pName.c_str() ) ); 
		return cItem->ReadValue();
	}
	UInt_t CbcRegMap::GetReadValue( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr )const{

		const CbcRegItem *cItem = GetCbcRegItem( pFe, pCbc, pPage, pAddr );
		if( cItem == 0 ) throw Exception( Form( "CbcRegItem for FE=%d, CBC=%d, Page=%d, Address=0x%02X does not exist.", pFe, pCbc, pPage, pAddr ) ); 
		return cItem->ReadValue();
	}
	void CbcRegMap::Clear(){

		CbcRegMap::iterator cIt = begin();
		for( ; cIt != end(); cIt ++ ){
			cIt->second.Clear();
		}
		clear();
	}
	void CbcRegMap::ClearRegisters(){

		CbcRegMap::iterator cIt = begin();
		for( ; cIt != end(); cIt ++ ){
			FeCbcRegMap &cFeCbcRegMap = cIt->second;
			FeCbcRegMap::iterator cIt2 = cFeCbcRegMap.begin();
			for( ; cIt2 != cFeCbcRegMap.end(); cIt2 ++ ){
				cIt2->second.Clear();
			}
		}
	}
	void CbcRegMap::ClearRegisters( UInt_t pFe, UInt_t pCbc ){

		CbcRegMap::iterator cIt = find( pFe );
		if( cIt == end() ) {
			throw Exception( Form( "Invalid FE = %d.", pFe ) );
		}
		FeCbcRegMap &cFeCbcRegMap = cIt->second;
		FeCbcRegMap::iterator cIt2 = cFeCbcRegMap.find( pCbc );
		if( cIt2 == cFeCbcRegMap.end() ) {
			throw Exception( Form( "Invalid CBC = %d in FE = %d.", pCbc, pFe ) );
		}
		cIt2->second.Clear();
	}
	void CbcRegMap::Reset( UInt_t pNFe, UInt_t pNCbc ){

		Clear();
		for( UInt_t cFe=0; cFe < pNFe; cFe++ ){

			FeCbcRegMap cFeCbcRegMap;

			for( UInt_t cCbc=0; cCbc < pNCbc; cCbc++ ){

				cFeCbcRegMap.insert( std::pair<UInt_t, CbcRegList>( cCbc, CbcRegList( cFe, cCbc ) ) ); 
			}
			insert( std::pair<UInt_t, FeCbcRegMap>(cFe, cFeCbcRegMap ) );
		}
	}
	void CbcRegMap::SetCbcRegSettingFileName( UInt_t pFeId, UInt_t pCbcId, const std::string &pName ){

		FeCbcRegMap *cFeCbcRegMap = getFeCbcRegMap( pFeId );
		FeCbcRegMap::iterator cIt = cFeCbcRegMap->find( pCbcId );
		if( cIt == cFeCbcRegMap->end() ){
			throw Exception( Form( "CbcRegMap::SetCbcRegSettingFileName() Invalid CBC. [FE: %d, CBC: %d] does not exist.", pFeId, pCbcId ) );
		}
		cIt->second.SetFileName( pName );
	}
	void CbcRegMap::AddFe( UInt_t pFe ){ 

		insert( std::pair<UInt_t, FeCbcRegMap >( pFe, FeCbcRegMap(pFe) ) ); 
	}
	void CbcRegMap::AddItem( UInt_t pFe, UInt_t pCbc, CbcRegItem *pItem ){ 

		FeCbcRegMap *cCRL = getFeCbcRegMap( pFe );
		if( cCRL == 0 ){
			AddFe( pFe );
			cCRL =  &(this->find(pFe)->second);
		}
		cCRL->AddItem( pCbc, pItem );
	}
	const CbcRegItem *CbcRegMap::SetValue0( UInt_t pFe, UInt_t pCbc, const std::string &pName, UInt_t pValue0 ){

		CbcRegItem *cCRI = getCbcRegItem( pFe, pCbc, pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for FE=%d, CBC=%d, Name=%s.", pFe, pCbc, pName.c_str() ) );
		cCRI->SetValue0( pValue0 );
		return cCRI;
	}
	const CbcRegItem *CbcRegMap::SetValue0( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue0 ){

		CbcRegItem *cCRI = getCbcRegItem( pFe, pCbc, pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for FE=%d, CBC=%d, Page=0x%02X, Address=0x%02X.", pFe, pCbc, pPage, pAddr ) );
		cCRI->SetValue0( pValue0 );
		return cCRI;
	}
	const CbcRegItem *CbcRegMap::SetWrittenValue( UInt_t pFe, UInt_t pCbc, const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pFe, pCbc, pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for FE=%d, CBC=%d, Name=%s.", pFe, pCbc, pName.c_str() ) );
		cCRI->SetWrittenValue( pValue );
		return cCRI;
	}
	const CbcRegItem *CbcRegMap::SetWrittenValue( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pFe, pCbc, pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for FE=%d, CBC=%d, Page=0x%02X, Address=0x%02X.", pFe, pCbc, pPage, pAddr ) );
		cCRI->SetWrittenValue( pValue );
		return cCRI;
	}
	const CbcRegItem *CbcRegMap::SetReadValue( UInt_t pFe, UInt_t pCbc, const std::string &pName, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pFe, pCbc, pName );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for FE=%d, CBC=%d, Name=%s.", pFe, pCbc, pName.c_str() ) );
		cCRI->SetReadValue( pValue );
		return cCRI;
	}
	const CbcRegItem *CbcRegMap::SetReadValue( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr, UInt_t pValue ){

		CbcRegItem *cCRI = getCbcRegItem( pFe, pCbc, pPage, pAddr );
		if( cCRI == 0 ) throw Exception( Form( "CbcRegItem is not found for FE=%d, CBC=%d, Page=0x%02X, Address=0x%02X.", pFe, pCbc, pPage, pAddr ) );
		cCRI->SetReadValue( pValue );
		return cCRI;
	}
	FeCbcRegMap *CbcRegMap::getFeCbcRegMap( UInt_t pFe ){

		FeCbcRegMap *cMap(0);
		std::map<UInt_t, FeCbcRegMap >::iterator cIt = this->find( pFe );
		if( cIt != this->end() ){
			cMap = &(cIt->second);
		}
		return cMap;
	}
	CbcRegList *CbcRegMap::getCbcRegList( UInt_t pFe, UInt_t pCbc ){

		FeCbcRegMap *cMap = getFeCbcRegMap( pFe );
		if( !cMap ) return 0;
		return cMap->getCbcRegList( pCbc );
	}
	CbcRegItem *CbcRegMap::getCbcRegItem( UInt_t pFe, UInt_t pCbc, UInt_t pPage, UInt_t pAddr ){

		FeCbcRegMap *cMap = getFeCbcRegMap( pFe );
		if( !cMap ) return 0;
		return cMap->getCbcRegItem( pCbc, pPage, pAddr );
	}
	CbcRegItem *CbcRegMap::getCbcRegItem( UInt_t pFe, UInt_t pCbc, const std::string &pName ){

		FeCbcRegMap *cMap = getFeCbcRegMap( pFe );
		if( !cMap ) return 0;
		return cMap->getCbcRegItem( pCbc, pName );
	}
}

