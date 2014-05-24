
################################################################################
# xdaq_profile, created Tues Nov  12 15:39:24 CEST 2013
################################################################################


## TRACKER-DAQ
export ENV_TRACKER_DAQ=/opt/trackerDAQ
## only for rebuilding trackerDAQ (best done outside this script)
#source /opt/testing/trackerDAQ-3.2/trackerDAQ.env

## RCMS
export ANT_HOME=/opt/apache-ant-1.6.5
#export JRE_HOME=/usr/java/jdk1.6.0_22
export JAVA_HOME=/usr/java/jdk1.6.0_22
export CATALINA_HOME=/opt/apache-tomcat-6.0.29
export RCMS_HOME=/opt/TriDAS/RunControl
export CVSROOT=:kserver:isscvs.cern.ch:/local/reps/tridas

## XDAQ
export XDAQ_ROOT=/opt/xdaq
export XDAQ_DOCUMENT_ROOT=/opt/xdaq/htdocs

## CMSSW
export VO_CMS_SW_DIR=/opt/cmssw/Base
export ROOTSYS=/opt/cmssw/Base/slc5_amd64_gcc472/lcg/root/5.34.07-cms
## ROOT
source /usr/local/bin/thisroot.sh
#EUDAQ
export PATH=/usr/local/Trolltech/Qt-4.8.5/bin:$PATH
#export LD_LIBRARY_PATH=/usr/local/Trolltech/Qt-4.8.5/lib:$LD_LIBRARY_PATH
#export QTDIR=/usr/local/Trolltech/Qt-4.8.5/lib/qt4 scons prefix="/usr" oss=0 lash=1 debug=1

#PYTHON/PYCHIPS
export PYTHONPATH=/opt/PyChips/src:$PYTHONPATH
export PYTHONSTARTUP=$HOME/.pythonrc



################################################################################
# Library And Binary Paths
################################################################################
#export LD_LIBRARY_PATH=${XDAQ_ROOT}/lib:${CMSSWDIR}/lib:${ENV_TRACKER_DAQ}/lib:/opt/corvus:/usr/local/lib/root:/home/xtaldaq/eudaq/bin:${LD_LIBRARY_PATH}
export LD_LIBRARY_PATH=${XDAQ_ROOT}/lib:${CMSSWDIR}/lib:${ENV_TRACKER_DAQ}/lib:/opt/corvus:/home/xtaldaq/eudaq/bin:${LD_LIBRARY_PATH}

export PATH=/sbin/:/usr/sbin/:${ENV_TRACKER_DAQ}/bin:${ANT_HOME}/bin:${JAVA_HOME}/bin:${PATH}



################################################################################
# Aliases
################################################################################

alias  InitCmsEnv=". $VO_CMS_SW_DIR/cmsset_default.sh"
alias  StartTomcat="$CATALINA_HOME/bin/startup.sh"
alias  StopTomcat="$CATALINA_HOME/bin/shutdown.sh"   
alias  RunManager="java -Duser.home=/home/xtaldaq/RCMS -jar /home/xtaldaq/RCMS/manager.jar"

##############################################################################
# EOS
#############################################################################
export EOS_MGM_URL=root://eoscms.cern.ch
export LD_LIBRARY_PATH=/opt/cactus/lib:${LD_LIBRARY_PATH}


export PATH=./bin:$PATH
export LD_LIBRARY_PATH=./lib:${LD_LIBRARY_PATH}

