//
// Created by freezestudio on 2017/12/1.
//

#ifndef FREEGL_PBR_MODEL_H
#define FREEGL_PBR_MODEL_H

namespace freeze
{
	struct pbr_vertex
	{
		float x,y,z;
		float nx,ny,nz;
		float u,v;
	};

	template<typename Vertex>
	class pbr_mesh
	{
	public:
		pbr_mesh()
		{
			vecVertices.clear();
			vecIndices.clear();
			mapTextures.clear();
		}

		void load(std::vector<Vertex>& v,
			std::vector<GLuint>& i,
			std::map<std::string,std::vector<char>>& d)
		{
			vecVertices.swap(v);
			vecIndices.swap(i);
			for (auto data : d)
			{
				auto tex = make_texture2d();
				quickly_set_texture2d(tex, data.second);
				if (data.first == "BaseColor")
				{
					mapTextures.insert(std::make_pair(0, tex));
				}
				else if (data.first == "Normal")
				{
					mapTextures.insert(std::make_pair(1, tex));
				}
				else if (data.first == "ORM")
				{
					mapTextures.insert(std::make_pair(2, tex));
				}
			}

			vao.bind();
			auto vbo = make_vertex_buffer();
			auto ebo = make_element_buffer();
			ebo.bind();
			ebo.copy_data(&vecIndices[0], vecIndices.size() * sizeof(GLuint));
			//ebo.unbind();
			quickly_set_vbo(vbo, &vecVertices[0], vecVertices.size() * sizeof(Vertex), 3, 3, 2);
			
			vao.unbind();
		}

		void draw(program& p)
		{
			auto index = 0u;
			for (auto tex : mapTextures)
			{
				tex.second.active(tex.first);
				tex.second.bind();
			}

			vao.bind();
			glDrawElements(GL_TRIANGLES, vecIndices.size(), GL_UNSIGNED_INT, 0);
			vao.unbind();
		}
	private:
		std::vector<Vertex> vecVertices;
		std::vector<GLuint> vecIndices;
		std::map<GLuint,texture2d> mapTextures;
		vertex_array_buffer vao;
	};

	template<typename Vertex>
	class pbr_model
	{
	public:
		pbr_model()
		{
			vecMeshs.clear();
		}

		void load(std::string const& file)
		{
			auto pos = file.find_last_of("\\/");
			model_path = file.substr(0, pos+1);
			model_name = file.substr(pos+1, file.size() - pos - 4-1);//length(".obj")==4

			auto importer = make<Assimp::Importer>();
			auto scene = importer.ReadFile(file, 
				aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs);

			process_node(scene->mRootNode,scene);
		}
	private:
		void process_node(aiNode const* node,aiScene const* scene)
		{
			for (auto i = 0u; i < node->mNumMeshes; ++i)
			{
				auto sub_mesh = scene->mMeshes[node->mMeshes[i]];
				vecMeshs.push_back(process_mesh(sub_mesh));
			}

			for (auto i = 0u; i < node->mNumChildren; ++i)
			{
				process_node(node->mChildren[i], scene);
			}
		}

		pbr_mesh<Vertex> process_mesh(aiMesh const* mesh)
		{
			//顶点
			std::vector<Vertex> vecVerices;
			for (auto i = 0u; i < mesh->mNumVertices; ++i)
			{
				Vertex V = { };
				V.x = mesh->mVertices[i].x;
				V.y = mesh->mVertices[i].y;
				V.z = mesh->mVertices[i].z;
				V.nx = mesh->mNormals[i].x;
				V.ny = mesh->mNormals[i].y;
				V.nz = mesh->mNormals[i].z;
				if (mesh->mTextureCoords[0])
				{
					V.u = mesh->mTextureCoords[0][i].x;
					V.v = mesh->mTextureCoords[0][i].y;
				}
				vecVerices.emplace_back(V);
			}

			//索引
			std::vector<GLuint> vecIndices;
			for (auto i = 0u; i < mesh->mNumFaces; ++i)
			{
				auto face = mesh->mFaces[i];
				for (auto x = 0u; x < face.mNumIndices; ++x)
				{
					auto indice = face.mIndices[x];
					vecIndices.emplace_back(indice);
				}
			}

			//纹理
			std::map<std::string, std::vector<char>> mapTextures;
			auto texture_name = model_name + "_BaseColor.png"s;
			mapTextures.insert(std::make_pair("BaseColor"s, load_texture(texture_name)));
			texture_name = model_name + "_Normal.png"s;
			mapTextures.insert(std::make_pair("Normal"s, load_texture(texture_name)));
			texture_name = model_name + "_OcclsionRoughnessMetallic.png"s;
			mapTextures.insert(std::make_pair("ORM"s, load_texture(texture_name)));

			//装载一个网格
			auto pbrMesh = make<pbr_mesh<Vertex>>();
			pbrMesh.load(vecVerices, vecIndices, mapTextures);
			return pbrMesh;
		}

		std::vector<char> load_texture(std::string const& name)
		{
			auto file = model_path + name;
			std::vector<char> buffer;
			std::ifstream ifs{ file };
			if (ifs.is_open())
			{
				ifs.seekg(0, std::ios::end);
				auto length = static_cast<std::size_t>(ifs.tellg());
				ifs.seekg(0, std::ios::beg);
				if (length > 0)
				{
					buffer.resize(length);
					ifs.read(buffer.data(), length);
					ifs.close();
				}
			}
			return buffer;
		}

	private:
		std::vector<pbr_mesh<Vertex>> vecMeshs;
		std::string model_path;
		std::string model_name;
	};
}

namespace freeze
{
	using pbrmodel = pbr_model<pbr_vertex>;
}

#endif //FREEGL_PBR_MODEL_H