Objs			= Calibrator.o ScurveAnalyser.o AnalysedData.o GUIData.o
GUISourceFiles	= GUICalibConfigFrame.cc GUICalibratorControlButtonFrame.cc GUICalibHistoFrame.cc GUICalibratorFrame.cc 
GUIObj			= GUICalibratorFrameDict.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC `root-config --cflags`
#DevFlags		    = -D__CBCDAQ_DEV__
DevFlags		    = 

IncludeDirs	=  /opt/cactus/include /opt/xdaq/include ../

IncludePaths		= $(IncludeDirs:%=-I%)


all: $(Objs) $(Objs:.o=.h) $(Objs:.o=.cc) $(GUIObj:.o=.cxx) $(GUIObj) $(GUISourceFiles:.cc=.h) $(GUISourceFiles)
	$(CC) -shared -o libICCalib.so $(Objs) $(GUIObj)
	mv libICCalib.so ../lib/

GUICalibratorFrameDict.cxx: GUICalibratorFrameLinkDef.h $(Objs:.o=.h) $(Objs:.o=.cc) $(GUISourceFiles:.cc=.h) $(GUISourceFiles)
	rootcint -f $@ -c -I../ $(GUISourceFiles) $< 

GUICalibratorFrameDict.o: GUICalibratorFrameDict.cxx GUICalibratorFrameDict.h
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<
	
%.o: %.cc %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

%.o: %.cxx %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<


clean:
		rm -f *.o GUICalibratorFrameDict.cxx GUICalibratorFrameDict.h

