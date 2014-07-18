#ifndef __GUIFRAME_H__
#define __GUIFRAME_H__

#include <TGFrame.h>
#include <TString.h>
#include <map>
#include <iostream>
#include <vector>
#include "GUIDaqMainConfigFrame.h"

class TGTextEntry;
class TGTextView;
class TGNumberEntry;
class TGTab;
class TRootEmbeddedCanvas;
class TGLabel;
class TCanvas;
class TThread;
class TH1F;


namespace Strasbourg{

	class Data;
	class Event;
}
namespace Cbc{

	class CbcRegItem;
	class CbcRegList;
}
namespace CbcDaq{

	class DAQController;
}

using namespace Strasbourg;
using namespace Cbc;

namespace CbcDaq{

	class ConfigurationFileFrame;
	class DaqMainConfigurationFrame;
	class LogFrame;
	class CbcRegFrames;
	class AnalyserFrame;
	class DataStreamFrame;
	class ControlButtonFrame;

	class GUIFrame : public TGMainFrame {

		public:
			GUIFrame( const TGWindow *p, UInt_t w, UInt_t h, const char *pWindowName, DAQController *pDAQController );
//			virtual ~GUIFrame(){ Cleanup();}
			virtual ~GUIFrame(){}
			virtual void AddFrames();
			virtual void AddAnalyserFrames( TGTab *pTab );
			virtual void AddControlButtonFrame();
			virtual void ProcessDAQControllerMessage( const char * );
			virtual void LoadConfiguration( const char *pName );
			virtual void ConfigureGlib();
			virtual void ConfigureCbc();
			virtual void ConfigureAnalyser();
			void Run();
			static void Run( void *p );
			virtual void Stop();
			virtual void CbcHardReset();
			virtual void ShowDataStream();
			virtual void UpdateCbcRegFrame( std::vector<const CbcRegItem *> pList );
			virtual void SetCbcRegFileName( UInt_t pFeId, UInt_t pCbcId, const char *pName );
			virtual TGTextEdit *GetInputLogFrame(){ return fDaqMainConfigFrame->GetInputLogFrame(); } 
			DAQController *GetDAQController(){ return fDAQController; }
			ClassDef( GUIFrame, 0 );
		protected:
			TString                         fWindowName;
			ConfigurationFileFrame          *fConfigurationFileFrame;
			DaqMainConfigurationFrame       *fDaqMainConfigFrame;
			LogFrame                        *fLogFrame;
			CbcRegFrames                    *fCbcRegFrames;
			AnalyserFrame                   *fAnalyserFrame;
			ControlButtonFrame              *fControlButtonFrame;
			DAQController		            *fDAQController;
			TThread *fDaq;
	};
}
#endif
