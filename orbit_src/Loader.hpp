#pragma once

#include "pch.h"

#include "Consts.hpp"

// ReSharper disable CppUnusedIncludeDirective
#include "Exports/OrbitClient.hpp"
#include "Exports/SavegameInfo.hpp"
#include "Exports/SavegameReader.hpp"
#include "Exports/SavegameWriter.hpp"
// ReSharper restore CppUnusedIncludeDirective

#include "Helpers/Json.hpp"
#include "Helpers/Failure.hpp"
#include "Utils/Singleton.hpp"
#include "Objects/OrbitConfig.hpp"

namespace UbiorbitapiR2Loader
{
	using std::filesystem::directory_iterator;
	using std::filesystem::path;

	// ReSharper disable once CppInconsistentNaming
	namespace fs = std::filesystem;

	inline void InitConfig()
	{
		const auto currentPath = fs::current_path();
		const auto configPath = currentPath / path(CONFIG_NAME);

		// ReSharper disable CppRedundantQualifier

		if (!fs::exists(configPath))
		{
			Fail(fmt::format("{} file not found!", CONFIG_NAME), false);
		}

		if (fs::exists(configPath) && fs::is_empty(configPath))
		{
			Fail(fmt::format("{} file is empty!", CONFIG_NAME), false);
		}

		// ReSharper restore CppRedundantQualifier

		OrbitConfig config;
		NameValuePair<Orbit &> orbit("Orbit", config.Orbit);

		DeserializeFromJsonFile(configPath, orbit);

		Singleton<OrbitConfig>::Instance().Set(config);
	}

	inline void InitLog()
	{
		if (Singleton<OrbitConfig>::Instance().Get().Orbit.Logging.Active)
		{
			const auto logFile = Singleton<OrbitConfig>::Instance().Get().Orbit.Logging.Path;

			// ReSharper disable CppRedundantQualifier

			if (fs::exists(logFile))
			{
				if (!fs::remove(logFile))
				{
					Fail(fmt::format("Unable to remove {}!", logFile), false);
				}
			}

			// ReSharper restore CppRedundantQualifier

			init(plog::debug, logFile.c_str());
		}
	}
} // namespace UbiorbitapiR2Loader
