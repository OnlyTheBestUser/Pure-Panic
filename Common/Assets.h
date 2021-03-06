/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include <string>

namespace NCL {
	namespace Assets {
#ifdef _WIN64
		const std::string AUDIODIR("../../Assets/Audio/");
		const std::string SHADERDIR("../../Assets/Shaders/");
		const std::string MESHDIR("../../Assets/Meshes/");
		const std::string TEXTUREDIR("../../Assets/Textures/");
		const std::string FONTSSDIR("../../Assets/Fonts/");
		const std::string DATADIR("../../Assets/Data/");
		const std::string MAPDIR("../../Assets/Maps/");
#endif
#ifdef _ORBIS
		const std::string AUDIODIR("/app0/Assets/Audio/");
		const std::string SHADERDIR("/app0/Assets/Shaders/");
		const std::string MESHDIR("/app0/Assets/Meshes/");
		const std::string TEXTUREDIR("/app0/Assets/Textures/");
		const std::string FONTSSDIR("/app0/Assets/Fonts/");
		const std::string DATADIR("/app0/Assets/Data/");
		const std::string MAPDIR("/app0/Assets/Maps/");
#endif
		extern bool ReadTextFile(const std::string &filepath, std::string& result);
		extern bool ReadBinaryFile(const std::string &filepath, char** into, size_t& size);
	}
}