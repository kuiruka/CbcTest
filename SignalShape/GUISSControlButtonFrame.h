#ifndef __GUISSCONTROLBUTTONFRAME_H__
#define __GUISSCONTROLBUTTONFRAME_H__

#include <TROOT.h>
#include <TGFrame.h>
#include "CbcDaq/GUIControlButtonFrame.h"
#include "GUISSControllerFrame.h"
#include <iostream>

using namespace CbcDaq;

namespace SignalShape {

	class SSControlButtonFrame : public ControlButtonFrame {

		public:
			SSControlButtonFrame( TGCompositeFrame *pFrame, SSControllerFrame *pSSControllerFrame );
			virtual ~SSControlButtonFrame(){;}
			void StartScan(){ std::cout << "I am pressed" << std::endl; }

			ClassDef( SSControlButtonFrame, 0 );

		private:
			void addProcessButtons();
			TGTextButton *fStartScanButton;
	};
}
#endif

