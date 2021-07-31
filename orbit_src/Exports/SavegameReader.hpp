#pragma once

#include "pch.h"

#include "Macro.hpp"
#include "Helpers/Saves.hpp"

#include "Interfaces/ISavegameReadListener.hpp"

// ReSharper disable CppInconsistentNaming
// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppMemberFunctionMayBeConst
namespace mg::orbitclient
{
	using namespace std;
	using namespace UbiorbitapiR2Loader;
	using std::filesystem::path;

	class UPLAY_CPP_API SavegameReader
	{
		path FilePath{};

	public:
		explicit SavegameReader(const path& filePath);
		void Close();
		void Read(unsigned int requestId, ISavegameReadListener* savegameReadListenerCallBack,
		          unsigned int offset, void* buff, unsigned int numberOfBytes);
	};
} // namespace mg::orbitclient

//------------------------------------------------------------------------------
inline mg::orbitclient::SavegameReader::SavegameReader(const path& filePath)
{
	FilePath = filePath;
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::SavegameReader::Close()
{
	LOGD << "__CALL__";
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::SavegameReader::Read(unsigned int requestId,
                                                  ISavegameReadListener* savegameReadListenerCallBack,
                                                  unsigned int offset, void* buff, unsigned int numberOfBytes)
{
	LOGD << fmt::format("RequestId: {} SavegameReadListenerCallBack: {} Offset: {} Buff: {} NumberOfBytes: {}",
	                    requestId,
	                    reinterpret_cast<void *>(&savegameReadListenerCallBack), offset, buff, numberOfBytes);

	const auto callBack = reinterpret_cast<ISavegameReadListener::CallBackPtrType>(**savegameReadListenerCallBack->
		CallBackPtr);

	if (callBack == nullptr)
	{
		return;
	}

	const auto bytesCount = ReadSave(FilePath, offset, buff, numberOfBytes);

	callBack(savegameReadListenerCallBack, requestId, bytesCount);
}

// ReSharper restore CppInconsistentNaming
// ReSharper restore CppParameterMayBeConst
// ReSharper restore CppMemberFunctionMayBeConst
