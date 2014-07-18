Objs			= SSController.o 
GUISourceFiles	= GUISSControlButtonFrame.cc GUIHistoFrame.cc GUISSConfigFrame.cc GUISSControllerFrame.cc 
GUIObj			= GUISSControllerFrameDict.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC `root-config --cflags`
#DevFlags		    = -D__CBCDAQ_DEV__
DevFlags		    = 

IncludeDirs	=  /opt/cactus/include /opt/xdaq/include ../

IncludePaths		= $(IncludeDirs:%=-I%)


all: $(Objs) $(Objs:.o=.h) $(Objs:.o=.cc) $(GUIObj:.o=.cxx) $(GUIObj) $(GUISourceFiles:.cc=.h) $(GUISourceFiles)
	$(CC) -shared -o libSignalShape.so $(Objs) $(GUIObj)
	mv libSignalShape.so ../lib/

GUISSControllerFrameDict.cxx: GUISSControllerFrameLinkDef.h $(Objs:.o=.h) $(Objs:.o=.cc) $(GUISourceFiles:.cc=.h) $(GUISourceFiles)
	rootcint -f $@ -c -I../ $(GUISourceFiles) $< 

GUISSControllerFrameDict.o: GUISSControllerFrameDict.cxx GUISSControllerFrameDict.h
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<
	
%.o: %.cc %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

%.o: %.cxx %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<


clean:
		rm -f *.o GUISSControllerFrameDict.cxx GUISSControllerFrameDict.h

