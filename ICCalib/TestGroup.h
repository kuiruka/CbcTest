#ifndef __TESTGROUP_H__
#define __TESTGROUP_H__
#include <TROOT.h>
#include <map>
#include <vector>
#include "CalibrationResult.h"

class TGraphErrors;

namespace ICCalib{

	class TestGroup : public std::vector<UInt_t>{
		public:
			TestGroup():fChannelList(std::vector<Channel *>(0)), fActivated(0){}
			TestGroup(const TestGroup &pT): std::vector<UInt_t>(pT), fChannelList(pT.fChannelList), fActivated(pT.fActivated){}
			~TestGroup(){}

			const std::vector<Channel *> *GetChannelList()const{ return &fChannelList; }
			bool Has( UInt_t pChannel )const; 
			bool Activated()const{ return fActivated; }

			void AddChannel( Channel *pChannel );
			void Activate( bool pActivate ){ fActivated = pActivate; }

			void ClearChannelList(){ fChannelList.clear(); } 

		private:
			std::vector<Channel *> fChannelList;
			bool fActivated;
	};

	class TestGroupMap : public std::map< UInt_t, TestGroup >{

		public:
			TestGroupMap():fActivatedGroup(-1){}
			~TestGroupMap(){}
			TestGroup *GetGroup( UInt_t pGroupId );
			UInt_t GetGroupId( UInt_t pChannel )const;
			TestGroup *GetActivatedGroup();
			TestGroup *GetActivatedGroup(UInt_t &pGroupId );
			Int_t Activate( UInt_t pGroupId ); 
		private:
			Int_t fActivatedGroup;
	};
}
#endif

