#pragma once

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <string>

struct NetworkSettings
{
	std::wstring serverIp = L"127.0.0.1";
	uint16 port = 7777;
};

inline std::string TrimNetworkSetting(std::string value)
{
	const auto isNotSpace = [](unsigned char ch) { return !std::isspace(ch); };
	value.erase(value.begin(), std::find_if(value.begin(), value.end(), isNotSpace));
	value.erase(std::find_if(value.rbegin(), value.rend(), isNotSpace).base(), value.end());
	return value;
}

inline std::filesystem::path FindNetworkConfig()
{
	auto findFrom = [](std::filesystem::path directory)
	{
		for (int depth = 0; depth < 8 && !directory.empty(); ++depth)
		{
			const auto candidate = directory / L"NetworkConfig.txt";
			if (std::filesystem::exists(candidate))
				return candidate;
			const auto parent = directory.parent_path();
			if (parent == directory)
				break;
			directory = parent;
		}
		return std::filesystem::path{};
	};

	if (const auto fromWorkingDirectory = findFrom(std::filesystem::current_path()); !fromWorkingDirectory.empty())
		return fromWorkingDirectory;

	wchar_t modulePath[MAX_PATH] = {};
	if (GetModuleFileNameW(nullptr, modulePath, MAX_PATH) != 0)
		return findFrom(std::filesystem::path(modulePath).parent_path());
	return {};
}

inline NetworkSettings LoadNetworkSettings()
{
	NetworkSettings settings;
	const auto configPath = FindNetworkConfig();
	if (configPath.empty())
		return settings;

	std::ifstream file(configPath);
	std::string line;
	while (std::getline(file, line))
	{
		line = TrimNetworkSetting(line);
		if (line.empty() || line[0] == '#')
			continue;

		const size_t separator = line.find('=');
		if (separator == std::string::npos)
			continue;

		const std::string key = TrimNetworkSetting(line.substr(0, separator));
		const std::string value = TrimNetworkSetting(line.substr(separator + 1));
		if (key == "ServerIp" && !value.empty())
			settings.serverIp.assign(value.begin(), value.end());
		else if (key == "Port")
		{
			try
			{
				const unsigned long parsedPort = std::stoul(value);
				if (parsedPort > 0 && parsedPort <= 65535)
					settings.port = static_cast<uint16>(parsedPort);
			}
			catch (...)
			{
				// Keep the default port when the external value is invalid.
			}
		}
	}
	return settings;
}
