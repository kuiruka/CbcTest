#ifndef __TESTGROUP_H__
#define __TESTGROUP_H__
#include <TROOT.h>
#include <map>
#include <vector>
#include "AnalysedData.h"

class TGraphErrors;

namespace ICCalib{

	template <class T> class _TestGroup;
	template <class T> class _TestGroupMap;
	typedef _TestGroup<UInt_t>    TestGroup;
	typedef _TestGroupMap<UInt_t> TestGroupMap;
	template <class T>
		class _TestGroup : public std::vector<UInt_t>{
			public:
				_TestGroup():fChannelList(std::vector<Channel<T> *>(0)), fActivated(0){}
				_TestGroup(const _TestGroup &pT): std::vector<UInt_t>(pT), fChannelList(pT.fChannelList), fActivated(pT.fActivated){}
				~_TestGroup(){}

				const std::vector<Channel<T> *> *GetChannelList()const{ return &fChannelList; }
				bool Has( UInt_t pChannel )const; 
				bool Activated()const{ return fActivated; }

				void AddChannel( Channel<T> *pChannel );
				void Activate( bool pActivate ){ fActivated = pActivate; }

				void ClearChannelList(){ fChannelList.clear(); } 

			private:
				std::vector<Channel<T> *> fChannelList;
				bool fActivated;
		};
	template <class T>
		class _TestGroupMap : public std::map< UInt_t, _TestGroup<T> >{

			public:
				typedef typename std::map<UInt_t, _TestGroup<T> > Map;
				typedef typename Map::const_iterator const_iterator;
				const_iterator begin()const{ return ((Map *)this)->begin(); }
				const_iterator end() const { return ((Map *)this)->end(); }
				const_iterator find( UInt_t pGroupId )const{ return ((Map *)this)->find(pGroupId); }

				typedef typename Map::iterator iterator;
				iterator begin(){ return ((Map *)this)->begin(); }
				iterator end(){ return ((Map *)this)->end(); }
				iterator find( UInt_t pGroupId ){ return ((Map *)this)->find(pGroupId); }

				_TestGroupMap():fActivatedGroup(-1){}
				_TestGroupMap( TestGroupMap &pTestGroupMap ); 
				~_TestGroupMap(){}
				_TestGroup<T> *GetGroup( UInt_t pGroupId );
				UInt_t GetGroupId( UInt_t pChannel )const;
				_TestGroup<T> *GetActivatedGroup();
				_TestGroup<T> *GetActivatedGroup(UInt_t &pGroupId );
				Int_t Activate( UInt_t pGroupId ); 
			private:
				Int_t fActivatedGroup;
		};

}
#include "TestGroup.icc"
#endif

