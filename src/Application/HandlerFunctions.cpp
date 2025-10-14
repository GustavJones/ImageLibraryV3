#include "GLog/Log.hpp"
#include "ImageLibrary/Application/HandlerFunctions.hpp"
#include "ImageLibrary/Application/APIEntries.hpp"
#include "Wepp/Server/HTTPChecks.hpp"
#include <string>
#include <filesystem>
#include <fstream>

static const std::filesystem::path s_DATA_PATH = std::filesystem::absolute("api");

static const ImageLibrary::APIEntry s_VALID_API_URIS[] = {
  ImageLibrary::APIEntry(ImageLibrary::StaticAPIEntry("/api/folders", "application/json", s_DATA_PATH / "folders.json")),
  ImageLibrary::APIEntry(ImageLibrary::DynamicAPIEntry("/api/folders", "application/json", s_DATA_PATH / "folders")),
  ImageLibrary::APIEntry(ImageLibrary::DynamicAPIEntry("/api/images", "image/jpg", s_DATA_PATH / "images")),
};

static const size_t s_VALID_API_URIS_SIZE = sizeof(s_VALID_API_URIS) / sizeof(s_VALID_API_URIS[0]);

namespace ImageLibrary {
  static bool ReadFile(const std::filesystem::path &_filePath, std::vector<unsigned char> &_emptyBuffer) {
    std::fstream file;
    size_t fileSize;

    _emptyBuffer.clear();

    file.open(_filePath, std::ios::in | std::ios::ate | std::ios::binary);

    if (file.is_open())
    {
      fileSize = file.tellg();
      file.seekg(file.beg);

      _emptyBuffer.resize(fileSize);

      file.read(reinterpret_cast<char *>(_emptyBuffer.data()), fileSize);

      file.close();
      return true;
    }
    else
    {
      file.close();
      return false;
    }
  }

  static bool GetAPIEntryInfoFromURI(const ImageLibrary::APIEntry* _apiEntries, const size_t _apiEntriesCount, const std::string &_uri, std::filesystem::path &_dataFilePath, std::string &_mimeType) {
    std::string modifiedURI = _uri;

    if (modifiedURI.back() == '/')
    {
      modifiedURI.erase(modifiedURI.end() - 1);
    }
    
    bool found = false;

    _dataFilePath = "";
    _mimeType = "";

    for (size_t i = 0; i < _apiEntriesCount; i++)
    {
      const ImageLibrary::APIEntry& entry = _apiEntries[i];

      const ImageLibrary::StaticAPIEntry* staticEntry = dynamic_cast<const ImageLibrary::StaticAPIEntry*>(entry.Entry());
      const ImageLibrary::DynamicAPIEntry* dynamicEntry = dynamic_cast<const ImageLibrary::DynamicAPIEntry*>(entry.Entry());
      const ImageLibrary::ExecutableAPIEntry* executableEntry = dynamic_cast<const ImageLibrary::ExecutableAPIEntry*>(entry.Entry());

      if (staticEntry)
      {
        if (staticEntry->identifier == modifiedURI)
        {
          if (!std::filesystem::exists(staticEntry->filePath) || !std::filesystem::is_regular_file(staticEntry->filePath))
          {
            break;
          }

          _mimeType = staticEntry->mimeType;
          _dataFilePath = staticEntry->filePath;
          found = true;
          break;
        }
      }
      else if (dynamicEntry)
      {
        if (std::filesystem::path(modifiedURI).parent_path().string() == dynamicEntry->identifier)
        {
          if (std::filesystem::exists(dynamicEntry->folderPath / std::filesystem::path(modifiedURI).filename()))
          {
            _mimeType = dynamicEntry->mimeType;
            _dataFilePath = dynamicEntry->folderPath / std::filesystem::path(modifiedURI).filename();
            found = true;
            break;
          }
        }
      }
      else if (executableEntry)
      {
        if (executableEntry->identifier == modifiedURI)
        {
          if (!std::filesystem::exists(executableEntry->executablePath) || !std::filesystem::is_regular_file(executableEntry->executablePath))
          {
            break;
          }

          // TODO
          found = false;
        }
      }
    }

    return found;
  }

  static bool HandleGETRequest(const GParsing::HTTPRequest &_req, GParsing::HTTPResponse &_resp, bool &_closeConnection) {
    std::filesystem::path dataFile;
    std::string mimeType;

    if (!GetAPIEntryInfoFromURI(s_VALID_API_URIS, s_VALID_API_URIS_SIZE, _req.uri, dataFile, mimeType))
    {
      GLog::Log(GLog::LOG_WARNING, "[Handler]: Connection requested invalid URI.");

      _resp.version = "HTTP/1.1";
      _resp.response_code = 404;
      _resp.response_code_message = "Not Found";
      _resp.headers.push_back({ "Connection", {"close"} });
      _closeConnection = true;

      return false;
    }

    if (!ReadFile(dataFile, _resp.message))
    {
      _resp.version = "HTTP/1.1";
      _resp.response_code = 500;
      _resp.response_code_message = "Internal Server Error";
      _resp.headers.push_back({ "Connection", {"close"} });
      _resp.message.clear();
      _closeConnection = true;

      return false;
    }

    _closeConnection = true;
    _resp.response_code = 200;
    _resp.response_code_message = "OK";
    _resp.version = "HTTP/1.1";
    _resp.headers.push_back({ "Connection", {"close"} });
    _resp.headers.push_back({ "Content-Type", {mimeType} });
    _resp.headers.push_back({ "Access-Control-Allow-Origin", {"*"} });

    return true;
  }

  static bool HandleHEADRequest(const GParsing::HTTPRequest& _req, GParsing::HTTPResponse& _resp, bool& _closeConnection) {
    _closeConnection = true;
    _resp.response_code = 200;
    _resp.response_code_message = "OK";
    _resp.version = "HTTP/1.1";
    _resp.headers.push_back({ "Connection", {"close"} });
    _resp.headers.push_back({ "Access-Control-Allow-Origin", {"*"} });
    _resp.message.clear();
    return true;
  }

  void SetupHandling() {
    std::filesystem::path path = s_DATA_PATH;

    if (!std::filesystem::exists(std::filesystem::absolute(path)) || !std::filesystem::is_directory(std::filesystem::absolute(path))) {
      std::filesystem::create_directory(std::filesystem::absolute(path));
    }

    for (size_t i = 0; i < s_VALID_API_URIS_SIZE; i++)
    {
      auto& entry = s_VALID_API_URIS[i];
      const auto* staticEntry = dynamic_cast<const ImageLibrary::StaticAPIEntry*>(entry.Entry());
      const auto* dynamicEntry = dynamic_cast<const ImageLibrary::DynamicAPIEntry*>(entry.Entry());

      if (staticEntry)
      {
        path = staticEntry->filePath;

        if (!std::filesystem::exists(std::filesystem::absolute(path)) || !std::filesystem::is_regular_file(std::filesystem::absolute(path))) {
          std::fstream f;
          f.open(std::filesystem::absolute(path), std::ios::out);
          f.close();
        }
      }
      else if (dynamicEntry)
      {
        path = dynamicEntry->folderPath;

        if (!std::filesystem::exists(std::filesystem::absolute(path)) || !std::filesystem::is_directory(std::filesystem::absolute(path))) {
          std::filesystem::create_directory(std::filesystem::absolute(path));
        }
      }
    }
  }

  bool HandleREST(GParsing::HTTPRequest _req, GParsing::HTTPResponse& _resp, bool& _closeConnection) {
    try
    {
      std::vector<unsigned char> buffer;
      buffer = _req.CreateRequest();
      buffer.push_back('\0');

      GLog::Log(GLog::LOG_TRACE, (std::string)"[Handler]: " + (char*)buffer.data());
    }
    catch (const std::exception&)
    {
      GLog::Log(GLog::LOG_WARNING, (std::string)"[Handler]: Invalid request sent to handler");
      _req.method = GParsing::GPARSING_UNKNOWN;
    }

    if (!Wepp::HasHostHeader(_req)) {
      GLog::Log(GLog::LOG_WARNING, "[Handler]: Connection request did not provide a Host header.");

      _resp.version = "HTTP/1.1";
      _resp.response_code = 400;
      _resp.response_code_message = "Bad Request";
      _resp.headers.push_back({ "Connection", {"close"} });
      _closeConnection = true;
      return false;
    }

    GLog::Log(GLog::LOG_TRACE, "[Handler]: Handling api request for - " + _req.uri);
    switch (_req.method)
    {
    case GParsing::GPARSING_UNKNOWN:
    {
      _resp.version = "HTTP/1.1";
      _resp.response_code = 400;
      _resp.response_code_message = "Bad Request";
      _resp.headers.push_back({ "Connection", {"close"} });
      _closeConnection = true;

      return false;
      break;
    }
    case GParsing::GPARSING_GET:
    {
      return HandleGETRequest(_req, _resp, _closeConnection);
      break;
    }
    case GParsing::GPARSING_HEAD:
    {
      return HandleHEADRequest(_req, _resp, _closeConnection);
      break;
    }
    default:
    {
      _resp.version = "HTTP/1.1";
      _resp.response_code = 501;
      _resp.response_code_message = "Not Implemented";
      _resp.headers.push_back({ "Connection", {"close"} });
      _resp.message.clear();
      _closeConnection = true;

      return false;
      break;
    }
    }
  }

  bool HandleRESTPost(GParsing::HTTPRequest _req, GParsing::HTTPResponse& _resp) {
    return false;
  }
}