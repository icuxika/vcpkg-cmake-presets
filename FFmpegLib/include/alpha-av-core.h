#pragma once

#include "alpha-av-decode.h"
#include "alpha-av-demux.h"
#include <iostream>

#ifdef _WIN32
#define LIB_DEMO_DLL_EXPORT __declspec(dllexport)
#else
#define LIB_DEMO_DLL_EXPORT
#endif

namespace av {
class LIB_DEMO_DLL_EXPORT AlphaAVCore {
  public:
	static AlphaAVCore &GetInstance();
	int openFile(const char *url);
	int play();

	std::unique_ptr<AlphaAVDemux> AlphaAVDemuxContext;
	std::unique_ptr<AlphaAVDecode> AlphaAVDecodeContext;

  private:
	AlphaAVCore();
	~AlphaAVCore();
	AlphaAVCore(const AlphaAVCore &) = delete;
	AlphaAVCore &operator=(const AlphaAVCore &) = delete;
};
} // namespace av