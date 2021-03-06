#include "PlantDistributor.h"
#include "core/Trace.h"
#include "voxel/World.h"

namespace frontend {

void distributePlants(const voxel::WorldPtr& world, const glm::ivec3& pos, std::vector<glm::vec3>& translations) {
	core_trace_scoped(WorldRendererDistributePlants);
	const glm::ivec3& size = world->meshSize();
	core::Random random(pos.x);
	const voxel::BiomeManager& biomeMgr = world->biomeManager();
	std::vector<glm::vec2> positions;
	const voxel::Region region(pos.x, 0, pos.z, pos.x + size.x - 1, voxel::MAX_TERRAIN_HEIGHT, pos.z + size.z - 1);
	biomeMgr.getPlantPositions(region, positions, random, 5);
	for (const glm::vec2& p : positions) {
		const int y = world->findFloor(p.x, p.y, voxel::isFloor);
		if (y == voxel::NO_FLOOR_FOUND || y < voxel::MAX_WATER_HEIGHT) {
			continue;
		}
		const glm::ivec3 translation(p.x, y, p.y);
		translations.push_back(translation);
	}
}

}
