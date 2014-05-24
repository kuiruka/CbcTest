#include "TestGroup.h"

namespace ICCalib{
	void TestGroup::AddChannel( Channel *pChannel ){ 

		fChannelList.push_back( pChannel ); 
	}
	bool TestGroup::Has( UInt_t pChannel )const{ 
		for( UInt_t i=0; i < size(); i++ ) {
			if( at(i) == pChannel ) return true;
		}
		return false;
	}
	TestGroup *TestGroupMap::GetGroup( UInt_t pGroupId ){

		TestGroupMap::iterator cIt = find( pGroupId );
		if( cIt != end() ) return &cIt->second;
		else return 0;
	}
	UInt_t TestGroupMap::GetGroupId( UInt_t pChannel )const{

		TestGroupMap::const_iterator cIt = begin();
		for( ; cIt != end(); cIt++ ){
			TestGroup cTG = cIt->second;
			for( UInt_t i=0; i < cTG.size(); i++ ){
				if( cTG[i] == pChannel ) return cIt->first;
			}
		}
		std::cout << "The channel is not found in the TestGroupMap." << std::endl;
		return 0;
	}
	TestGroup *TestGroupMap::GetActivatedGroup(){

		if( fActivatedGroup != -1 ){
			return GetGroup( fActivatedGroup );
		}
		else return 0;
	}
	TestGroup *TestGroupMap::GetActivatedGroup(UInt_t &pGroupId ){

		if( fActivatedGroup != -1 ){
			pGroupId = fActivatedGroup;
			return GetGroup( fActivatedGroup );
		}
		else return 0;
	}
	Int_t TestGroupMap::Activate( UInt_t pGroupId ){

		if( fActivatedGroup == (Int_t) pGroupId ) return fActivatedGroup;
		else if( fActivatedGroup != -1 ){
			TestGroupMap::iterator cIt = find( pGroupId );
			cIt->second.Activate(0);
			fActivatedGroup = -1;
		}

		TestGroupMap::iterator cIt = find( pGroupId );
		if( cIt == end() ){
			return fActivatedGroup;
		}
		cIt->second.Activate(1);
		fActivatedGroup = pGroupId;
		return fActivatedGroup;
	}
}

