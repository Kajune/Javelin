#pragma once

#include "JGlobal.h"
#include <vector>
#include <map>
#include "JImage.h"

namespace Javelin {

	typedef struct {
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT2 texel;
	}vertex_t;

	typedef struct {
		std::vector<vertex_t>	vertexList;
		std::vector<UINT>		indexList;
	}materialObject_t;

	typedef std::map<std::string, materialObject_t> object_t;

	typedef struct {
		COLOR diffuse, specular, ambient;
		float opacity, reflection, refraction;
		std::string diffuseMap, specularMap, ambientMap, bumpMap, opacityMap, reflectionMap;
	}material_t;

	using MeshObjectList = std::map<std::string, object_t>;
	using MeshMaterialList = std::map<std::string, material_t>;

	//
	//　このクラスはテンプレート引数としてメッシュデータの読み込みを行うポリシーを指定するポリシークラスです。
	//　ConcreteLoaderには、メッシュデータを読み込んで格納する
	//関数Load(string, map<string, object_t>*, map<string, material_t>*)が必要です。
	//
	template<typename ConcreteLoader>
	class CMeshLoader final {
	private:
		MeshObjectList	m_objectList;
		MeshMaterialList m_materialList;

		ConcreteLoader loader;

	public:
		void Initialize(const std::string& filename) noexcept {
			Cleanup();
			try {
				loader.Load(filename, &m_objectList, &m_materialList);
			} catch (...) {
				Application::WriteLog("MeshLoader::Initialize メッシュの読み込みに失敗しました：" + filename);
			}
		}
		void Cleanup() noexcept {
			m_objectList.clear();
			m_materialList.clear();
		}

		const std::map<std::string, object_t>& GetObjectList() const noexcept {
			return m_objectList;
		}
		const std::map<std::string, material_t>& GetMaterialList() const noexcept {
			return m_materialList;
		}
	};

}
