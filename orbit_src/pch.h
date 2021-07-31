#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <exception>

#include <Windows.h>
#include <plog/Log.h>
#include <date/date.h>
#include <fmt/format.h>

#include <string_theory/st_string.h>
#include <string_theory/st_utf_conv.h>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/json.hpp>