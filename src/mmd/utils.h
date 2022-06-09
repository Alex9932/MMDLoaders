/*
 * utils.h
 *
 *  Created on: Apr 2, 2022
 *      Author: alex9932
 */

#ifndef MMD_UTILS_H_
#define MMD_UTILS_H_

#include <engine/engine.h>

namespace MMD_Utils {

	// Encoding utils
	void SJISToUTF8(String sjis);
	void UTF16ToUTF8(WString utf16, Uint32 len);

	String GetBuffer();

}



#endif /* MMD_UTILS_H_ */
