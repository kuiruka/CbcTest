Objs			= SEUTestController.o 
GUISourceFiles	=  
GUIObj			= 
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC `root-config --cflags`
#DevFlags		    = -D__CBCDAQ_DEV__
DevFlags		    = 

IncludeDirs	=  /opt/cactus/include /opt/xdaq/include ../

IncludePaths		= $(IncludeDirs:%=-I%)


all: $(Objs) $(Objs:.o=.h) $(Objs:.o=.cc) $(GUIObj:.o=.cxx) $(GUIObj) $(GUISourceFiles:.cc=.h) $(GUISourceFiles)
	$(CC) -shared -o libSEUTest.so $(Objs) $(GUIObj)
	mv libSEUTest.so ../lib/

%.o: %.cc %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

%.o: %.cxx %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<


clean:
		rm -f *.o 

