#ifndef __RADTEST_CONTROLLER_H__
#define __RADTEST_CONTROLLER_H__
#include <fstream>
//#include <netinet/in.h>
#include <TQObject.h>
#include <TString.h>
#include <map>
#include "Cbc/CbcRegInfo.h"
#include "CbcDaq/DAQController.h"
//#include "TestGroup.h"

class TPad;

namespace Strasbourg{

	class Data;
	class Event;
}

namespace Analysers{
	class ErrorAnalyser;
}
namespace RadTest{
	typedef std::pair< std::string, std::string > SettingItem;
	typedef std::map< std::string, std::string >  Setting; 
}


using namespace Cbc;
using namespace Strasbourg;
using namespace CbcDaq;
using namespace Analysers;

namespace RadTest{

	class Controller: public DAQController {

		public:
			enum RunType { Type1=1, NRunTypes };

			Controller( const char *pConfigFile = 0 );
			~Controller();

			//main functions
			virtual void ConfigureAnalyser();
			virtual void ConfigureRun();
			virtual void Run();
			void RunType1();

			//functions for configuration on the fly
//			void SetRadTestSetting( const char *pNode, UInt_t pValue ){ fRadTestSetting.find( pNode )->second = pValue; }		

			//functions for interactive study 
			void SetSetting( const char *pNode, UInt_t pValue ){ fSetting.find( pNode )->second = pValue; }		
			const Setting &GetSetting()const{ return fSetting; }
			std::string GetSetting( const char *pNode ) const { return fSetting.find(pNode )->second; }

		private:
			void initialiseSetting(); //Called by DAQController::Initialise()
			void ReadSettingFile( const char *pFname );

			Setting								fSetting;

			std::vector<UInt_t>             fGroupList;

	};
}
#endif

