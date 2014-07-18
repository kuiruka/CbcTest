#ifndef __GUIANALYSERFRAME_H__
#define __GUIANALYSERFRAME_H__


#include "TGFrame.h"

namespace CbcDaq{

	class GUIFrame;
	class AnalyserTextFrame;
	class AnalyserHistFrame;

	class AnalyserFrame : public TGCompositeFrame {

		public:
		static const double fTextFrameHeight;
		public:
			AnalyserFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			virtual ~AnalyserFrame(){}
			void RenewFrame();
			ClassDef( AnalyserFrame, 0 );
		private:
			TGCompositeFrame  *fMotherFrame;
			GUIFrame          *fGUIFrame;
			AnalyserTextFrame *fTextFrame;
			AnalyserHistFrame *fHistFrame;
	};

	class AnalyserTextFrame : public TGTextView{

		public:
			AnalyserTextFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			virtual ~AnalyserTextFrame(){;}
			void RenewFrame();
			ClassDef( AnalyserTextFrame, 0 );

		private:
			TGCompositeFrame *fMotherFrame;
			GUIFrame *fGUIFrame;
	};

	class AnalyserHistFrame : public TGCompositeFrame {

		public:
			AnalyserHistFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			virtual ~AnalyserHistFrame(){}
			void RenewFrame();
			ClassDef( AnalyserHistFrame, 0 );
		private:
			TGCompositeFrame *fMotherFrame;
			GUIFrame         *fGUIFrame;
			TGTab            *fFrame;
	};
}
#endif

