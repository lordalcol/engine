#include "core/benchmark/AbstractBenchmark.h"
#include "voxel/WorldPager.h"
#include "voxel/polyvox/PagedVolume.h"
#include "voxel/polyvox/CubicSurfaceExtractor.h"
#include "voxel/WorldContext.h"
#include "voxel/BiomeManager.h"
#include "voxel/Constants.h"
#include "voxel/IsQuadNeeded.h"

class PagedVolumeBenchmark: public core::AbstractBenchmark {
protected:
	voxel::BiomeManager _biomeManager;
	voxel::WorldContext _ctx;

public:
	void onCleanupApp() override {
		_biomeManager.shutdown();
	}

	bool onInitApp() override {
		voxel::initDefaultMaterialColors();
		const std::string& luaParameters = core::App::getInstance()->filesystem()->load("world.lua");
		const std::string& luaBiomes = core::App::getInstance()->filesystem()->load("biomes.lua");
		Log::info("%s", luaParameters.c_str());
		Log::info("%s", luaBiomes.c_str());
		_biomeManager.init(luaBiomes);
		_ctx.load(luaParameters);
		return true;
	}
};

BENCHMARK_DEFINE_F(PagedVolumeBenchmark, pageIn) (benchmark::State& state) {
	const uint16_t chunkSideLength = state.range(0);
	const uint32_t volumeMemoryMegaBytes = chunkSideLength * 2;
	voxel::WorldPager pager;
	pager.setSeed(0l);
	pager.setPersist(false);
	voxel::PagedVolume *volumeData = new voxel::PagedVolume(&pager, volumeMemoryMegaBytes * 1024 * 1024, chunkSideLength);
	pager.init(volumeData, &_biomeManager, &_ctx);
	const glm::ivec3 meshSize(16, 128, 16);
	int x = 0;
	while (state.KeepRunning()) {
		glm::ivec3 mins(x, 0, 0);
		x += meshSize.x;
		voxel::Region region(mins, mins + meshSize);
		voxel::Mesh mesh(0, 0, true);
		voxel::Mesh waterMesh(0, 0, true);
		voxel::extractAllCubicMesh(volumeData, region, &mesh, &waterMesh, voxel::IsQuadNeeded(), voxel::IsWaterQuadNeeded(), voxel::MAX_WATER_HEIGHT);
	}
	delete volumeData;
}

BENCHMARK_REGISTER_F(PagedVolumeBenchmark, pageIn)->RangeMultiplier(2)->Range(8, 256);

BENCHMARK_MAIN()
