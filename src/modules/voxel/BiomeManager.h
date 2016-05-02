#pragma once

#include "core/Common.h"
#include "noise/SimplexNoise.h"
#include "Voxel.h"

namespace voxel {

struct Biome {
	Voxel voxel;
	int16_t yMin;
	int16_t yMax;
	float humidity;
	float temperature;

	Biome* next;
};

class BiomeManager {
private:
	Biome bioms[MAX_TERRAIN_HEIGHT];

	const Voxel INVALID = createVoxel(Air);
	const Voxel ROCK = createVoxel(Rock);
	const Voxel GRASS = createVoxel(Grass);

public:
	BiomeManager();

	bool addBiom(int lower, int upper, float humidity, float temperature, const Voxel& type);

	// this lookup must be really really fast - it is executed once per generated voxel
	inline const Voxel& getVoxelType(const glm::ivec3& pos, bool underground = false, float noise = 1.0f) const {
		if (pos.y < 0 || pos.y >= int(SDL_arraysize(bioms))) {
			return INVALID;
		}
		if (underground) {
			return ROCK;
		}
		core_assert(noise >= 0.0f && noise <= 1.0f);
		const Biome* biome = getBiome(pos, noise);
		if (biome == nullptr) {
			return GRASS;
		}
		return biome->voxel;
	}

	inline const Voxel& getVoxelType(int x, int y, int z, bool underground = false, float noise = 1.0f) const {
		return getVoxelType(glm::ivec3(x, y, z), underground, noise);
	}

	inline bool hasTrees(const glm::ivec3& pos, float noise = 1.0f) const {
		if (pos.y < MAX_WATER_HEIGHT) {
			return false;
		}
		const Biome* biome = getBiome(pos, noise);
		if (biome == nullptr) {
			return false;
		}
		if (biome->voxel.getMaterial() != Grass) {
			return false;
		}
		return biome->temperature > 0.3f && biome->humidity > 0.3f;
	}

	inline const Biome* getBiome(const glm::ivec3& pos, float noise = 1.0f) const {
		core_assert_msg(noise >= 0.0f && noise <= 1.0f, "noise must be normalized [-1.0,1.0]: %f", noise);
		const int index = glm::clamp(int(pos.y * noise), 0, int(SDL_arraysize(bioms))- 1);
		const Biome* biome = &bioms[index];
		const glm::vec4 noisePos(pos.x, pos.y, pos.z, noise);
		const float humidityNoise = noise::Simplex::Noise4D(noisePos, 1, 1.0f, 1.0f, 1.0f);
		const float humidityNoiseNorm = noise::norm(humidityNoise);
		//while (biome != nullptr) {
		// TODO: humidity and temperature noise map
		//biome = biome->next;
		//}
		return biome;
	}

	inline bool hasClouds(const glm::ivec3& pos, float noise = 1.0f) const {
		if (pos.y <= MAX_TERRAIN_HEIGHT) {
			return false;
		}
		const Biome* biome = getBiome(pos, noise);
		if (biome == nullptr) {
			return false;
		}
		return biome->humidity >= 0.5f;
	}
};

}
