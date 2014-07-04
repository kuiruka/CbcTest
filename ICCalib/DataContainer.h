#ifndef __DATACONTAINER_H__
#define __DATACONTAINER_H__

#include <map>
#include <vector>
#include <TROOT.h>
#include <TH1F.h>

#include <TCanvas.h>

class TPad;
class TH1F;
class TGraphErrors;
namespace ICCalib{

	template <typename ChannelData, typename CbcData> class DataContainer; 

	template<typename ChannelData> class Channel {
		friend class ScurveAnalyser;
		public:

			Channel( UInt_t pFeId=0, UInt_t pCbcId=0, UInt_t pChannelId=0 ):
				fFeId(pFeId), fCbcId(pCbcId), fChannelId(pChannelId), fData(){}

			Channel( const Channel &pC ): fFeId(pC.fFeId), fCbcId(pC.fCbcId), fChannelId(pC.fChannelId), fData(pC.fData){} 

			~Channel(){}

			void Id( UInt_t &pFeId, UInt_t &pCbcId, UInt_t &pChannelId ){ pFeId = fFeId; pCbcId = fCbcId; pChannelId = fChannelId; }
			UInt_t FeId()const{ return fFeId; } 
			UInt_t CbcId()const{ return fCbcId; } 
			UInt_t ChannelId()const{ return fChannelId; }
			const ChannelData &GetData()const{ return fData; }
			void SetData( ChannelData &pData ){ fData = pData; }

		private:
			ChannelData &getData(){ return fData; }
			UInt_t fFeId;
			UInt_t fCbcId;
			UInt_t fChannelId;
			ChannelData fData;
	};

	template <typename ChannelData, typename CbcData> 
		class CbcInfo: public std::map<UInt_t, Channel<ChannelData> * >{

			friend class ScurveAnalyser;

			public:
			typedef typename std::map<UInt_t, Channel<ChannelData> * > Map;
			typedef typename Map::const_iterator const_iterator;
			const_iterator begin()const{ return ( (Map *)this )->begin(); }
			const_iterator end() const { return ( (Map *)this )->end(); }
			const_iterator find( UInt_t pId )const{ return ( (Map *)this )->find(pId); }

			typedef typename Map::iterator iterator;
			iterator begin(){ return ( (Map *)this )->begin(); }
			iterator end(){ return ( (Map *)this )->end(); }
			iterator find( UInt_t pId ){ return ( (Map *)this )->find(pId); }

			public:
			CbcInfo( UInt_t pFeId, UInt_t pCbcId ): std::map<UInt_t, Channel<ChannelData> * >(), fFeId(pFeId), fCbcId(pCbcId){}
			CbcInfo(const CbcInfo &pC): std::map<UInt_t, Channel<ChannelData> * >(pC), fFeId(pC.fFeId), fCbcId(pC.fCbcId), fData(pC.fData){}
			~CbcInfo(){}

			UInt_t FeId()const{ return fFeId; } 
			UInt_t CbcId()const{ return fCbcId; } 
			const Channel<ChannelData> * GetChannel( UInt_t pChannelId )const;
			void AddChannel( UInt_t pChannelId, Channel<ChannelData> * pChannel );
			const CbcData &GetData()const{ return fData; }

			private:
			CbcData &getData(){ return fData; }
			UInt_t                           fFeId;
			UInt_t                           fCbcId;
			CbcData                          fData;
		};

	template <typename ChannelData, typename CbcData> 
		class FeInfo: public std::map<UInt_t, CbcInfo<ChannelData, CbcData> >{ 

			friend class DataContainer<ChannelData, CbcData>;

			public:
			typedef typename std::map<UInt_t, CbcInfo<ChannelData, CbcData> > Map;
			typedef typename Map::const_iterator const_iterator;
			const_iterator begin()const{ return ( (Map *)this )->begin(); }
			const_iterator end() const { return ( (Map *)this )->end(); }
			const_iterator find( UInt_t pId )const{ return ( (Map *)this )->find(pId); }

			typedef typename Map::iterator iterator;
			iterator begin(){ return ( (Map *)this )->begin(); }
			iterator end(){ return ( (Map *)this )->end(); }
			iterator find( UInt_t pId ){ return ( (Map *)this )->find(pId); }

			public:
			FeInfo(UInt_t pFeId): fFeId(pFeId), fCbcIds(){}
			FeInfo(const FeInfo &pFeInfo): std::map<UInt_t, CbcInfo<ChannelData, CbcData> >(pFeInfo), fFeId(pFeInfo.fFeId), fCbcIds(pFeInfo.fCbcIds){}
			~FeInfo(){}
			const std::vector<UInt_t> &GetCbcIds(){ return fCbcIds; }
			const CbcInfo<ChannelData, CbcData> *GetCbcInfo( UInt_t pCbcId )const;
			const Channel<ChannelData> *GetChannel( UInt_t pCbcId, UInt_t pChannelId )const;
			void AddCbcInfo( UInt_t pCbcId, CbcInfo<ChannelData, CbcData> pCbcInfo ); 
			void AddChannel( UInt_t pCbcId, UInt_t pChannelId, Channel<ChannelData> *pChannel ); 
			private:
			CbcInfo<ChannelData, CbcData> *getCbcInfo( UInt_t pCbcId );
			UInt_t fFeId;
			std::vector<UInt_t> fCbcIds;
		};

	template <typename ChannelData, typename CbcData> 
		class DataContainer : public std::map<UInt_t, FeInfo<ChannelData, CbcData> >{ 

			friend class ScurveAnalyser;

			public:
			typedef typename std::map<UInt_t, FeInfo<ChannelData, CbcData> > Map;
			typedef typename Map::const_iterator const_iterator;
			const_iterator begin()const{ return ( (Map *)this )->begin(); }
			const_iterator end() const { return ( (Map *)this )->end(); }
			const_iterator find( UInt_t pId )const{ return ( (Map *)this )->find(pId); }

			typedef typename Map::iterator iterator;
			iterator begin(){ return ( (Map *)this )->begin(); }
			iterator end(){ return ( (Map *)this )->end(); }
			iterator find( UInt_t pId ){ return ( (Map *)this )->find(pId); }

			public:
			DataContainer(){}
			DataContainer( const DataContainer &pC ): std::map<UInt_t, FeInfo<ChannelData, CbcData> >(pC), fFeIds(pC.fFeIds){}
			~DataContainer(){}
			const std::vector<UInt_t> GetFeIds()const{ return fFeIds; }
			const FeInfo<ChannelData, CbcData> *GetFeInfo( UInt_t pFeId )const;
			const CbcInfo<ChannelData, CbcData> *GetCbcInfo( UInt_t pFeId, UInt_t pCbcInfo )const;
			const Channel<ChannelData> *GetChannel( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannelId )const;
			void AddFeInfo( UInt_t pFeId, FeInfo<ChannelData, CbcData> pFeInfo );
			void AddCbcInfo( UInt_t pFeId, UInt_t pCbcId, CbcInfo<ChannelData, CbcData> pCbcInfo );
			void AddChannel( UInt_t pFeId, UInt_t pCbcId, UInt_t pChannelId, Channel<ChannelData> *pChannel ); 

			private:
			FeInfo<ChannelData, CbcData> *getFeInfo( UInt_t pFeId );
			CbcInfo<ChannelData, CbcData> *getCbcInfo( UInt_t pFeId, UInt_t pCbcId );
			std::vector<UInt_t> fFeIds;
		};

}
#include "ICCalib/DataContainer.icc"
#endif


