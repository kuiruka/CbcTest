Objs			= Analysers.o AnalysedData.o GUIData.o ScurveAnalyser.o SignalShapeAnalyser.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC `root-config --cflags`
#DevFlags		    = -D__CBCDAQ_DEV__
DevFlags		    = 

IncludeDirs	=  /opt/cactus/include /opt/xdaq/include ../

IncludePaths		= $(IncludeDirs:%=-I%)


all: $(Objs) $(Objs:.o=.h) $(Objs:.o=.cc) 
	$(CC) -shared -o libAnalysers.so $(Objs) 
	mv libAnalysers.so ../lib/

%.o: %.cc %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

clean:
		rm -f *.o 

