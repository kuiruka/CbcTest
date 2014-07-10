#ifndef __GUISSCONFIGFRAME_H__
#define __GUISSCONFIGFRAME_H__


#include <TGFrame.h>
#include <map>

class TGNumberEntry;

namespace SignalShape{

	class SSControllerFrame;

	typedef std::pair<std::string, TGNumberEntry *> SSItemEntry;
	typedef std::map<std::string, TGNumberEntry *> SSSettingEntries;

	class SSConfigurationFrame : public TGCompositeFrame {

		public:
			SSConfigurationFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame );
			~SSConfigurationFrame(){}
			const SSSettingEntries &GetSSSettingEntries()const{ return fSSSettingEntries; }
			void RenewFrame();
			ClassDef( SSConfigurationFrame, 0 );
		private:
			TGCompositeFrame                *fMotherFrame;
			SSControllerFrame               *fSSControllerFrame;
			TGCompositeFrame                *fFrame;
			SSSettingEntries                fSSSettingEntries;
	};
}
#endif

