#pragma once

#include "pch.h"

#include "Consts.hpp"
#include "Utils/Singleton.hpp"
#include "Objects/OrbitConfig.hpp"

#include "Failure.hpp"

namespace UbiorbitapiR2Loader
{
	using namespace std;
	using std::filesystem::path;

	inline path GetSavesPath(const unsigned int productId)
	{
		path savesPath;

		savesPath /= Singleton<OrbitConfig>::Instance().Get().Orbit.Saves.Path;
		savesPath /= to_string(productId);

		return savesPath;
	}

	inline path GetSavePath(const unsigned productId, const unsigned int saveId)
	{
		path savePath;

		savePath /= GetSavesPath(productId);
		savePath /= fmt::format("{}{}", saveId, SAVE_FILE_EXTENSION);

		return savePath;
	}

	inline path GetSaveNamePath(const path &savesPath, const unsigned int saveId)
	{
		path namePath;

		namePath /= savesPath;
		namePath /= fmt::format("{}{}", saveId, SAVE_NAME_EXTENSION);

		return namePath;
	}

	inline path GetSaveNamePath(const unsigned productId, const unsigned int saveId)
	{
		const auto savesPath = GetSavesPath(productId);
		const auto namePath = GetSaveNamePath(savesPath, saveId);

		return namePath;
	}

	// ReSharper disable once CppNotAllPathsReturnValue
	inline string ReadSaveName(const path &file)
	{
		if (const auto fs = fstream(file, ios::in); fs)
		{
			stringstream buffer;
			buffer << fs.rdbuf();
			return buffer.str();
		}

		Fail(fmt::format("File read error: {}", file.string()), true);
	}

	inline void WriteSaveName(const path &file, const string &content)
	{
		if (auto fs = fstream(file, ios::out | ios::trunc); fs)
		{
			fs.write(content.data(), content.size());
			return;
		}

		Fail(fmt::format("File write error: {}", file.string()), true);
	}

	// ReSharper disable once CppNotAllPathsReturnValue
	inline unsigned int ReadSave(const path &file, const unsigned int offset, void *buff,
								 const unsigned int numberOfBytes)
	{
		if (auto fs = fstream(file, ios::in | ios::binary); fs)
		{
			fs.seekg(offset, ios::beg);
			fs.read(&static_cast<char *>(buff)[0], numberOfBytes);

			const auto bytesCount = static_cast<unsigned int>(fs.gcount());
			return bytesCount;
		}

		Fail(fmt::format("File read error: {}", file.string()), true);
	}

	// ReSharper disable once CppNotAllPathsReturnValue
	inline unsigned int WriteSave(const path &file, void *buff, const unsigned int numberOfBytes)
	{
		if (auto fs = fstream(file, ios::out | ios::binary | ios::trunc); fs)
		{
			fs.seekp(0, ios::beg);
			const auto currentPosition = fs.tellp();
			fs.write(&static_cast<char *>(buff)[0], numberOfBytes);

			const auto bytesCount = static_cast<unsigned int>(fs.tellp() - currentPosition);
			return bytesCount;
		}

		Fail(fmt::format("File write error: {}", file.string()), true);
	}
} // namespace UbiorbitapiR2Loader
