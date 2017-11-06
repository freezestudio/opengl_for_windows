#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <array>
#include <fstream>
#include <sstream>
#include <memory>
#include <type_traits>
#include <filesystem>
#include <algorithm>

namespace fs = std::experimental::filesystem;

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "stb_image.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "base.h"
#include "shader.h"