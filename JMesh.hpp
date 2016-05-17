#include "JMesh.h"

using namespace Javelin;

template <typename VertexType, typename VertexAllocator>
CMesh<VertexType, VertexAllocator>::~CMesh() noexcept {
}

template <typename VertexType, typename VertexAllocator>
template <class ConcreteLoader>
void CMesh<VertexType, VertexAllocator>::Initialize(const CMeshLoader<ConcreteLoader>& loader) {
	for (const auto& it : loader.GetObjectList()) {
		m_object.emplace(std::piecewise_construct, std::make_tuple(it.first), std::make_tuple());
	}
	for (const auto& it : loader.GetObjectList()) {
		for (const auto& it2 : it.second) {
			std::vector<VertexType> res;
			for (const auto& it3 : it2.second.vertexList) {
				res.emplace_back(VertexAllocator::Convert(it3));
			}

			m_object.at(it.first).emplace(std::piecewise_construct, 
				std::make_tuple(it2.first), std::make_tuple());

			m_object.at(it.first).at(it2.first).vertex.
				Initialize(res.size(), res.data(), true);
			m_object.at(it.first).at(it2.first).index.
				Initialize(it2.second.indexList.size(), it2.second.indexList.data(), true);
		}
	}

	for (const auto& it : loader.GetMaterialList()) {
		m_material.emplace(std::piecewise_construct, std::make_tuple(it.first), std::make_tuple());
		auto& mat = m_material.at(it.first);
		mat.materialParam = it.second;
		if (!it.second.diffuseMap.empty()) {
			mat.diffuseMap.Initialize(it.second.diffuseMap);
		}
		if (!it.second.specularMap.empty()) {
			mat.specularMap.Initialize(it.second.specularMap);
		}
		if (!it.second.ambientMap.empty()) {
			mat.ambientMap.Initialize(it.second.ambientMap);
		}
		if (!it.second.bumpMap.empty()) {
			mat.bumpMap.Initialize(it.second.bumpMap);
		}
		if (!it.second.opacityMap.empty()) {
			mat.opacityMap.Initialize(it.second.opacityMap);
		}
		if (!it.second.reflectionMap.empty()) {
			mat.reflectionMap.Initialize(it.second.reflectionMap);
		}
	}
}

template <typename VertexType, typename VertexAllocator>
void CMesh<VertexType, VertexAllocator>::Cleanup() noexcept {
	m_vertex.clear();
	m_index.clear();
	m_material.clear();
}

