#include "JObjLoader.h"
#include <fstream>
#include "JUtility.h"
#include "JApplication.h"

using namespace Javelin;

const std::string CObjLoader::commandList[] = {
	"mtllib",
	"g",
	"usemtl",
	"v",
	"vt",
	"vn",
	"f",
};

const std::string CObjLoader::commandListMtl[] = {
	"newmtl",
	"Kd",
	"Ks",
	"Ka",
	"d",
	"Ns",
	"Ni",
	"map_Kd",
	"map_Ks",
	"map_Ka",
	"map_Bump",
	"map_D",
	"refl",
};

const std::function<void(CObjLoader*, CObjLoader::commandIt begin, CObjLoader::commandIt end)>
CObjLoader::commandExecutor[array_length(commandList)] = {
	&mtllib,
	&g,
	&usemtl,
	&v,
	&vt,
	&vn,
	&f,
};

const std::function<void(CObjLoader*, CObjLoader::commandIt begin, CObjLoader::commandIt end)>
CObjLoader::commandExecutorMtl[array_length(commandListMtl)] = {
	&newmtl,
	&Kd,
	&Ks,
	&Ka,
	&d,
	&Ns,
	&Ni,
	&map_Kd,
	&map_Ks,
	&map_Ka,
	&map_Bump,
	&map_D,
	&refl,
};

void CObjLoader::mtllib(commandIt begin, commandIt end) {
	if (begin == end) {
		Application::WriteLog("コマンド数が足りません");
		throw - 1;
	}
	LoadMaterial(m_directory + *begin);
}

void CObjLoader::g(commandIt begin, commandIt end) {
	m_currentObject = *begin;
	m_vertexCache.clear();
}

void CObjLoader::usemtl(commandIt begin, commandIt end) {
	m_currentMaterial = *begin;
	m_vertexCache.clear();
}

void CObjLoader::v(commandIt begin, commandIt end) {
	float val[3];
	std::fill_n(val, 3, 0.0f);
	for (UINT i = 0; i < 3 && begin != end; i++) {
		val[i] = std::stof(*begin++);
	}
	m_positionList.emplace_back(val);
}

void CObjLoader::vt(commandIt begin, commandIt end) {
	float val[2];
	std::fill_n(val, 2, 0.0f);
	for (UINT i = 0; i < 2 && begin != end; i++) {
		val[i] = std::stof(*begin++);
	}
	m_texelList.emplace_back(val);
}

void CObjLoader::vn(commandIt begin, commandIt end) {
	float val[3];
	std::fill_n(val, 3, 0.0f);
	for (UINT i = 0; i < 3 && begin != end; i++) {
		val[i] = std::stof(*begin++);
	}
	m_normalList.emplace_back(val);
}

void CObjLoader::f(commandIt begin, commandIt end) {
	if (end - begin < 3) {
		Application::WriteLog("頂点が少ないため面を定義できません");
		throw - 1;
	}

	typedef struct {
		UINT value[3];
	}index_t;
	std::vector<index_t> index;
	while (begin != end) {
		std::vector<std::string> token;
		Split(*begin, token, '/');
		index_t temp;
		std::fill_n(temp.value, 3, 0);
		for (UINT i = 0; i < token.size(); i++) {
			temp.value[i] = std::stoi(token.at(i));
		}
		index.emplace_back(temp);

		begin++;
	}

	for (UINT i = 1; i < index.size() - 1; i++) {
		for (auto j : {(UINT)0, i, i + 1}) {
			auto& indexList = (*m_objectList)[m_currentObject][m_currentMaterial].indexList;
			auto& vertexList = (*m_objectList)[m_currentObject][m_currentMaterial].vertexList;

			if (m_vertexCache.find(index.at(j).value[0]) != m_vertexCache.end()) {
				auto& it = m_vertexCache.at(index.at(j).value[0]);
				if (it.find(index.at(j).value[1]) != it.end()) {
					auto& it2 = it.at(index.at(j).value[1]);
					if (it2.find(index.at(j).value[2]) != it2.end()) {
						indexList.emplace_back(it2.at(index.at(j).value[2]));
						continue;
					}
				}
			}

			m_vertexCache[index.at(j).value[0]][index.at(j).value[1]][index.at(j).value[2]] =
				(*m_objectList)[m_currentObject][m_currentMaterial].vertexList.size();

			vertex_t vertex;
			vertex.position = m_positionList.at(index.at(j).value[0]);
			vertex.texel = m_texelList.at(index.at(j).value[1]);
			vertex.normal = m_normalList.at(index.at(j).value[2]);
			vertexList.emplace_back(vertex);

			indexList.emplace_back(vertexList.size() - 1);
		}
	}
}

void CObjLoader::newmtl(commandIt begin, commandIt end) {
	m_currentMaterial = *begin;
}

void CObjLoader::Kd(commandIt begin, commandIt end) {
	float col[4];
	std::fill_n(col, 4, 1.0f);
	for (UINT i = 0; i < 4 && begin != end; i++) {
		col[i] = std::stof(*begin++);
	}
	(*m_materialList)[m_currentMaterial].diffuse = col;
}

void CObjLoader::Ks(commandIt begin, commandIt end) {
	float col[4];
	std::fill_n(col, 4, 1.0f);
	for (UINT i = 0; i < 4 && begin != end; i++) {
		col[i] = std::stof(*begin++);
	}
	(*m_materialList)[m_currentMaterial].specular = col;
}

void CObjLoader::Ka(commandIt begin, commandIt end) {
	float col[4];
	std::fill_n(col, 4, 1.0f);
	for (UINT i = 0; i < 4 && begin != end; i++) {
		col[i] = std::stof(*begin++);
	}
	(*m_materialList)[m_currentMaterial].ambient = col;
}

void CObjLoader::d(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].opacity = std::stof(*begin);
}

void CObjLoader::Ns(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].reflection = std::stof(*begin);
}

void CObjLoader::Ni(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].refraction = std::stof(*begin);
}

void CObjLoader::map_Kd(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].diffuseMap = m_directory + *begin;
}

void CObjLoader::map_Ks(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].specularMap = m_directory + *begin;
}

void CObjLoader::map_Ka(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].ambientMap = m_directory + *begin;
}

void CObjLoader::map_Bump(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].bumpMap = m_directory + *begin;
}

void CObjLoader::map_D(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].opacityMap = m_directory + *begin;
}

void CObjLoader::refl(commandIt begin, commandIt end) {
	(*m_materialList)[m_currentMaterial].reflectionMap = m_directory + *begin;
}

void CObjLoader::LoadMaterial(const std::string& filename) {
	std::ifstream ifs(filename);

	if (!ifs) {
		Application::WriteLog("ファイルのオープンに失敗しました：" + filename);
		throw - 1;
	}

	std::string str;
	int index = 0;
	while (std::getline(ifs, str)) {
		std::vector<std::string> token;
		Javelin::Split(str, token, ' ');

		if (token.empty() || token.at(0) == "#") {
			index++;
			continue;
		}
		for (UINT i = 0; i < array_length(commandListMtl); i++) {
			if (token.at(0) == commandListMtl[i]) {
				try {
					commandExecutorMtl[i](this, ++token.begin(), token.end());
				} catch (...) {
					Application::WriteLog(std::to_string(index) + "行目に問題があります：" + filename);
				}
				break;
			}
		}
		index++;
	}
}

void CObjLoader::Load(const std::string& filename,
	MeshObjectList* objectList,
	MeshMaterialList* materialList) {

	m_objectList = objectList;
	m_materialList = materialList;

	{
		m_directory.clear();
		std::vector<std::string> token;
		Javelin::Split(filename, token, "/");
		for (UINT i = 0; i < token.size() - 1; i++) {
			m_directory.append(token.at(i) + '/');
		}
	}

	std::ifstream ifs(filename);

	if (!ifs) {
		Application::WriteLog("ファイルのオープンに失敗しました：" + filename);
		throw - 1;
	}

	m_positionList.clear();
	m_normalList.clear();
	m_texelList.clear();
	m_positionList.emplace_back(0.0f, 0.0f, 0.0f);
	m_normalList.emplace_back(0.0f, 0.0f, 0.0f);
	m_texelList.emplace_back(0.0f, 0.0f);

	std::string str;
	int index = 0;
	while (std::getline(ifs, str)) {
		std::vector<std::string> token;
		Javelin::Split(str, token, " ");

		if (token.empty() || token.at(0) == "#") {
			index++;
			continue;
		}
		for (UINT i = 0; i < array_length(commandList); i++) {
			if (token.at(0) == commandList[i]) {
				try {
					commandExecutor[i](this, ++token.begin(), token.end());
				} catch (...) {
					Application::WriteLog(std::to_string(index) + "行目に問題があります：" + filename);
				}
				break;
			}
		}
		index++;
	}
}
