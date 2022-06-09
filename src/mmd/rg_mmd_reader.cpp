/*
 * rg_mmd_reader.cpp
 *
 *  Created on: May 7, 2022
 *      Author: alex9932
 */

#include "rg_mmd_reader.h"

MMD_FSReader::MMD_FSReader(String file): FSReader(file) { }
MMD_FSReader::~MMD_FSReader() {
//	~FSReader();
}

void MMD_FSReader::ReadZString(char* ptr, Uint32 size) {
	Read(ptr, size);
}
