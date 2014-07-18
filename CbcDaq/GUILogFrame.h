#ifndef __GUILOGFRAME_H__
#define __GUILOGFRAME_H__

#include <TGTextView.h>

class TGCompositeFrame;

namespace CbcDaq{

class GUIFrame;

class LogFrame : public TGTextView{

	public:
		LogFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
		virtual ~LogFrame(){;}
		ClassDef( LogFrame, 0 );

	private:
		TGCompositeFrame *fMotherFrame;
		GUIFrame *fGUIFrame;
		std::string                     fLogFileName;
};
}

#endif

