#pragma once

#include "JMeshLoader.h"
#include "JVertexAndIndexBuffer.h"

namespace Javelin {

	struct CDefaultVertexAllocator {
		static vertex_t Convert(const vertex_t& in) {
			return in;
		}
	};

	template <typename VertexType = vertex_t, typename VertexAllocator = CDefaultVertexAllocator>
	class CMesh {
		typedef struct {
			CVertexBuffer<VertexType> vertex;
			CIndexBuffer index;
		}meshObject_t;
		mutable std::map<std::string, std::map<std::string, meshObject_t>>	m_object;

		using matIt_t = typename std::map<std::string, meshObject_t>::iterator;
		using objIt_t = typename std::map<std::string, std::map<std::string, meshObject_t>>::iterator;

		typedef struct {
			material_t	materialParam;
			CImage		diffuseMap, specularMap, ambientMap, bumpMap, opacityMap, reflectionMap;
		}meshMaterial_t;
		std::map<std::string, meshMaterial_t>						m_material;
	public:
		virtual ~CMesh() noexcept;

		template <class ConcreteLoader>
		void Initialize(const CMeshLoader<ConcreteLoader>& loader);
		void Cleanup() noexcept;

		class meshIterator : public std::iterator<std::input_iterator_tag, meshObject_t> {
			objIt_t m_objIt;
			objIt_t m_end;
			matIt_t m_matIt;
		public:
			meshIterator(const objIt_t& objIt, const matIt_t& matIt, const objIt_t& end) {
				m_objIt = objIt;
				m_matIt = matIt;
				m_end = end;
			}
			meshIterator& operator++() {
				m_matIt++;
				if (m_matIt == m_objIt->second.end()) {
					m_objIt++;
					if (m_objIt != m_end) {
						m_matIt = m_objIt->second.begin();
					}
				}
				return *this;
			}
			meshIterator& operator++(int) {
				return operator++();
			}
			const meshObject_t& operator*() {
				return m_matIt->second;
			}
			const meshObject_t* operator->() {
				return &m_matIt->second;
			}
			bool operator!=(const meshIterator& it) {
				return !operator==(it);
			}
			bool operator==(const meshIterator& it) {
				return m_objIt == it.m_objIt && m_matIt == it.m_matIt;
			}

			const std::string& GetMaterialName() const {
				return m_matIt->first;
			}
		};

		meshIterator begin() const {
			return meshIterator(m_object.begin(), m_object.begin()->second.begin(), m_object.end());
		}
		meshIterator end() const {
			return meshIterator(m_object.end(), m_object.rbegin()->second.end(), m_object.end());
		}
		const meshMaterial_t& GetMaterial(const meshIterator& it) const {
			return m_material.at(it.GetMaterialName());
		}
	};

}
