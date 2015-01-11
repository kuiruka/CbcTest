#ifndef __SEUTESTCONTROLLER_H__
#define __SEUTESTCONTROLLER_H__
#include <fstream>
//#include <netinet/in.h>
#include <TQObject.h>
#include <TString.h>
#include "CbcDaq/DAQController.h"

using namespace Cbc;
using namespace Strasbourg;
using namespace Analysers;
using namespace CbcDaq;

namespace Analysers{
	class ErrorAnalyser;
}

namespace SEUTest{

	typedef std::pair< std::string, UInt_t > SEUTestItem;
	typedef std::map< std::string, UInt_t > SEUTestSetting; 

	class SEUTestController: public DAQController {

		public:
			enum RunType { Type1=1, Type2, NRunTypes };

		public:
			SEUTestController( const char *pConfigFile = 0 );
			virtual ~SEUTestController();

			//main functions
			//virtual void Initialise();
			//virtual void ConfigureGlib();
			//virtual void InitialiseGlib();
			//virtual void ConfigureCbc();
			virtual void ConfigureAnalyser();
			virtual void ConfigureRun();
			virtual void Run();
			void RunType1();
			void RunType2();

			void SetSEUTestSetting( const char *pNode, UInt_t pValue ){ fSEUTestSetting.find( pNode )->second = pValue; }		
			const SEUTestSetting &GetSEUTestSetting()const{ return fSEUTestSetting; }
			UInt_t GetSEUTestSetting( const char *pNode ) const { return fSEUTestSetting.find(pNode )->second; }

		protected:
			void initialiseSetting();

			SEUTestSetting                    fSEUTestSetting;
			ErrorAnalyser					  *fErrorAnalyser;		
	};
}
#endif

