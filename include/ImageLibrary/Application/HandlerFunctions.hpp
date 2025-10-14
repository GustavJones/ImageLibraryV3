#pragma once
#include "GParsing/GParsing.hpp"

namespace ImageLibrary {
  void SetupHandling();
  bool HandleREST(GParsing::HTTPRequest _req, GParsing::HTTPResponse& _resp, bool& _closeConnection);
  bool HandleRESTPost(GParsing::HTTPRequest _req, GParsing::HTTPResponse& _resp);
}