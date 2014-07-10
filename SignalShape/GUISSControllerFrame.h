#ifndef __GUISSCONTROLLERFRAME_H__
#define __GUISSCONTROLLERFRAME_H__

#include <TGFrame.h>
#include "CbcDaq/GUIFrame.h"

class TThread;
using namespace CbcDaq;

namespace SignalShape{

		class SSController;
		class SSConfigurationFrame;
		class ScurveHistogramFrame;
		class SignalShapeGraphFrame;
		class DataStreamFrame;

		class SSControllerFrame : public GUIFrame {

			public:
				SSControllerFrame( const TGWindow *p, UInt_t w, UInt_t h, const char *pWindowName, SSController *pSSController );
				virtual ~SSControllerFrame(){ Cleanup();}
				void AddAnalyserFrames( TGTab *pTab );
				void AddControlButtonFrame();
				void ProcessDAQControllerMessage( const char *pMessage );
				void ConfigureAnalyser();
				void StartScan();
				static void StartScan( void *p );
				SSController *GetSSController(){ return fSSController; }

				ClassDef( SSControllerFrame, 0 );

			private:
				TThread                         *fThreadScan;
				SSController                    *fSSController;
				SSConfigurationFrame            *fSSConfigurationFrame;
				ScurveHistogramFrame            *fScurveHistogramFrame;
				SignalShapeGraphFrame           *fSignalShapeGraphFrame;
				DataStreamFrame                 *fDataStreamFrame;
		};
}

#endif

