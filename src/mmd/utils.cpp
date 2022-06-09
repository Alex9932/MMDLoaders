/*
 * utils.cpp
 *
 *  Created on: Apr 2, 2022
 *      Author: alex9932
 */

#include "utils.h"

#include <string.h>
#include <sstream>
#include <locale>
#include <codecvt>
#include <string>

#include <engine/engine.h>

#include <iconv.h>

#define MAX_BUF 1024

namespace MMD_Utils {

	static char BUFFER[MAX_BUF];

	void SJISToUTF8(String sjis) {
		char inbuf[MAX_BUF+1] = { 0 };
		char outbuf[MAX_BUF+1] = { 0 };
		char* in = inbuf;
		char* out = outbuf;
		size_t in_size = (size_t)MAX_BUF;
		size_t out_size = (size_t)MAX_BUF;
		iconv_t ic = iconv_open("UTF-8", "SJIS");
		memcpy(in, sjis, strlen(sjis));
		iconv(ic, &in, &in_size, &out, &out_size);

		memset(BUFFER, 0, MAX_BUF);
		memcpy(BUFFER, outbuf, strlen(outbuf));
		iconv_close(ic);

		//rgLogInfo(RG_LOG_SYSTEM, "ICONV: %s", BUFFER);

	}

	void UTF16ToUTF8(WString utf16, Uint32 len) {
		char inbuf[MAX_BUF+1] = { 0 };
		char outbuf[MAX_BUF+1] = { 0 };
		char* in = inbuf;
		char* out = outbuf;
		size_t in_size = (size_t)MAX_BUF;
		size_t out_size = (size_t)MAX_BUF;
		iconv_t ic = iconv_open("UTF-8", "UTF-16LE");
		memcpy(in, utf16, len);
		iconv(ic, &in, &in_size, &out, &out_size);

		memset(BUFFER, 0, MAX_BUF);
		memcpy(BUFFER, outbuf, strlen(outbuf));
		iconv_close(ic);
	}

	String GetBuffer() {
		return BUFFER;
	}

}


