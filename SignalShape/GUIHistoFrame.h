#ifndef __GUIHISTOGRAMFRAME_H__
#define __GUIHISTOGRAMFRAME_H__

#include <TGFrame.h>

class TGTab;
namespace SignalShape{

	class SignalShapeGraphFrame : public TGCompositeFrame {

		public:
			SignalShapeGraphFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame );
			~SignalShapeGraphFrame(){}
			void RenewFrame();
			ClassDef( SignalShapeGraphFrame, 0 );
		private:
			TGCompositeFrame   *fMotherFrame;
			SSControllerFrame  *fSSControllerFrame;
			TGTab              *fFrame;
	};

	class ScurveHistogramFrame : public TGCompositeFrame {

		public:
			ScurveHistogramFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame );
			virtual ~ScurveHistogramFrame(){}
			void RenewFrame();
			ClassDef( ScurveHistogramFrame, 0 );
		private:
			TGCompositeFrame *fMotherFrame;
			SSControllerFrame  *fSSControllerFrame;
			TGTab            *fFrame;
	};

	class DataStreamFrame : public TGCompositeFrame {

		public:
			DataStreamFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			virtual ~DataStreamFrame(){}
			void RenewFrame();
			ClassDef( DataStreamFrame, 0 );
		private:
			TGCompositeFrame *fMotherFrame;
			GUIFrame         *fGUIFrame;
			TGTab            *fFrame;
	};
}
#endif

