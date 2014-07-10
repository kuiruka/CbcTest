#ifndef __DAQCONTROLLER_H__
#define __DAQCONTROLLER_H__
#include <fstream>
//#include <netinet/in.h>
#include <TQObject.h>
#include <TString.h>
#include <map>
#include "Cbc/CbcRegInfo.h"
#include "Cbc/TestGroup.h"
#include "Analyser.h"

class TPad;
class TGTextView;

namespace uhal{
	class HwInterface;
}

namespace Strasbourg{

	class HwController;
	class Data;
	class Event;
	typedef std::map< std::string, UInt_t > GlibSetting; 
	typedef std::pair< std::string, std::string > CbcConfigFilePair;
	typedef std::map< std::string, std::string > CbcConfigFileMap;
}
using namespace Cbc;
using namespace Strasbourg;


namespace CbcDaq{

	class GUIFrame;

	typedef std::pair< std::string, std::string > ConfigItem;
	typedef std::map< std::string, std::string > Configuration; 

	typedef std::pair< std::string, UInt_t > RunItem;
	typedef std::map< std::string, UInt_t > RunSetting; 

	class DAQController: public TQObject {

		public:
			static UInt_t NBe;

		public:
			DAQController( const char *pAnalyserName, const char *pConfigFile = 0 );
			virtual ~DAQController();

			//main functions
			virtual void Initialise();
			virtual void ConfigureGlib();
			virtual void InitialiseGlib();
			virtual void ConfigureCbc();
			virtual void ConfigureAnalyser();
			virtual void ConfigureRun();
			virtual void Run();

			//functions for configuration on the fly
			virtual void SetUhalConfigFileName( const char *pName ){ fUhalConfigFileName = pName; }
			virtual void SetBoardId( const char *pId ){ fBoardId = pId; }
			virtual void SetGlibSetting( const char *pName, UInt_t pValue );
			virtual void SetCbcRegSettingFileName( UInt_t pFeId, UInt_t pCbcId, std::string pName ); 
			virtual void AddCbcRegUpdateItem( const CbcRegItem *pCbcRegItem, UInt_t pValue );
			virtual void UpdateCbcRegValues();
			virtual std::vector<const CbcRegItem *> ResetCbcRegUpdateList();
			virtual void SetRunSetting( const char *pName, UInt_t pValue ){ fRunSetting.find( pName )->second = pValue; }

			//functions for interactive study 
			virtual const std::string &GetConfigurationFileName()const{ return fConfigurationFileName; }
			virtual const std::string &GetUhalConfigFileName()const{ return fUhalConfigFileName; }
			virtual const std::string &GetBoardId()const{ return fBoardId; }
			virtual const GlibSetting &GetGlibSetting()const;
			virtual UInt_t GetGlibSetting( const char * pName )const;
			virtual const CbcRegMap &GetCbcRegSetting()const;
			virtual const RunSetting &GetRunSetting()const{ return fRunSetting; }
			virtual UInt_t GetRunSetting( const char *pNode ) const { return fRunSetting.find(pNode )->second; }
			virtual bool GetShowDataStream()const{ return fAnalyser->ShowDataStream(); }
			virtual UInt_t GetNFe()const{ return fNFe; }
			virtual UInt_t GetNCbc()const{ return fNCbc; }

			virtual void SetGUIFrame( GUIFrame *pGUIFrame ){ fGUIFrame = pGUIFrame; }
			virtual void SetAnalyserHistPad( UInt_t pFeId, UInt_t pCbcId, TPad *pPad );	
			virtual void SetAnalyserTextView( TGTextView *pTextView );
			virtual void ReadSettingFile( const char *pFname );
			virtual void Stop(){ fStop = true; }
			virtual void CbcHardReset();
			virtual void ShowDataStream( bool pShowData ){ fAnalyser->ShowDataStream( pShowData ); }
			virtual void ReConfigureCbc( UInt_t pFe, UInt_t pCbc );
			virtual void SaveCbcRegInfo( UInt_t pFe, UInt_t pCbc );
			virtual void SaveCbcRegInfo( const char *pNewDir = 0 );

		protected:
			virtual void initialiseSetting();

			TString 						fAnalyserName;
			GUIFrame                        *fGUIFrame;

			std::string                     fConfigurationFileName;
			Configuration                   fConfiguration;
			std::string                     fUhalConfigFileName;
			std::string                     fBoardId;
			CbcConfigFileMap                fCbcConfigFileMap;
			std::string                     fOutputDir;
			std::ofstream                   *fDataFile;
			RunSetting                      fRunSetting;

			HwController                    *fHwController;
			Analyser                        *fAnalyser;

			UInt_t							fBeId;
			UInt_t                          fNFe;
			UInt_t                          fNCbc;
			unsigned int                    fNAcq;
			unsigned int                    fNeventPerAcq;
			bool                            fNegativeLogicCBC;
			bool                            fStop;

			TestGroupMap                         *fTestPulseGroupMap;
	};
}
#endif

