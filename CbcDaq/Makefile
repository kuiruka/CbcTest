Objs			= DAQController.o
GUISourceFiles	= GUI.cc GUIConfigFrame.cc GUIDaqMainConfigFrame.cc GUILogFrame.cc GUICbcRegFrame.cc GUIControlButtonFrame.cc GUIAnalyserFrame.cc GUIFrame.cc  
GUIObj			= GUIFrameDict.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC `root-config --cflags`
#DevFlags		    = -D__CBCDAQ_DEV__
DevFlags		    = 

IncludeDirs	=  /opt/cactus/include /opt/xdaq/include ../

IncludePaths		= $(IncludeDirs:%=-I%)


all: $(Objs) $(Objs:.o=.h) $(Objs:.o=.cc) $(GUIObj:.o=.cxx) $(GUIObj) $(GUISourceFiles:.cc=.h) $(GUISourceFiles)
	$(CC) -shared -o libCbcDaq.so $(Objs) $(GUIObj)
	mv libCbcDaq.so ../lib/

GUIFrameDict.cxx: GUIFrameLinkDef.h $(Objs:.o=.h) $(Objs:.o=.cc) $(GUISourceFiles:.cc=.h) $(GUISourceFiles)
	rootcint -f $@ -c -I../ $(GUISourceFiles) $< 

GUIFrameDict.o: GUIFrameDict.cxx GUIFrameDict.h
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<
	
%.o: %.cc %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

%.o: %.cxx %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<


clean:
		rm -f *.o GUIFrameDict.cxx GUIFrameDict.h

