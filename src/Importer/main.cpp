#include "GParsing/GParsing.hpp"
#include "ImageLibrary/Importer/DateUtils.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>

static const char s_IMAGE_LIBRARY_JSON_VERSION[] = "V1.0";

static const std::filesystem::path s_IMAGES_PATH = std::filesystem::absolute("api/images");
static const std::filesystem::path s_FOLDERS_PATH = std::filesystem::absolute("api/folders");
static const std::filesystem::path s_FOLDERS_CONFIG_PATH = std::filesystem::absolute("api/folders.json");

static bool ValidatePaths(const std::filesystem::path &_imagesPath, const std::filesystem::path& _foldersPath, const std::filesystem::path& _foldersConfigPath) {
	if (!std::filesystem::exists(_imagesPath))
	{
		return false;
	}

	if (!std::filesystem::exists(_foldersPath))
	{
		return false;
	}

	if (!std::filesystem::exists(_foldersConfigPath))
	{
		return false;
	}

	return true;
}

static bool ReadFile(const std::filesystem::path& _filePath, std::vector<unsigned char>& _buffer) {
	std::fstream f;
	f.open(_filePath, std::fstream::binary | std::fstream::in | std::fstream::ate);

	if (f.is_open())
	{
		_buffer.resize(f.tellg());
		f.seekg(f.beg);

		f.read(reinterpret_cast<char*>(_buffer.data()), _buffer.size());
		f.close();
		return true;
	}
	else
	{
		f.close();
		return false;
	}	
}

static bool CreateDefaultFoldersConfig(const std::filesystem::path& _foldersConfigPath, const std::string &_version) {
	GParsing::JSONObject<unsigned char> json;
	GParsing::JSONString<unsigned char> key;
	GParsing::JSONValue<unsigned char> value;

	key.SetString("version");
	value.SetString("V1.0");

	json.AddMember(key, value);

	key.SetString("folders");
	value.SetArray(GParsing::JSONArray<unsigned char>());

	json.AddMember(key, value);

	if (!json.Serialize(_foldersConfigPath.string()))
	{
		return false;
	}

	return true;
}

static bool ClearFoldersInFoldersConfig(const std::filesystem::path& _foldersConfigPath, const std::string& _version) {
	std::fstream f;
	f.open(_foldersConfigPath, std::fstream::out);
	f.close();

	return CreateDefaultFoldersConfig(_foldersConfigPath, _version);
}

static bool AddFolderData(const std::string& _folderName, const std::filesystem::path &_foldersPath, const std::string &_imageName) {
	GParsing::JSONObject<unsigned char> json;
	GParsing::JSONValue<unsigned char> value;
	GParsing::JSONObject<unsigned char> object;

	if (!json.Parse((_foldersPath / _folderName).string()))
	{
		return false;
	}

	const std::string imageURI = "/api/images/" + _imageName;

	value.SetString(imageURI);
	object.AddMember((GParsing::JSONString<unsigned char>)"uri", value);

	auto& images = json[(GParsing::JSONString<unsigned char>)"images"].GetArray();

	bool duplicate = false;
	for (size_t i = 0; i < images.GetSize(); i++)
	{
		GParsing::JSONString<unsigned char> objKey;
		GParsing::JSONValue<unsigned char> objValue;
		auto &imagePath = images.GetValue(i).GetObject();

		for (size_t j = 0; j < imagePath.GetMembersCount(); j++)
		{
			imagePath.GetMember(objKey, objValue, j);

			if (objKey == (GParsing::JSONString<unsigned char>)"uri")
			{
				if (objValue.GetString() == value.GetString())
				{
					duplicate = true;
					break;
				}
			}
		}

		if (duplicate)
		{
			break;
		}
	}

	if (!duplicate)
	{
		images.PushValue(object);
	}

	return json.Serialize((_foldersPath / _folderName).string());
}

static bool AddFolderToFoldersConfig(const std::string& _folderName, const std::filesystem::path& _foldersPath, const std::filesystem::path& _foldersConfigPath) {
	GParsing::JSONObject<unsigned char> json;
	GParsing::JSONValue<unsigned char> value;
	GParsing::JSONObject<unsigned char> object;

	value.SetString("/api/folders/" + _folderName);

	object.AddMember((GParsing::JSONString<unsigned char>)"uri", value);	

	value.SetObject(object);

	if (!json.Parse(_foldersConfigPath.string()))
	{
		return false;
	}

	auto& folders = json[(GParsing::JSONString<unsigned char>)"folders"].GetArray();
	bool duplicate = false;
	for (size_t i = 0; i < folders.GetSize(); i++)
	{
		GParsing::JSONString<unsigned char> tempKey;
		GParsing::JSONValue<unsigned char> tempValue;
		GParsing::JSONString<unsigned char> compareKey;
		GParsing::JSONValue<unsigned char> compareValue;

		auto& folderObject = folders.GetValue(i).GetObject();

		for (size_t j = 0; j < folderObject.GetMembersCount(); j++)
		{
			folderObject.GetMember(tempKey, tempValue, j);

			if (tempKey == (GParsing::JSONString<unsigned char>)"uri")
			{
				break;
			}
		}

		for (size_t k = 0; k < object.GetMembersCount(); k++)
		{
			object.GetMember(compareKey, compareValue, k);

			if (tempValue.GetString() == compareValue.GetString())
			{
				duplicate = true;
				break;
			}
		}

	}

	if (!duplicate)
	{
		folders.PushValue(value);
	}

	return json.Serialize(_foldersConfigPath.string());
}

static bool CreateDefaultFolderData(const std::string& _folderName, const std::filesystem::path& _foldersPath, const std::filesystem::path& _foldersConfigPath, const std::string& _version) {
	GParsing::JSONObject<unsigned char> json;
	GParsing::JSONString<unsigned char> key;
	GParsing::JSONValue<unsigned char> value;

	key.SetString("version");
	value.SetString(_version);

	json.AddMember(key, value);

	key.SetString("images");
	value.SetArray(GParsing::JSONArray<unsigned char>());

	json.AddMember(key, value);

	if (!json.Serialize((_foldersPath / _folderName).string()))
	{
		return false;
	}

	return true;
}

int main(const int argc, const char* argv[])
{
	std::vector<unsigned char> buffer;
	GParsing::EXIFImageJPEG<unsigned char> image;

	if (!std::filesystem::exists(s_FOLDERS_CONFIG_PATH))
	{
    std::cout << "Creating " << s_FOLDERS_CONFIG_PATH << std::endl;
		CreateDefaultFoldersConfig(s_FOLDERS_CONFIG_PATH, s_IMAGE_LIBRARY_JSON_VERSION);
	}
	else
	{
    std::cout << "Clearing previous " << s_FOLDERS_CONFIG_PATH << std::endl;
		ClearFoldersInFoldersConfig(s_FOLDERS_CONFIG_PATH, s_IMAGE_LIBRARY_JSON_VERSION);
	}

	if (!ValidatePaths(s_IMAGES_PATH, s_FOLDERS_PATH, s_FOLDERS_CONFIG_PATH))
	{
    std::cerr << "Please check that " << s_IMAGES_PATH << ", " << s_FOLDERS_PATH << " and " << s_FOLDERS_CONFIG_PATH << " exists" << std::endl;
		return 1;
	}

	std::filesystem::directory_iterator directoryIterator(s_IMAGES_PATH);
	for (const auto& entry : directoryIterator)
	{
		if (entry.is_directory())
		{
      std::cerr << entry << " is a directory, continuing" << std::endl;
			continue;
		}

		if (!ReadFile(entry, buffer))
		{
      std::cerr << "Cannot read file " << entry << ", continuing" << std::endl;
			continue;
		}

		if (!image.Parse(buffer))
		{
      std::cerr << "Cannot parse file " << entry << " as JPEG, continuing" << std::endl;
			continue;
		}

		auto dateTimeTags = image.GetTags(GParsing::EXIFTagNumber::DateTime_MainImage);

		if (dateTimeTags.size() <= 0)
		{
			continue;
		}

		auto &tag = dateTimeTags[0];

		std::vector<char> folderNameBuffer(11);
		auto timePoint = ImageLibrary::ParseTime(tag.CastTo<unsigned char>());
		std::strftime(folderNameBuffer.data(), folderNameBuffer.size(), "%F", &timePoint);
		folderNameBuffer.pop_back();

		const std::string folderName = std::string(folderNameBuffer.begin(), folderNameBuffer.end()) + ".json";

		if (!std::filesystem::exists(s_FOLDERS_PATH / folderName))
		{
      std::cout << "Creating " << s_FOLDERS_PATH / folderName << std::endl;
			CreateDefaultFolderData(folderName, s_FOLDERS_PATH, s_FOLDERS_CONFIG_PATH, s_IMAGE_LIBRARY_JSON_VERSION);
		}

    std::cout << "Adding " << entry << " to " << s_FOLDERS_PATH / folderName << std::endl;
		AddFolderData(folderName, s_FOLDERS_PATH, entry.path().filename().string());
	}

	directoryIterator = std::filesystem::directory_iterator(s_FOLDERS_PATH);
	for (const auto& entry : directoryIterator)
	{
		std::string fileName = std::filesystem::path(entry).filename().string();

		AddFolderToFoldersConfig(fileName, s_FOLDERS_PATH, s_FOLDERS_CONFIG_PATH);
	}

	return 0;
}
