#ifndef __GUICONFIGFRAME_H__
#define __GUICONFIGFRAME_H__


#include <TGFrame.h>

class TGLabel;
class TGTextEntry;
class TGTextButton;

namespace CbcDaq{

	class GUIFrame;

	class ConfigurationFileFrame : public TGHorizontalFrame {

		public:
			ConfigurationFileFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame, const char *pFname );
			~ConfigurationFileFrame(){}
			void Load();
			ClassDef( ConfigurationFileFrame, 0 );
		private:
			GUIFrame *fGUIFrame;
			TGLabel *fLabel;
			TGTextEntry *fTextEntry;
			TGTextButton *fButton;
	};
}
#endif

