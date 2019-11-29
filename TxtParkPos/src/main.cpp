#include "TxtAxis.h"
#ifdef CLIENT_MPO
	#include "TxtMultiProcessingStation.h"
#elif CLIENT_HBW
	#include "TxtHighBayWarehouse.h"
#elif CLIENT_VGR
	#include "TxtVacuumGripperRobot.h"
#elif MAIN_SSC
	#include "TxtPanTiltUnit.h"
#else
	#error Set CLIENT_XXX define first!
#endif
#include "TxtSound.h"

#include "KeLibTxtDl.h"     // TXT Lib
#include "FtShmem.h"        // TXT Transfer Area

#include <stdio.h>          // for printf()
#include <string.h>         // for memset()
#include <unistd.h>         // for sleep()
#include <cmath>			// for pow()
#include <fstream>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

unsigned int DebugFlags;
FILE *DebugFile;

// Version info
#define VERSION_HEX ((0<<16)|(8<<8)|(1<<0))
char TxtAppVer[32];

FISH_X1_TRANSFER* pTArea = NULL;

int main(int argc, char* argv[])
{
	sprintf(TxtAppVer,"%d.%d.%d",
			(VERSION_HEX>>16)&0xff,(VERSION_HEX>>8)&0xff,(VERSION_HEX>>0)&0xff);

	// Set the default logger to file logger
	auto file_logger = spdlog::basic_logger_mt("basic_logger", "Data/ParkPos.log", true);
	spdlog::set_default_logger(file_logger);

    if (StartTxtDownloadProg() == KELIB_ERROR_NONE)
    {
        pTArea = GetKeLibTransferAreaMainAddress();

        if (pTArea)
        {
				ft::TxtTransfer T(pTArea);
#ifdef CLIENT_MPO
				spdlog::info("MPO {}", TxtAppVer);
		    	ft::TxtMultiProcessingStation mpo(&T);
				//printf("setCompressor on\n");
		    	mpo.setCompressor(true);
				//printf("setValveOvenDoor open\n");
		    	mpo.setValveOvenDoor(true);
				std::this_thread::sleep_for(std::chrono::milliseconds(300));

				//printf("axisRotTable\n");
				std::thread tR = mpo.axisRotTable.moveS1Thread();
				//printf("axisRotTable\n");
				std::thread tO = mpo.axisOvenInOut.moveS2Thread();
				//printf("axisRotTable\n");
				std::thread tG = mpo.axisGripper.moveS1Thread();
				tG.join();
				tO.join();
				tR.join();

				//printf("setCompressor off\n");
		    	mpo.setCompressor(false);

				//printf("axisRotTable 500\n");
		    	mpo.axisRotTable.setMotorRight();
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
				mpo.axisRotTable.setMotorOff();

				//printf("axisGripper 6000\n");
				mpo.axisGripper.setMotorRight();
				std::this_thread::sleep_for(std::chrono::milliseconds(6000));
				mpo.axisGripper.setMotorOff();
#elif CLIENT_HBW
				spdlog::info("HBW {}", TxtAppVer);
				//load config
			    Json::Value root;
			    Json::CharReaderBuilder builder;
			    std::string errs;
			    const std::string filenameConfig = "Data/Config.ParkPos.json";
			    std::ifstream test(filenameConfig, std::ifstream::binary);
			    if (test.is_open()) {
			        std::cout << "load file " << filenameConfig << std::endl;
			        bool ok = Json::parseFromStream(builder, test, &root, &errs);
			        if ( !ok )
			        {
			            std::cout  << errs << "\n";
			        }
			    }
			    int posx = root.get("x", 170 ).asInt();
			    int posy = root.get("y", 900 ).asInt();

		    	ft::TxtHighBayWarehouse hbw(&T);
		    	hbw.moveRef();
		    	hbw.axisX.moveAbs(posx);
		    	hbw.axisY.moveAbs(posy);
#elif CLIENT_VGR
		    	spdlog::info("VGR {}", TxtAppVer);
				//load config
			    Json::Value root;
			    Json::CharReaderBuilder builder;
			    std::string errs;
			    const std::string filenameConfig = "Data/Config.ParkPos.json";
			    std::ifstream test(filenameConfig, std::ifstream::binary);
			    if (test.is_open()) {
			        std::cout << "load file " << filenameConfig << std::endl;
			        bool ok = Json::parseFromStream(builder, test, &root, &errs);
			        if ( !ok )
			        {
			            std::cout  << errs << "\n";
			        }
			    }
			    int posx = root.get("x", 467 ).asInt();
			    int posy = root.get("y", 865 ).asInt();
			    int posz = root.get("z", 10 ).asInt();

		    	ft::TxtVacuumGripperRobot vgr(&T);
		    	vgr.moveYRef();
		    	vgr.moveZRef();
		    	vgr.moveXRef();
		    	vgr.axisX.moveAbs(posx);
		    	vgr.axisY.moveAbs(posy);
		    	vgr.axisZ.moveAbs(posz);
#elif MAIN_SSC
		    	spdlog::info("SSC {}", TxtAppVer);
		    	ft::TxtPanTiltUnit ptu(&T);
		    	ptu.init();
		    	ptu.moveHome();
		    	ptu.movePanCenter();
		    	ptu.moveTiltPos(100);
#else
				#error Set CLIENT_XXX define first!
#endif
				ft::TxtSound::play(pTArea,1);
        }
        StopTxtDownloadProg();
    }
	return 0;
}
