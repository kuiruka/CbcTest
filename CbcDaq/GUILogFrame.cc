#include "GUILogFrame.h"

namespace CbcDaq{
	LogFrame::LogFrame( TGCompositeFrame *pFrame, GUIFrame *pGUIFrame ):
		TGTextView( pFrame ),
		fMotherFrame( pFrame ),
		fGUIFrame( pGUIFrame ){

			fMotherFrame->AddFrame( this, gLHVexpand );
		}
}

