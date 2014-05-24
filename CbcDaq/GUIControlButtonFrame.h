#ifndef __GUICONTROLBUTTONFRAME_H__
#define __GUICONTROLBUTTONFRAME_H__

#include <TROOT.h>
#include <TGFrame.h>
#include <vector>
#include <TGButton.h> 

namespace CbcDaq{

	class GUIFrame;

	class ControlButtonFrame : public TGHorizontalFrame {

		public:
			enum State { InitialState, GlibConfigured, CbcConfigured, AnalyserConfigured, Running, Stopped, CbcHardReset };

		public:
			ControlButtonFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame );
			virtual ~ControlButtonFrame(){;}
			virtual void AddButtons();
			virtual void SetButtonState( State pState ); 
			virtual Bool_t ToggleDataStreamDisplayButton();
			ClassDef( ControlButtonFrame, 0 );
		protected:
			virtual void addProcessButtons();
			void setMainControlButtons( EButtonState pState, ULong_t pColor );
			void resetMainControlButtons();
			TGCompositeFrame *fMotherFrame;
			GUIFrame *fGUIFrame;
			TGTextButton *fGlibConfigurationButton;
			TGTextButton *fCbcConfigurationButton;
			TGTextButton *fAnalyserConfigurationButton;
			TGTextButton *fDaqStartButton;
			TGTextButton *fStopButton;
			TGTextButton *fCbcHardResetButton;
			TGTextButton *fDataStreamDisplayButton;
			TGTextButton *fExitButton;
			std::vector< TGTextButton *> fMainControlButtons;
			std::vector< TGTextButton *> fProcessButtons;
	};
}

#endif

