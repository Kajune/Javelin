#pragma once
#include <cmath>

template <unsigned int SizeX, unsigned int SizeY, unsigned int SizeZ>
class Voxel {
	typedef struct {
		float density = 0.0f;
	}cell_t;
	const float	m_threshould;
	const float	m_atten;
public:
	Voxel(float threshould, float atten);
	bool Move();

	cell_t		m_cellList[SizeX][SizeY][SizeZ];
};

namespace {
	typedef struct {
		int x, y, z;
	}pos_t;

	const pos_t offsetList[] = {
		{ 0,0,0 },
		{ 1,0,0 },
		{ 0,1,0 },
		{ 0,0,1 },
		{ 1,1,0 },
		{ 1,0,1 },
		{ 0,1,1 },
		{ 1,1,1 },
	};
}

template <unsigned int SizeX, unsigned int SizeY, unsigned int SizeZ>
Voxel<SizeX, SizeY, SizeZ>::Voxel(float threshould, float atten) :
	m_threshould(threshould), m_atten(atten) {
}

template <unsigned int SizeX, unsigned int SizeY, unsigned int SizeZ>
bool Voxel<SizeX, SizeY, SizeZ>::Move() {
	bool isMoved = false;
	for (auto odd_even : { -1, 1 }) {
		for (unsigned x = 1; x < SizeX - 1; x++) {
			for (unsigned y = 1; y < SizeY - 1; y++) {
				for (unsigned z = 1; z < SizeZ - 1; z++) {
					float density = m_cellList[x][y][z].density;
					if (density <= 0.0f) {
						continue;
					}
					bool isSteady = true;
					for (const auto& offset : offsetList) {
						float density_temp = m_cellList
							[x + offset.x * odd_even]
						[y + offset.y * odd_even]
						[z + offset.z * odd_even].density;
						if ((density >= m_threshould && density_temp < m_threshould) ||
							(density < m_threshould && density_temp >= m_threshould)) {
							isSteady = false;
							break;
						}
					}
					if (isSteady) {
						continue;
					}
					isMoved = true;
					float spreadDensity = 0.0f;
					int spreadNum = 0;
					for (const auto& offset : offsetList) {
						float& density_temp = m_cellList
							[x + offset.x * odd_even]
						[y + offset.y * odd_even]
						[z + offset.z * odd_even].density;
						if (density_temp >= m_threshould) {
							spreadDensity += density_temp * m_atten;
						} else {
							spreadNum++;
						}
					}
					for (const auto& offset : offsetList) {
						float& density_temp = m_cellList
							[x + offset.x * odd_even]
						[y + offset.y * odd_even]
						[z + offset.z * odd_even].density;
						if (density_temp >= m_threshould) {
							density_temp *= 1.0f - m_atten;
						} else {
							density_temp += spreadDensity / (float)spreadNum;
						}
					}
				}
			}
		}
	}
	return isMoved;
}

