#include "Wepp/Server/Server.hpp"
#include "Wepp/Server/HandlerFunctions.hpp"
#include "ImageLibrary/Application/HandlerFunctions.hpp"
#include <string>
#include <cstdint>
#include <future>

static const std::string PREFIX = "[Image Library Server]";
static const std::string ADDRESS = "0.0.0.0";
static const uint16_t PORT = 8080;

int main(int argc, char* argv[]) {
#ifdef NDEBUG
  GLog::SetLogLevel(GLog::LOG_WARNING);
#else
  GLog::SetLogLevel(GLog::LOG_TRACE);
#endif // NDEBUG

  GLog::SetLogPrefix(PREFIX);

  Wepp::SetupHandling();
  ImageLibrary::SetupHandling();
  Wepp::Server httpServer(Wepp::HandleWeb, Wepp::HandleWebPost, true, 12);
  Wepp::Server apiServer(ImageLibrary::HandleREST, ImageLibrary::HandleRESTPost, true, 16);

  std::atomic<bool> close = false;
  GLog::Log(GLog::LOG_PRINT, "Starting Wepp server on " + ADDRESS + ':' + std::to_string(PORT));

  auto httpThread = std::async(std::launch::async,
    [&httpServer, &close]() {
      try {
        httpServer.Run(ADDRESS, PORT, close);
      }
      catch (const std::exception& e) {
        GLog::Log(GLog::LOG_ERROR, e.what());
        close = true;
      }
    }
  );

  auto apiThread = std::async(std::launch::async,
    [&apiServer, &close]() {
      try {
        apiServer.Run(ADDRESS, PORT + 1, close);
      }
      catch (const std::exception& e) {
        GLog::Log(GLog::LOG_ERROR, e.what());
        close = true;
      }
    }
  );

  while (!close)
  {
    continue;
  }

  return 0;
}
