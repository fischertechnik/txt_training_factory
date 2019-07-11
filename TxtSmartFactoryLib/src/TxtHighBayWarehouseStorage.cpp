/*
 * TxtHighBayWarehouseStorage.cpp
 *
 *  Created on: 18.02.2019
 *      Author: steiger-a
 */

#include "TxtHighBayWarehouseStorage.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#include <fstream>


namespace ft {


TxtHighBayWarehouseStorage::TxtHighBayWarehouseStorage()
	: filename("Data/Config.HBW.Storage.json")
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "TxtHighBayWarehouseStorage",0);
	if (!loadStorageState())
	{
		resetStorageState();
	}
	currentPos.x = -1;
	currentPos.y = -1;
	//nextStorePos.x = -1;
	//nextStorePos.y = -1;
	nextFetchPos.x = -1;
	nextFetchPos.y = -1;
	Notify();
	print();
}

TxtHighBayWarehouseStorage::~TxtHighBayWarehouseStorage()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "~TxtHighBayWarehouseStorage",0);
}

bool TxtHighBayWarehouseStorage::loadStorageState()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "loadStorageState",0);
    std::ifstream infile(filename.c_str());
    if ( infile.good())
    {
    	//load file
        Json::Value root;
        Json::CharReaderBuilder builder;
        std::string errs;
        std::ifstream test(filename.c_str(), std::ifstream::binary);
        if (test.is_open()) {
            std::cout << "load file " << filename << std::endl;
            bool ok = Json::parseFromStream(builder, test, &root, &errs);
            if ( !ok )
            {
                std::cout  << "error: " << errs << std::endl;
                return false;
            }
        }
        const Json::Value val_Storage = root["Storage"];
        const char * locations[3][3] = {{"A1","A2","A3"},{"B1","B2","B3"},{"C1","C2","C3"}};
    	for(int i=0;i<3;i++)
    	{
    		for(int j=0;j<3;j++)
    		{
    			wpc[i][j] = false;
    		}
    	}
        for(int i=0;i<3;i++)
    	{
    		for(int j=0;j<3;j++)
    		{
    			std::string loc = locations[i][j];
    	        const Json::Value val_wp = val_Storage[loc];
    	        if (val_wp.isNull()) {
    	            wp[i][j] = 0;
    	            wpc[i][j] = true;
    	    		std::cout << loc << ": null" << std::endl;
    	        } else {
    	            wp[i][j] = new TxtWorkpiece(
    	            		(std::string)val_wp["tag_uid"].asString(),
    	            		(TxtWPType_t)(val_wp["type"].asInt()),
    	            		(TxtWPState_t)(val_wp["state"].asInt()));
    	    		std::cout << loc << " tag_uid:" << wp[i][j]->tag_uid
    	    				<< " :" << (int)wp[i][j]->state
							<< " :" << (int)wp[i][j]->type <<std::endl;
    	        }
    		}
    	}
    	Notify();
    	return true;
    }
	return false;
}

bool TxtHighBayWarehouseStorage::saveStorageState()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "saveStorageState",0);
	Json::Value event;
    const char * locations[3][3] = {{"A1","A2","A3"},{"B1","B2","B3"},{"C1","C2","C3"}};
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			std::string loc = locations[i][j];
	        if (wp[i][j] == NULL) {
	        	event["Storage"][loc] = Json::nullValue;
	        } else {
	        	event["Storage"][loc]["tag_uid"] = wp[i][j]->tag_uid;
	        	event["Storage"][loc]["state"] = (int)wp[i][j]->state;
	        	event["Storage"][loc]["type"] = (int)wp[i][j]->type;
	        }
		}
	}

    Json::StreamWriterBuilder builder;
    builder["commentStyle"] = "None";
    builder["indentation"] = " ";

    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream(filename.c_str(), std::ios_base::out);
    if(!outputFileStream.is_open())
	{
    	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "filename {} is not opened!",filename.c_str());
    	return false;
	}
    return (writer->write(event, &outputFileStream) == 0);
}

void TxtHighBayWarehouseStorage::resetStorageState()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "resetStorageState",0);
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<3;j++)
		{
			wp[i][j] = 0;
			wpc[i][j] = true;
		}
	}
	Notify();
	saveStorageState();
}

bool TxtHighBayWarehouseStorage::storeContainer()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "storeContainer",0);
	/*nextStorePos.x = -1; //set invalid pos
	nextStorePos.y = -1;
	bool found = false;
	for(int i=0;i<3 && !found;i++)
	{
		for(int j=2;j>=0&& !found;j--)
		{
			StoragePos2 p;
			p.x = i; p.y = j;
			if (wp[i][j] == 0)
			{
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "0 -> nextStorePos {} {}",p.x, p.y);
				nextStorePos = p;
				found = true;
			}
		}
	}*/
	if (isValidPos(nextFetchPos)) //nextFetchPos
	{
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK -> nextStorePos",0);
		wp[nextFetchPos.x][nextFetchPos.y] = 0;
		Notify();
		print();
		return true;
	}
	return false;
}

bool TxtHighBayWarehouseStorage::store(TxtWorkpiece _wp)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "store wp:{} {} {}",_wp.tag_uid,_wp.type,_wp.state);
	/*nextStorePos.x = -1; //set invalid pos
	nextStorePos.y = -1;
	if (_wp.type == WP_TYPE_NONE)
	{
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "NONE -> return false",0);
		return false;
	} else
	{
		bool found = false;
		for(int i=0;i<3 && !found;i++)
		{
			for(int j=2;j>=0&& !found;j--)
			{
				StoragePos2 p;
				p.x = i; p.y = j;
				if (wp[i][j] == 0)
				{
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "0 -> nextStorePos {} {}",p.x, p.y);
					nextStorePos = p;
					found = true;
				}
			}
		}
	}*/
	if (isValidPos(nextFetchPos))
	{
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK -> nextStorePos type {} ",_wp.type);
		wp[nextFetchPos.x][nextFetchPos.y] = new TxtWorkpiece(_wp);
		Notify();
		print();
		return true;
	}
	return false;
}

bool TxtHighBayWarehouseStorage::fetch(TxtWPType_t t)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "fetch {}",t);
	nextFetchPos.x = -1; //set invalid pos
	nextFetchPos.y = -1;
	if (t == WP_TYPE_NONE)
	{
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "STORAGE_EMPTY -> return false",0);
		return false;
	} else
	{
		bool found = false;
		for(int i=0;i<3 && !found;i++)
		{
			for(int j=2;j>=0&& !found;j--)
			{
				StoragePos2 p;
				p.x = i; p.y = j;
				if (wp[i][j] == NULL)
					continue;
				if (wp[i][j]->type == t)
				{
					SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "t {} -> nextFetchPos {} {}",t, p.x, p.y);
					nextFetchPos = p;
					found = true;
				}
			}
		}
	}
	if (isValidPos(nextFetchPos))
	{
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK -> nextFetchPos type {} ",t);
		delete wp[nextFetchPos.x][nextFetchPos.y];
		wp[nextFetchPos.x][nextFetchPos.y] = 0;
		Notify();
		print();
		return true;
	}
	return false;
}

bool TxtHighBayWarehouseStorage::fetchContainer()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "fetchContainer",0);
	nextFetchPos.x = -1; //set invalid pos
	nextFetchPos.y = -1;
	bool found = false;
	for(int i=0;i<3 && !found;i++)
	{
		for(int j=2;j>=0&& !found;j--)
		{
			StoragePos2 p;
			p.x = i; p.y = j;
			if (wp[i][j] == 0)
			{
				SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "cont -> nextFetchPos {} {}",p.x, p.y);
				nextFetchPos = p;
				found = true;
			}
		}
	}
	if (isValidPos(nextFetchPos))
	{
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK -> nextFetchPos cont ",0);
		wp[nextFetchPos.x][nextFetchPos.y] = 0;
		Notify();
		print();
		return true;
	}
	return false;
}

bool TxtHighBayWarehouseStorage::isValidPos(StoragePos2 p)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "isValidPos {} {}",p.x,p.y);
	bool ret = false;
	if ((p.x >= 0) && (p.x <= 2) && (p.y >= 0) && (p.y <= 2))
	{
		ret = true;
		SPDLOG_LOGGER_DEBUG(spdlog::get("console"), "OK",0);
	}
	return ret;
}

bool TxtHighBayWarehouseStorage::canColorBeStored(TxtWPType_t c)
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "canColorBeStored {}", c);
	bool ret = false;
	int numc = 0;
	for(int i=0;i<3;i++)
	{
		for(int j=2;j>=0;j--)
		{
			if (wp[i][j] != 0){
				if (wp[i][j]->type == c) numc++;
			}
		}
	}
	if (numc < 3)
	{
		ret = true;
	}
	return ret;
}

Stock_map_t TxtHighBayWarehouseStorage::getStockMap()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "getStockMap",0);
	Stock_map_t map_wps;
	map_wps["A1"] =  wp[0][0];
	map_wps["A2"] =  wp[1][0];
	map_wps["A3"] =  wp[2][0];
	map_wps["B1"] =  wp[0][1];
	map_wps["B2"] =  wp[1][1];
	map_wps["B3"] =  wp[2][1];
	map_wps["C1"] =  wp[0][2];
	map_wps["C2"] =  wp[1][2];
	map_wps["C3"] =  wp[2][2];
	return map_wps;
}

char TxtHighBayWarehouseStorage::charType(int x, int y)
{
	char c = '?';
	if (wp[x][y])
	{
		switch(wp[x][y]->type)
		{
		case WP_TYPE_NONE:
			c = '!';
			break;
		case WP_TYPE_WHITE:
			c = 'W';
			break;
		case WP_TYPE_RED:
			c = 'R';
			break;
		case WP_TYPE_BLUE:
			c = 'B';
			break;
		default:
			break;
		}
	} else {
		c = '_';
	}
	return c;
}

void TxtHighBayWarehouseStorage::print()
{
	SPDLOG_LOGGER_TRACE(spdlog::get("console"), "print",0);
	std::cout << charType(0,0) << charType(1,0) << charType(2,0) << std::endl;
	std::cout << charType(0,1) << charType(1,1) << charType(2,1) << std::endl;
	std::cout << charType(0,2) << charType(1,2) << charType(2,2) << std::endl;
}


} /* namespace ft */
