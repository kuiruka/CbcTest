CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC `root-config --cflags`
DevFlags		    = -D__CBCDAQ_DEV__
#DevFlags		    = 

IncludeDirs	=  /opt/cactus/include /opt/xdaq/include ../

IncludePaths		= $(IncludeDirs:%=-I%)

LibraryDirs = /opt/cactus/lib /opt/xdaq/lib ../lib
LibraryPaths = $(LibraryDirs:%=-L%)

ExternalObjects = $(LibraryPaths) -lpthread -lboost_thread -lboost_filesystem -lboost_regex -lboost_system -lboost_thread -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal -lcgicc `root-config --glibs` -lutils -lCbc -lStrasbourg -lAnalysers 

%.o: %.cc %.h 
	$(CXX) $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<


all: calib vcthscan calibGUI daq daqGUI ssGUI seuGUI dumpRawData radtestGUI

calib: calib.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq -lICCalib 
	mv $@ ../bin

vcthscan: vcthscan.cc 
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq -lICCalib 
	mv $@ ../bin

calibGUI: calibGUI.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq -lICCalib 
	mv $@ ../bin

daq: daq.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq 
	mv $@ ../bin

daqGUI: daqGUI.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq 
	mv $@ ../bin

ssGUI: ssGUI.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq -lSignalShape
	mv $@ ../bin

seuGUI: seuGUI.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq 
	mv $@ ../bin

radtestGUI: radtestGUI.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) -lCbcDaq 
	mv $@ ../bin

dumpRawData: dumpRawData.cc  
	$(CXX) $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) 
	mv $@ ../bin

clean:

