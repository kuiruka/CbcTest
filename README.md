WARNING
=======

Most stable version for the current 2CBC2/virtual machine setup found in branch "CbcTestForIPbus2.2". Includes firmware versions for both polarities.

CbcTest
=======

Please check the folder doc/ for more info.

Tested in the beamtest virtual machine for two versions with single GLIB, single old FMC, one and two CBC2 chips.

Should work in any of the newer versions of the VM.

Known problem
-------
IPBus cannot deal with a big data to read at once with readBlock() function in VM v1.1.0
One solution to this is to reduce the # of events to be read at once with
GlibReg_user_wb_ttc_fmc_regs.pc_commands.CBC_DATA_PACKET_NUMBER 

As small as 10 works.

