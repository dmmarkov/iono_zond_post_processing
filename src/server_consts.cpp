/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "server_consts.hpp"

const char* FILENAME_RAW_ID = "ID.RAW";
const char* FILENAME_RAW_DATA = "RAW_DATA_";
const char* FILENAME_PID = "/var/run/ionozondserver.pid";

const int LOCK_CREATED_FILE = 1000;
const int LOCK_PRINT_PLOTS = 100;
const int LOCK_RELEASE_MEMORY = 500;

const size_t MAX_TCP_MESSAGES_FOR_SENDING = 1000000;

const char* FILENAME_LOG = "ionozondserver.log";

std::string PATH_OUT_MERGE_DATA = "";
std::string PATH_OUT_TMP_MERGE_DATA = "";