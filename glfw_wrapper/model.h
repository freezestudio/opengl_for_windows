//
// Created by freezestudio on 2017/12/2.
//

#ifndef FREEGL_MODEL_H
#define FREEGL_MODEL_H

namespace freeze
{
    struct model_vertex
    {
        //Position 顶点坐标
        float x, y, z;
        //Normal 法线
        float nx, ny, nz;
        //TexCoords 纹理
        float u, v;
    };

    struct mesh_data
    {
        std::vector<model_vertex> Vertices;
        std::vector<unsigned> Indices;
        std::vector<char> Albedo;
        std::vector<char> Normal;
        std::vector<char> Orm;
    };
}
namespace freeze
{
    template<typename Vertex,bool Delay>
    class mesh_t
    {
    public:
        mesh_t()
        {

        }

    public:
        void load(std::vector<Vertex>& v,
            std::vector<GLuint>& i,
            std::vector<texture2d>& t)
        {
            vecVertices = v;
            vecIndices = i;
            vecTextures = t;
        }

        void setup(program& shader)
        {
            if (!vao && vao.is_delayed())
            {
                vao.set();
            }
            vao.bind();
            auto vbo = make_vertex_buffer();
            vbo.bind();
            vbo.copy_data(&vecVertices[0], vecVertices.size() * sizeof(Vertex));
            auto ebo = make_element_buffer();
            ebo.bind();
            ebo.copy_data(&vecIndices[0], vecIndices.size() * sizeof(GLuint));
            auto vp = make_vertex();
            vp.set(0, 3, GL_FLOAT, 8 * sizeof(float), 0);
            vp.set(1, 3, GL_FLOAT, 8 * sizeof(float), 3 * sizeof(float));
            vp.set(2, 2, GL_FLOAT, 8 * sizeof(float), 6 * sizeof(float));
            vbo.unbind();
            //ebo.unbind();
            vao.unbind();

            shader.use();
            shader.set_int("material.albedo", 0);
            shader.set_int("material.normal", 1);
            shader.set_int("material.orm", 2);
        }

        void draw(program& shader)
        {
            shader.use();
            vecTextures[0].active(0);
            vecTextures[0].bind();
            vecTextures[1].active(1);
            vecTextures[1].bind();
            vecTextures[2].active(2);
            vecTextures[2].bind();

            vao.bind();
            glDrawElements(GL_TRIANGLES, vecIndices.size(), GL_UNSIGNED_INT, 0);
            vao.unbind();
        }
    private:
        std::vector<Vertex> vecVertices;
        std::vector<GLuint> vecIndices;
        std::vector<texture2d> vecTextures;
        vertex_array_buffer_t<Delay> vao;
    };
}

namespace freeze
{
    template<typename Vertex, bool Delay>
    class model_t
    {
    public:
        struct mesh_list
        {
            mesh_t<Vertex,Delay> mesh;
            mesh_data      data;
        };

        model_t()
        {

        }
    public:
        void load(std::string const& file)
        {
            auto pos = file.find_last_of("\\/")+1;
            dir = file.substr(0, pos);
            name = file.substr(pos, file.size() - pos - 4);//length(".obj")==4

            auto importer = make<Assimp::Importer>();
            auto scene = importer.ReadFile(file,
                aiProcessPreset_TargetRealtime_MaxQuality/* | aiProcess_FlipUVs*/);
            if (!scene)return;
            process_node(scene->mRootNode, scene);
        }

        void setup(program& shader)
        {
            for (auto& list : vecMeshs)
            {  
                std::vector<texture2d> texs =
                    to_texture(
                        std::vector<std::vector<char>>{ list.data.Albedo,list.data.Normal,list.data.Orm });
                list.mesh.load(list.data.Vertices, list.data.Indices, texs);
                list.mesh.setup(shader);
            }
        }

        void draw(program& shader)
        {
            for (auto& list : vecMeshs)
            {
                list.mesh.draw(shader);
            }
        }
    private:
        void process_node(aiNode const* node, aiScene const* scene)
        {
            for (auto i = 0u; i < node->mNumMeshes; ++i)
            {
                auto mesh = scene->mMeshes[node->mMeshes[i]];
                process_mesh(mesh);
            }

            for (auto i = 0u; i < node->mNumChildren; ++i)
            {
                process_node(node->mChildren[i], scene);
            }
        }

        void process_mesh(aiMesh const* mesh)
        {
            //顶点
            std::vector<Vertex> vecVerices;
            for (auto i = 0u; i < mesh->mNumVertices; ++i)
            {
                Vertex V = {};
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
            auto texture_name = name + "_BaseColor.png"s;
            auto albedo =load_texture(texture_name);
            texture_name = name + "_Normal.png"s;
            auto normal = load_texture(texture_name);
            texture_name = name + "_OcclusionRoughnessMetallic.png"s;
            auto orm = load_texture(texture_name);

            //装载一个网格
            auto tMesh = make<mesh_t<Vertex,Delay>>();
            mesh_data dataMesh = {
                vecVerices,vecIndices,albedo,normal,orm,
            };

            vecMeshs.emplace_back(mesh_list{ tMesh,dataMesh });
        }

        std::vector<char> load_texture(std::string const& name)
        {
            auto file = dir + name;
            std::vector<char> buffer;
            std::ifstream ifs{ file,std::ios::binary };
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

        std::vector<texture2d> to_texture(std::vector<std::vector<char>>& datas)
        {
            std::vector<texture2d> t2d;
            for (auto& d : datas)
            {
                auto tex = make_texture2d();
                tex.bind();
                tex.set_image(d);
                tex.mipmap();
                tex.set_wrap_s(GL_REPEAT);
                tex.set_wrap_t(GL_REPEAT);
                tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
                tex.set_mag_filter(GL_LINEAR);
                tex.unbind();
                t2d.emplace_back(tex);
            }
            return t2d;
        }
    private:
        std::vector<mesh_list> vecMeshs;
        std::string dir;
        std::string name;
    };
}

namespace freeze
{
    using modeless = model_t<model_vertex, true>;
    using modeled  = model_t<model_vertex, false>;
}

#endif