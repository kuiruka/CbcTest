#ifndef __GUICALIBCONFIGFRAME_H__
#define __GUICALIBCONFIGFRAME_H__


#include <TGFrame.h>
#include <map>

class TGNumberEntry;

namespace ICCalib{

	class CalibratorFrame;

	typedef std::pair<std::string, TGNumberEntry *> CalibItemEntry;
	typedef std::map<std::string, TGNumberEntry *> CalibSettingEntries;

	class CalibrationConfigurationFrame : public TGCompositeFrame {

		public:
			CalibrationConfigurationFrame( TGCompositeFrame *pFrame, CalibratorFrame *pCalibratorFrame );
			~CalibrationConfigurationFrame(){}
			const CalibSettingEntries &GetCalibSettingEntries()const{ return fCalibSettingEntries; }
			void RenewFrame();
			ClassDef( CalibrationConfigurationFrame, 0 );
		private:
			TGCompositeFrame                *fMotherFrame;
			CalibratorFrame                 *fCalibratorFrame;
			TGCompositeFrame                *fFrame;
			CalibSettingEntries             fCalibSettingEntries;
	};
}
#endif

