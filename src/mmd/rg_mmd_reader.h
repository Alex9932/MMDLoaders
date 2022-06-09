/*
 * rg_mmd_reader.h
 *
 *  Created on: May 7, 2022
 *      Author: alex9932
 */

#ifndef MMD_RG_MMD_READER_H_
#define MMD_RG_MMD_READER_H_

#include <engine/core/filesystem.h>

class MMD_FSReader: public Engine::Filesystem::FSReader {
	public:
		MMD_FSReader(String file);
		virtual ~MMD_FSReader();

		void ReadZString(char* ptr, Uint32 size);
		RG_FORCE_INLINE void ReadZString15(char* ptr) { ReadZString(ptr, 15); }
		RG_FORCE_INLINE void ReadZString20(char* ptr) { ReadZString(ptr, 20); }
		RG_FORCE_INLINE void ReadZString30(char* ptr) { ReadZString(ptr, 30); }
		RG_FORCE_INLINE void ReadZString128(char* ptr) { ReadZString(ptr, 128); }
};

#endif /* MMD_RG_MMD_READER_H_ */
