#pragma once

#include "pch.h"

#include "Macro.hpp"
#include "Consts.hpp"

#include "Helpers/Saves.hpp"
#include "Utils/Singleton.hpp"
#include "Objects/OrbitConfig.hpp"

#include "SavegameInfo.hpp"
#include "SavegameReader.hpp"
#include "SavegameWriter.hpp"

#include "Interfaces/IRemoveSavegameListener.hpp"
#include "Interfaces/IGetLoginDetailsListener.hpp"
#include "Interfaces/IGetSavegameListListener.hpp"
#include "Interfaces/IGetSavegameWriterListener.hpp"
#include "Interfaces/IGetSavegameReaderListener.hpp"

// ReSharper disable CppInconsistentNaming
// ReSharper disable CppParameterMayBeConst
// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppMemberFunctionMayBeStatic
namespace mg::orbitclient
{
	using namespace std;
	using namespace UbiorbitapiR2Loader;

	namespace fs = std::filesystem;

	class UPLAY_CPP_API OrbitClient // NOLINT
	{
		typedef shared_ptr<SavegameReader> SharedSavegameReader;
		typedef shared_ptr<SavegameWriter> SharedSavegameWriter;
		typedef shared_ptr<vector<SavegameInfo *>> SharedSavegameList;

		atomic_int RequestId = {0};
		std::map<int, IGetLoginDetailsListener*> m_loginCallbacks;

		void InitSavegameList();
		void ClearSavegameList();

	public:
		OrbitClient();
		void StartProcess(unsigned short *, unsigned short *, unsigned short *);
		bool StartLauncher(unsigned int, unsigned int, char const *, char const *);
		void GetSavegameList(unsigned int requestId, IGetSavegameListListener *savegameListListenerCallBack,
							 unsigned int productId);
		void GetSavegameWriter(unsigned int requestId, IGetSavegameWriterListener *savegameWriterListenerCallBack,
							   unsigned int productId, unsigned int saveGameId, bool open);
		void GetSavegameReader(unsigned int requestId, IGetSavegameReaderListener *savegameReaderListenerCallBack,
							   unsigned int productId, unsigned int saveGameId);
		void RemoveSavegame(unsigned int requestId, IRemoveSavegameListener *removeSavegameListenerCallBack,
							unsigned int productId, unsigned int saveGameId);
		void GetLoginDetails(unsigned int requestId, IGetLoginDetailsListener *loginDetailsListenerCallBack);
		unsigned int GetRequestUniqueId();
		unsigned short *GetInstallationErrorString(char const *);
		unsigned int GetInstallationErrorNum();
		void Update();
		~OrbitClient();
	};
} // namespace mg::orbitclient

//------------------------------------------------------------------------------
inline mg::orbitclient::OrbitClient::OrbitClient()
{
	LOGD << "__CALL__";
	InitSavegameList();
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::InitSavegameList()
{
	Singleton<SharedSavegameList>::Instance().Set(make_shared<vector<SavegameInfo *>>());
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::ClearSavegameList()
{
	const auto saveGames = Singleton<SharedSavegameList>::Instance().Get();

	if (saveGames->size() > 0)
	{
		for (const auto *saveGame : *saveGames)
		{
			delete saveGame;
		}

		saveGames->clear();
	}
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::StartProcess(unsigned short *, unsigned short *, unsigned short *)
{
	LOGD << "__CALL__";
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::GetSavegameList(unsigned int requestId,
														  IGetSavegameListListener *savegameListListenerCallBack,
														  unsigned int productId)
{
	LOGD << fmt::format("RequestId: {} GetSavegameListListenerCallBack: {} ProductId: {}", requestId,
						reinterpret_cast<void *>(&savegameListListenerCallBack), productId);

	const auto callBack = reinterpret_cast<IGetSavegameListListener::CallBackPtrType>(**savegameListListenerCallBack->CallBackPtr);

	if (callBack == nullptr)
	{
		return;
	}

	const auto savesPath = GetSavesPath(productId);

	// ReSharper disable CppRedundantQualifier

	if (!fs::exists(savesPath) || fs::is_empty(savesPath))
	{
		callBack(savegameListListenerCallBack, requestId, nullptr, 0);
		return;
	}

	ClearSavegameList();

	const auto saveGames = Singleton<SharedSavegameList>::Instance().Get();

	for (const auto &dirEntry : fs::directory_iterator(savesPath))
	{
		const auto &path = dirEntry.path();

		if (path.has_extension() && path.extension() == SAVE_FILE_EXTENSION)
		{
			const auto id = ST::string(path.stem().string()).to_uint();
			const auto size = static_cast<unsigned long>(fs::file_size(path));

			const auto namePath = GetSaveNamePath(productId, id);

			if (fs::exists(namePath))
			{
				const auto name = ST::string(ReadSaveName(namePath)).trim().to_std_wstring();
				const auto saveGameInfo = new SavegameInfo(id, size, name);

				saveGames->push_back(saveGameInfo);
			}
		}
	}

	if (saveGames->size() > 0)
	{
		callBack(savegameListListenerCallBack, requestId, *saveGames->data(), saveGames->size());
	}
	else
	{
		callBack(savegameListListenerCallBack, requestId, nullptr, 0);
	}

	// ReSharper restore CppRedundantQualifier
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::GetSavegameReader(unsigned int requestId,
															IGetSavegameReaderListener *savegameReaderListenerCallBack,
															unsigned int productId, unsigned int saveGameId)
{
	LOGD << fmt::format("RequestId: {} GetSavegameReaderListenerCallBack: {} ProductId: {} SaveGameId: {}", requestId,
						reinterpret_cast<void *>(&savegameReaderListenerCallBack),
						productId, saveGameId);

	const auto callBack = reinterpret_cast<IGetSavegameReaderListener::CallBackPtrType>(**savegameReaderListenerCallBack
																							  ->CallBackPtr);

	if (callBack == nullptr)
	{
		return;
	}

	const auto savePath = GetSavePath(productId, saveGameId);

	Singleton<SharedSavegameReader>::Instance().Set(
		make_shared<SavegameReader>(savePath));

	const auto saveGameReader = Singleton<SharedSavegameReader>::Instance().Get();

	callBack(savegameReaderListenerCallBack, requestId, NULL, saveGameReader.get());
}

inline int StringToWString(std::wstring& ws, const std::string& s)
{
	std::wstring wsTmp(s.begin(), s.end());

	ws = wsTmp;

	return 0;
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::Update()
{
	LOGD << "__CALL__ Update";

	for (auto cb : m_loginCallbacks)
	{
		const auto callBack = reinterpret_cast<IGetLoginDetailsListener::CallBackPtrType>(**cb.second->CallBackPtr);

		if (callBack == nullptr)
		{
			return;
		}

		const auto& profile = Singleton<OrbitConfig>::Instance()
			.Get()
			.Orbit
			.Profile;

		const auto accountId = profile.AccountId.c_str();
		const auto password = profile.Password.c_str();
		const auto gamekey = profile.GameKey.c_str();

		std::wstring passwordWstr;
		StringToWString(passwordWstr, password);

		callBack(cb.second, cb.first, accountId, passwordWstr.c_str(), gamekey);
	}
	m_loginCallbacks.clear();



}

//------------------------------------------------------------------------------
inline bool mg::orbitclient::OrbitClient::StartLauncher(unsigned int a, unsigned int b, char const *langCode, char const *arguments)
{
	LOGD << "__CALL__ StartLauncher";
	return false;
}

//------------------------------------------------------------------------------
inline unsigned short *mg::orbitclient::OrbitClient::GetInstallationErrorString(char const *err)
{
	LOGD << "__CALL__";
	return nullptr;
}

//------------------------------------------------------------------------------
inline unsigned int mg::orbitclient::OrbitClient::GetInstallationErrorNum()
{
	LOGD << "__CALL__ GetInstallationErrorNum";
	return 0;
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::GetSavegameWriter(unsigned int requestId,
															IGetSavegameWriterListener *savegameWriterListenerCallBack,
															unsigned int productId, unsigned int saveGameId, bool open)
{
	LOGD << fmt::format("RequestId: {} GetSavegameWriterListenerCallBack: {} ProductId: {} SaveGameId: {}", requestId,
						reinterpret_cast<void *>(&savegameWriterListenerCallBack), productId, saveGameId);

	const auto callBack = reinterpret_cast<IGetSavegameWriterListener::CallBackPtrType>(**savegameWriterListenerCallBack
																							  ->CallBackPtr);

	if (callBack == nullptr)
	{
		return;
	}

	const auto savePath = GetSavePath(productId, saveGameId);
	const auto namePath = GetSaveNamePath(productId, saveGameId);

	Singleton<SharedSavegameWriter>::Instance().Set(
		make_shared<SavegameWriter>(savePath, namePath, saveGameId));

	const auto saveGameWriter = Singleton<SharedSavegameWriter>::Instance().Get();

	callBack(savegameWriterListenerCallBack, requestId, NULL, saveGameWriter.get());
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::RemoveSavegame(unsigned int requestId,
														 IRemoveSavegameListener *removeSavegameListenerCallBack,
														 unsigned int productId, unsigned int saveGameId)
{
	LOGD << fmt::format("RequestId: {} RemoveSavegameListenerCallBack: {}", requestId,
						reinterpret_cast<void *>(&removeSavegameListenerCallBack));

	const auto callBack = reinterpret_cast<IRemoveSavegameListener::CallBackPtrType>(**removeSavegameListenerCallBack->CallBackPtr);

	if (callBack == nullptr)
	{
		return;
	}

	// ReSharper disable CppRedundantQualifier

	const auto savePath = GetSavePath(productId, saveGameId);
	const auto saveNamePath = GetSaveNamePath(productId, saveGameId);
	const auto isDeleted = fs::remove(savePath) && fs::remove(saveNamePath);

	callBack(removeSavegameListenerCallBack, requestId, isDeleted);

	// ReSharper restore CppRedundantQualifier
}

//------------------------------------------------------------------------------
inline void mg::orbitclient::OrbitClient::GetLoginDetails(unsigned int requestId,
														  IGetLoginDetailsListener *loginDetailsListenerCallBack)
{
	LOGD << fmt::format("RequestId: {} LoginDetailsListenerCallBack: {}", requestId,
						reinterpret_cast<void *>(&loginDetailsListenerCallBack));

	m_loginCallbacks[requestId] = loginDetailsListenerCallBack;
	
}

//------------------------------------------------------------------------------
inline unsigned int mg::orbitclient::OrbitClient::GetRequestUniqueId()
{
	LOGD << "__CALL__ GetRequestUniqueId";
	return ++RequestId;
}

//------------------------------------------------------------------------------
inline mg::orbitclient::OrbitClient::~OrbitClient()
{
	LOGD << "__CALL__";
	ClearSavegameList();
}

// ReSharper restore CppMemberFunctionMayBeStatic
// ReSharper restore CppMemberFunctionMayBeConst
// ReSharper restore CppInconsistentNaming
