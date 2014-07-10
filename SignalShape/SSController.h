#ifndef __SSCONTROLLER_H__
#define __SSCONTROLLER_H__

#include "CbcDaq/DAQController.h"

namespace Strasbourg{

	class Data;
	class Event;
}

namespace Analysers{

	class SignalShapeAnalyser;
	template <class ChannelData > class Channel;
	class SignalShapeChannelData;
}

namespace Cbc{

	class CbcRegMap;
	template <class T> class _TestGroup;
	template <class T> class _TestGroupMap;
	typedef _TestGroup<UInt_t>                    TestGroup;
	typedef _TestGroupMap<UInt_t>                 TestGroupMap;
	typedef _TestGroup<Analysers::Channel<Analysers::SignalShapeChannelData> >   SignalShapeTestGroup;
	typedef _TestGroupMap<Analysers::Channel<Analysers::SignalShapeChannelData> > SignalShapeTestGroupMap;
}

using namespace Cbc;
using namespace Strasbourg;
using namespace CbcDaq;
using namespace Analysers;

namespace SignalShape{

	typedef std::pair< std::string, UInt_t > SSItem;
	typedef std::map< std::string, UInt_t > SSSetting; 

	class SSController: public DAQController {


		public:
			SSController( const char *pConfigFile = 0 );
			~SSController();
			//main functions
			void ConfigureAnalyser();
			void Run();
			void ActivateGroup( UInt_t pGroupId );
			void VCthScan( UInt_t &pMinVCth, UInt_t &pMaxVCth );

			void SetSignalShapeGraphDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );
			void SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );
			void ReadSettingFile( const char *pFname ); //Called by DAQController::Initialise()

			void ConfigureCbcOffset( UInt_t &pMinVCth, UInt_t &pMaxVCth );
			void InitialiseOffsets();
			void initialiseSetting(); //Called by DAQController::Initialise()

			void SetSSSetting( const char *pNode, UInt_t pValue ){ fSSSetting.find( pNode )->second = pValue; }		

			//functions for interactive study 
			const SSSetting &GetSSSetting()const{ return fSSSetting; }
			UInt_t GetSSSetting( const char *pNode ) const { return fSSSetting.find(pNode )->second; }

		private:

			SSSetting                            fSSSetting;

			SignalShapeAnalyser                  *fSignalShapeAnalyser;
			SignalShapeTestGroupMap              *fTestGroupMap;

			std::vector<UInt_t>                  fGroupList;
			Int_t                                fCurrentTestPulseGroup;
			UInt_t                               fNonTestGroupOffset;
	};

}

#endif

