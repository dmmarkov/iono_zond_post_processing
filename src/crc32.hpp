/* 
 * File:   crc32.hpp
 * Author: dmitri
 */

#ifndef CRC32_HPP
#define	CRC32_HPP

#include "project_defines.hpp"

#include <vector>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned int NovatelCRC32Value(int i) ;
unsigned int NovatelEvaluateCRC32Value(std::size_t ulCount, unsigned char *ucBuffer) ;

#endif	/* CRC32_HPP */

