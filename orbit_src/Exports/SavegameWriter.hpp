#pragma once

#include "pch.h"

#include "Macro.hpp"
#include "Helpers/Saves.hpp"
#include "Utils/Singleton.hpp"
#include "Objects/OrbitConfig.hpp"

#include "Interfaces/ISavegameWriteListener.hpp"

// ReSharper disable CppInconsistentNaming
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppMemberFunctionMayBeStatic
// ReSharper disable CppParameterMayBeConst
namespace mg::orbitclient
{
	using namespace std;
	using namespace UbiorbitapiR2Loader;
	using std::filesystem::path;

	namespace fs = std::filesystem;

	class UPLAY_CPP_API SavegameWriter
	{
		path FilePath{};
		path NamePath{};
		unsigned int SaveId;

	public:
		SavegameWriter(const path &filePath, const path &namePath, unsigned int saveId);
		void Close(bool arg);
		void Write(unsigned int requestId, ISavegameWriteListener *savegameWriteListenerCallBack, void *buff,
				   unsigned int numberOfBytes);
		bool SetName(unsigned short *name);
		void BackupSave(const string &backupPath);
	};
} // namespace mg::orbitclient

//------------------------------------------------------------------------------
inline mg::orbitclient::SavegameWriter::SavegameWriter(const path &filePath, const path &namePath,
													   unsigned int saveId)
{
	FilePath = filePath;
	NamePath = namePath;
	SaveId = saveId;
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::SavegameWriter::BackupSave(const string &backupPath)
{
	// ReSharper disable CppRedundantQualifier

	if (!fs::exists(backupPath))
	{
		fs::create_directories(backupPath);
	}

	const auto backupFile = date::format("%m-%d-%Y %I-%M-%S UTC", chrono::system_clock::now());
	const auto backupFilePath = path(backupPath) / path(
													   fmt::format("{}_{}.{}", SaveId, backupFile, SAVE_BACKUP_EXTENSION));
	const auto backupFileNamePath = path(backupPath) / path(
														   fmt::format("{}_{}.{}", SaveId, backupFile, SAVE_NAME_EXTENSION));

	fs::copy(FilePath, backupFilePath);
	fs::copy(NamePath, backupFileNamePath);

	// ReSharper restore CppRedundantQualifier
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::SavegameWriter::Close(bool arg)
{
	LOGD << "__CALL__";
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::SavegameWriter::Write(unsigned int requestId,
												   ISavegameWriteListener *savegameWriteListenerCallBack, void *buff,
												   unsigned int numberOfBytes)
{
	LOGD << fmt::format("RequestId: {} SavegameWriteListenerCallBack: {} Buff: {} NumberOfBytes: {}", requestId,
						reinterpret_cast<void *>(&savegameWriteListenerCallBack), buff, numberOfBytes);

	const auto callBack = reinterpret_cast<ISavegameWriteListener::CallBackPtrType>(**savegameWriteListenerCallBack->CallBackPtr);

	if (callBack == nullptr)
	{
		return;
	}

	const auto &backup = Singleton<OrbitConfig>::Instance().Get().Orbit.Saves.Backup;
	const auto savesPath = FilePath.parent_path();

	// ReSharper disable CppRedundantQualifier

	fs::create_directories(savesPath);

	if (backup.Active && fs::exists(FilePath) && fs::exists(NamePath))
	{
		BackupSave(backup.Path);
	}

	// ReSharper restore CppRedundantQualifier

	const auto bytesCount = WriteSave(FilePath, buff, numberOfBytes);

	callBack(savegameWriteListenerCallBack, requestId, bytesCount);
}

//------------------------------------------------------------------------------
inline bool mg::orbitclient::SavegameWriter::SetName(unsigned short *name)
{
	//LOGD << fmt::format("Name: {}", reinterpret_cast<wchar_t *>(name));

	const auto nameString = ST::string(wstring(reinterpret_cast<wchar_t *>(name)));

	WriteSaveName(NamePath, nameString.to_std_string());

	return true;
}

// ReSharper restore CppInconsistentNaming
// ReSharper restore CppMemberFunctionMayBeConst
// ReSharper restore CppMemberFunctionMayBeStatic
// ReSharper restore CppParameterMayBeConst
