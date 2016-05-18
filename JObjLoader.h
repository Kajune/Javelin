#pragma once
#include "JMeshLoader.h"
#include <functional>

namespace Javelin {

	class CObjLoader {
		MeshObjectList*		m_objectList;
		MeshMaterialList*	m_materialList;

		std::string			m_currentObject;
		std::string			m_currentMaterial;

		using commandIt = std::vector<std::string>::iterator;

		static const std::string commandList[];
		static const std::string commandListMtl[];

		static const std::function<void(CObjLoader*,
			commandIt begin, commandIt end) > commandExecutor[];
		static const std::function<void(CObjLoader*,
			commandIt begin, commandIt end) > commandExecutorMtl[];

		std::vector<XMFLOAT3>	m_positionList;
		std::vector<XMFLOAT3>	m_normalList;
		std::vector<XMFLOAT2>	m_texelList;

		std::map<UINT, std::map<UINT, std::map<UINT, UINT>>>	m_vertexCache;

		std::string	m_directory;

		void mtllib(commandIt begin, commandIt end);
		void g(commandIt begin, commandIt end);
		void usemtl(commandIt begin, commandIt end);
		void v(commandIt begin, commandIt end);
		void vt(commandIt begin, commandIt end);
		void vn(commandIt begin, commandIt end);
		void f(commandIt begin, commandIt end);

		void newmtl(commandIt begin, commandIt end);
		void Kd(commandIt begin, commandIt end);
		void Ks(commandIt begin, commandIt end);
		void Ka(commandIt begin, commandIt end);
		void d(commandIt begin, commandIt end);
		void Ns(commandIt begin, commandIt end);
		void Ni(commandIt begin, commandIt end);
		void map_Kd(commandIt begin, commandIt end);
		void map_Ks(commandIt begin, commandIt end);
		void map_Ka(commandIt begin, commandIt end);
		void map_Bump(commandIt begin, commandIt end);
		void map_D(commandIt begin, commandIt end);
		void refl(commandIt begin, commandIt end);

		void LoadMaterial(const std::string& filename);
	public:
		void Load(const std::string& filename, 
			MeshObjectList* objectList, 
			MeshMaterialList* materialList);
	};

}
