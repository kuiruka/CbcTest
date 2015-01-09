WARNING
=======

Most stable version for the current 2CBC2/virtual machine setup found in branch "master". Includes firmware versions for both polarities.
calibGUI and ssGUI are tested.
SEUtest branch is merged, but seuGUI is not compatible with the beamtest firmware.

CbcTest
=======

Please check the folder doc/ for more info.

Caution : channel id in Analyser which dump the histograms and the number shown as Channel??? in the setting file FE?CBC?.txt are different.
			Former starts from 0, later starts from 1.

Tested in the beamtest virtual machine for two versions with single GLIB, single old FMC, one and two CBC2 chips.

Should work in any of the newer versions of the VM.

Known problem
-------
There are problems in SRAM interface. 
- 128th word for readBlock() is always 0.  This does not affect data with the data format in the beamtest setup. 
- Large data is not read from SRAM with readBlock() function in VM v1.1.0 (http://sbgcmstrackerupgrade.in2p3.fr/).  This seems to happen only for VM.
  One solution to this is to reduce the # of events to be read at once with
    GlibReg_user_wb_ttc_fmc_regs.pc_commands.CBC_DATA_PACKET_NUMBER 
  As small as 10 works.


