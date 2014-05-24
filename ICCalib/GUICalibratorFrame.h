#ifndef __GUICALIBRATORFRAME_H__
#define __GUICALIBRATORFRAME_H__

#include <TGFrame.h>
#include "CbcDaq/GUIFrame.h"

class TThread;
using namespace CbcDaq;

namespace ICCalib{

	class Calibrator;

	class CalibrationConfigurationFrame;
	class ScurveHistogramFrame;
	class DataStreamFrame;
	class VplusVsVCth0GraphFrame;

	class CalibratorFrame : public GUIFrame {

		public:
			CalibratorFrame( const TGWindow *p, UInt_t w, UInt_t h, const char *pWindowName, Calibrator *pCalibrator );
			virtual ~CalibratorFrame(){ Cleanup();}
			void AddAnalyserFrames( TGTab *pTab );
			void AddControlButtonFrame();
			void ProcessDAQControllerMessage( const char *pMessage );
			void ConfigureAnalyser();
			void DoVCthScan();
			static void DoVCthScan( void *p );
			void DoCalibration();
			static void DoCalibration( void *p );
			Calibrator *GetCalibrator(){ return fCalibrator; }
			ClassDef( CalibratorFrame, 0 );

		private:
			TThread *fThreadCalib;
			TThread *fThreadVCthScan;
			Calibrator *fCalibrator;
			CalibrationConfigurationFrame   *fCalibConfigFrame;
			VplusVsVCth0GraphFrame          *fVplusVsVCth0GraphFrame;
			ScurveHistogramFrame            *fScurveHistogramFrame;
			DataStreamFrame                 *fDataStreamFrame;
	};
}
#endif
