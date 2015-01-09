/*
	VCthScan( MinVCth, MaxVCth )
		Scan VCth from MinVCth to MaxVCth. 
		Single data aqcuisition for fNeventPerAcq events is done and ProcessCalibrationData() is called for each VCth.
		ScurveAnalyser->Configure() should be called before this method.

	ProcessCalibrationData( VCth, Data )
		Loop over events in Data and call ScurveAnalyser->FillHists( VCth, Event ) which fill the histogram of hit count distribution over VCth for each channel.	

	ActivateGroup()
		Should be called before ConfigureCbcOffset( OffsetTargetBit, MinVCth, MaxVCth )

	ScurveAnalyser->SetOffsets( TargetOffset ) 
		Set all offsets to the TargetOffset.

	ScurveAnalyser->Configure( OffsetTargetBit, Vplus )
		Reset histograms, update offsets, set target bit, set Vplus

	ConfigureCbcOffset( OffsetTargetBit, MinVCth, MaxVCth )
		ActivateGroup() should be called before this method.
		Offset value should be set in ScurvaAnalyser for channels in the test group. This is done in either ScurvaAnalyser->Configure() or ScurvaAnalyser->SetInitialOffset().
		For each channel,
			If OffsetTargetBit == 8 && not the cannel in test group, do nothing.
			If the channel is in test group, offset is set to the value from ScurveAnalyser, otherwise, fNonTestGroupOffset is set.

	ConfigureVplusScan( Vplus )
		Set Vplus.
			
*/
#ifndef __CALIBRATOR_H__
#define __CALIBRATOR_H__
#include <fstream>
//#include <netinet/in.h>
#include <TQObject.h>
#include <TString.h>
#include <map>
#include "Cbc/CbcRegInfo.h"
#include "CbcDaq/DAQController.h"
#include "Analysers/GUIData.h"
//#include "TestGroup.h"

class TPad;

namespace Strasbourg{

	class Data;
	class Event;
}

namespace Analysers{
	class ScurveAnalyser;
	class CalibrationChannelData;
	class CalibrationCbcData;
	typedef DataContainer<CalibrationChannelData, CalibrationCbcData> CalibrationResult; 
}
namespace ICCalib{
	typedef std::pair< std::string, UInt_t > CalibItem;
	typedef std::map< std::string, UInt_t > CalibSetting; 
}

namespace Cbc{
	template <class T> class _TestGroup;
	template <class T> class _TestGroupMap;
	typedef _TestGroup<UInt_t>                    TestGroup;
	typedef _TestGroupMap<UInt_t>                 TestGroupMap;
	typedef _TestGroup<Analysers::Channel<Analysers::CalibrationChannelData> >   CalibrationTestGroup;
	typedef _TestGroupMap<Analysers::Channel<Analysers::CalibrationChannelData> > CalibrationTestGroupMap;
}

using namespace Cbc;
using namespace Strasbourg;
using namespace CbcDaq;
using namespace Analysers;

namespace ICCalib{

	class Calibrator: public DAQController {

		public:
			Calibrator( const char *pConfigFile = 0 );
			~Calibrator();

			//main functions
			void ConfigureAnalyser();
			void Calibrate();
			void VCthScanForAllGroups();  

			//functions for configuration on the fly
			void SetCalibSetting( const char *pNode, UInt_t pValue ){ fCalibSetting.find( pNode )->second = pValue; }		

			//functions for interactive study 
			const CalibSetting &GetCalibSetting()const{ return fCalibSetting; }
			UInt_t GetCalibSetting( const char *pNode ) const { return fCalibSetting.find(pNode )->second; }
			const TestGroupMap *GetTestPulseGroupMap()const{ return fTestPulseGroupMap; }
			const CalibrationTestGroupMap *GetTestGroupMap()const{ return fTestGroupMap; }
			const std::vector<UInt_t> &GetGroupList()const{ return fGroupList; }
			UInt_t GetCurrentTestPulseGroup()const{ return fCurrentTestPulseGroup; }
			const CalibrationResult *GetCalibrationResult()const;

			void SetVplusVsVCth0GraphDisplayPad( UInt_t pFeId, TPad *pPad );
			void SetScurveHistogramDisplayPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );
			void ReadSettingFile( const char *pFname ); //Called by DAQController::Initialise()

			//functions used internally for a moment.
			Int_t GetDataStreamHistId( UInt_t pFeId, UInt_t pCbcId );
			void FindVplus();
			void CalibrateOffsets();

			void ConfigureVplusScan( UInt_t pVplus );
			void ConfigureCbcOffset( Int_t pTargetOffsetBit, UInt_t &pMinVCth, UInt_t &pMaxVCth );

			void VCthScanForVplusCalibration( UInt_t pVplus ); 
			void VCthScanForOffsetCalibration( Int_t pTargetBit ); 

			void VCthScan( UInt_t &pMinVCth, UInt_t &pMaxVCth );
			void PrintScurveHistogramDisplayPads();
			void DrawVplusVCthScanResult();
			void PrintVplusScan();
			void SetCalibratedOffsets();

		private:
			void initialiseSetting(); //Called by DAQController::Initialise()

			CalibSetting                    fCalibSetting;

			ScurveAnalyser                       *fScurveAnalyser;
			CalibrationTestGroupMap              *fTestGroupMap;

			std::vector<UInt_t>             fGroupList;

			UInt_t                          fNonTestGroupOffset;
//			UInt_t							fNAcq;
	};
}
#endif

