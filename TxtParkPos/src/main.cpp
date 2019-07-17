#include "TxtAxis.h"
#ifdef CLIENT_MPO
	#include "TxtMultiProcessingStation.h"
#elif CLIENT_HBW
	#include "TxtHighBayWarehouse.h"
#elif CLIENT_VGR
	#include "TxtVacuumGripperRobot.h"
#elif MAIN_SCC
	#include "TxtPanTiltUnit.h"
#else
	#error Set CLIENT_XXX define first!
#endif

#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include <stdio.h>          // for printf()
#include <string.h>         // for memset()
#include <unistd.h>         // for sleep()
#include <cmath>			// for pow()
#include <fstream>

unsigned int DebugFlags;
FILE *DebugFile;

// Version info
#define VERSION_HEX ((0<<16)|(1<<8)|(0<<0))
char TxtAppVer[32];

FISH_X1_TRANSFER* pTArea = NULL;

int main(int argc, char* argv[])
{
    if (StartTxtDownloadProg() == KELIB_ERROR_NONE)
    {
        pTArea = GetKeLibTransferAreaMainAddress();

        if (pTArea)
        {
				ft::TxtTransfer T(pTArea);
				ft::TxtTransfer* pT = &T;
#ifdef CLIENT_MPO
		    	ft::TxtMultiProcessingStation mpo(pT);
				printf("setCompressor on\n");
		    	mpo.setCompressor(true);
				printf("setValveOvenDoor open\n");
		    	mpo.setValveOvenDoor(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(300));

				printf("axisRotTable\n");
				std::thread tR = mpo.axisRotTable.moveS1Thread();
				printf("axisRotTable\n");
				std::thread tO = mpo.axisOvenInOut.moveS2Thread();
				printf("axisRotTable\n");
				std::thread tG = mpo.axisGripper.moveS1Thread();
				tG.join();
				tO.join();
				tR.join();

				printf("setCompressor off\n");
		    	mpo.setCompressor(false);

				printf("axisRotTable 500\n");
		    	mpo.axisRotTable.setMotorRight();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				mpo.axisRotTable.setMotorOff();

				printf("axisGripper 6000\n");
				mpo.axisGripper.setMotorRight();
				std::this_thread::sleep_for(std::chrono::milliseconds(6000));
				mpo.axisGripper.setMotorOff();
#elif CLIENT_HBW
		    	ft::TxtHighBayWarehouse hbw(pT);
		    	hbw.moveRef();
		    	hbw.axisX.moveAbs(100);
		    	hbw.axisY.moveAbs(900);
#elif CLIENT_VGR
		    	ft::TxtVacuumGripperRobot vgr(pT);
		    	vgr.moveYRef();
		    	vgr.moveZRef();
		    	vgr.moveXRef();
		    	vgr.axisX.moveAbs(472);
		    	vgr.axisY.moveAbs(865);
		    	vgr.axisZ.moveAbs(10);
#elif MAIN_SCC
		    	ft::TxtPanTiltUnit ptu(pT);
		    	ptu.init();
		    	ptu.moveHome();
#else
				#error Set CLIENT_XXX define first!
#endif

        }
        StopTxtDownloadProg();
    }
	return 0;
}
