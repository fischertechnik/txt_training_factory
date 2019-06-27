#!/bin/bash

#set -x #echo on

#Soucefiles containing a FSM:
FILE_LIST=("TxtHighBayWarehouseRun.cpp" "TxtMultiProcessingStationRun.cpp"  "TxtVacuumGripperRobotRun.cpp" "TxtSortingLineRun.cpp") #...

FILE_LIST_INC=("TxtHighBayWarehouse.h" "TxtMultiProcessingStation.h" "TxtVacuumGripperRobot.h" "TxtSortingLine.h") #...

SRC_DIR="../src/"
INC_DIR="../include/"

for idx in "${!FILE_LIST[@]}"; do
  i=${FILE_LIST[$idx]}
  j=${FILE_LIST_INC[$idx]}
  #echo "Processing $i and $j"
  docfsm ${SRC_DIR}$i ${INC_DIR}$j >"../doc/DocFsm/${i%cpp}gv"
  #docfsm ${SRC_DIR}$i >"../doc/DocFsm/${i%cpp}gv"
done
