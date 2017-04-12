/* 
 * File:   server_consts.hpp
 * Author: markov
 */

#ifndef SERVER_CONSTS_HPP
#define	SERVER_CONSTS_HPP

#include <cstddef>
#include <string>

extern const char* FILENAME_RAW_ID;
extern const char* FILENAME_RAW_DATA;
extern const char* FILENAME_PID;

extern const int LOCK_CREATED_FILE;
extern const int LOCK_PRINT_PLOTS;
extern const int LOCK_RELEASE_MEMORY;

extern const size_t MAX_TCP_MESSAGES_FOR_SENDING;

extern const char* FILENAME_LOG;

extern std::string PATH_OUT_MERGE_DATA;
extern std::string PATH_OUT_TMP_MERGE_DATA;

#endif	/* SERVER_CONSTS_HPP */

