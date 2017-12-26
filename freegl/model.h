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
        //顶点
        std::vector<model_vertex> Vertices;
        //索引
        std::vector<unsigned>     Indices;
        //基本颜色
        std::vector<char>         Albedo;
        //法线
        std::vector<char>         Normal;
        //遮蔽度，粗糙度，金属度
        std::vector<char>         Orm;
        //3 texture
        std::vector<texture2d>    Textures;
    };

    struct mesh_data_2
    {
        //顶点
        std::vector<model_vertex> Vertices;
        //索引
        std::vector<unsigned>     Indices;
        //基本颜色
        tex_dataless              Albedo;
        //法线
        tex_dataless              Normal;
        //遮蔽度，粗糙度，金属度
        tex_dataless              Orm;
        //3 texture
        std::vector<texture2d>    Textures;
    };
}

namespace freeze
{
    template<typename Vertex, bool Delay>
    class mesh_t
    {
    public:
        mesh_t()
        {

        }

    public:
        void setup(
            program& shader,
            std::vector<Vertex>& vecVertices,
            std::vector<GLuint>& vecIndices)
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
            vertex::set_enable(0, 3, 8, 0);
            vertex::set_enable(1, 3, 8, 3);
            vertex::set_enable(2, 2, 8, 6);
            vbo.unbind();
            //ebo.unbind();
            vao.unbind();

            shader.use();
            shader.set_int("material.albedo", 0);
            shader.set_int("material.normal", 1);
            shader.set_int("material.orm", 2);
        }

        void draw(
            program& shader, 
            std::vector<texture2d>& vecTextures, 
            std::vector<GLuint>& vecIndices)
        {
            shader.use();

            if (vecTextures.size() >= 3)
            {
                vecTextures[0].active(0);
                vecTextures[0].bind();
                vecTextures[1].active(1);
                vecTextures[1].bind();
                vecTextures[2].active(2);
                vecTextures[2].bind();
            }

            vao.bind();
            glDrawElements(GL_TRIANGLES, vecIndices.size(), GL_UNSIGNED_INT, 0);
            vao.unbind();
        }

        void draw_instanced(
            program& shader,
            std::vector<texture2d>& vecTextures,
            std::vector<GLuint>& vecIndices,
            GLsizei count)
        {
            shader.use();

            if (vecTextures.size() >= 3)
            {
                vecTextures[0].active(0);
                vecTextures[0].bind();
                vecTextures[1].active(1);
                vecTextures[1].bind();
                vecTextures[2].active(2);
                vecTextures[2].bind();
            }

            vao.bind();
            glDrawElementsInstanced(GL_TRIANGLES, 
                vecIndices.size(), GL_UNSIGNED_INT, nullptr,count);
            vao.unbind();
        }
    public:
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
            mesh_t<Vertex, Delay> mesh;
            mesh_data             data;
        };

        struct mesh_list_2
        {
            mesh_t<Vertex, Delay> mesh;
            mesh_data_2           data;
        };

        model_t()
        {

        }

    public:
        void load(std::string const& file)
        {
            auto pos = file.find_last_of("\\/") + 1;
            _dir = file.substr(0, pos);
            _name = file.substr(pos, file.size() - pos - 4);//length(".obj")==4

            auto importer = make<Assimp::Importer>();
            auto scene = importer.ReadFile(file,
                aiProcessPreset_TargetRealtime_MaxQuality /*| aiProcess_FlipUVs*/);
            if (!scene)return;
            process_node(scene->mRootNode, scene);
        }

        void setup(program& shader)
        {
            for (auto& list : _vec_mesh_data)
            {
                //list.data.Textures = to_texture(list.data.Albedo,
                //    list.data.Normal, 
                //    list.data.Orm);
                list.data.Textures = to_texture_2(list.data.Albedo, 
                    list.data.Normal, 
                    list.data.Orm);
                list.mesh.setup(shader, list.data.Vertices, list.data.Indices);
            }
        }

        void draw(program& shader)
        {
            for (auto& list : _vec_mesh_data)
            {
                list.mesh.draw(shader,list.data.Textures,list.data.Indices);
            }
        }

        void draw_instanced(program& shader, GLsizei count)
        {
            for (auto& list : _vec_mesh_data)
            {
                list.mesh.draw_instanced(shader, list.data.Textures,
                    list.data.Indices, count);
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
            auto albedo_name = _name + "_BaseColor.png"s;
            auto normal_name = _name + "_Normal.png"s;
            auto orm_name = _name + "_OcclusionRoughnessMetallic.png"s;

            //auto albedo = load_texture(albedo_name);
            //auto normal = load_texture(normal_name);
            //auto orm = load_texture(orm_name);

            //auto albedo = tex_dataless{ _dir + albedo_name };
            //auto normal = tex_dataless{ _dir + normal_name };
            //auto orm = tex_dataless{ _dir + orm_name };

            auto albedo = tex_dataless{ load_texture(albedo_name) };
            auto normal = tex_dataless{ load_texture(normal_name) };
            auto orm = tex_dataless{ load_texture(orm_name) };

            //装载一个网格
            auto mesh_instance = make<mesh_t<Vertex, Delay>>();
            //mesh_data data = {
            //    vecVerices,vecIndices,albedo,normal,orm,
            //};
            //_vec_mesh_data.emplace_back(mesh_list{ mesh_instance,data });
            mesh_data_2 data = {
                vecVerices,vecIndices,albedo,normal,orm
            };
            _vec_mesh_data.emplace_back(mesh_list_2{mesh_instance,data});
        }

        std::vector<char> load_texture(std::string const& name)
        {
            auto file = _dir + name;
            std::vector<char> buffer;

            auto path = fs::path{ file };
            if (!fs::exists(path))return buffer;

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

        std::vector<texture2d> to_texture(
            std::vector<char> const& albedo,
            std::vector<char> const& normal,
            std::vector<char> const& orm)
        {
            std::vector<texture2d> t2d;
            if(!albedo.empty())  t2d.emplace_back(to_texture(albedo));
            if (!normal.empty()) t2d.emplace_back(to_texture(normal));
            if (!orm.empty())    t2d.emplace_back(to_texture(orm));
            return t2d;
        }

        std::vector<texture2d> to_texture_2(
            tex_dataless& albedo,
            tex_dataless& normal,
            tex_dataless& orm)
        {
            std::vector<texture2d> t2d;
            if(!albedo.isnull()) t2d.emplace_back(to_texture_2(albedo));
            if (!normal.isnull()) t2d.emplace_back(to_texture_2(normal));
            if (!orm.isnull()) t2d.emplace_back(to_texture_2(orm));
            return t2d;
        }

        texture2d to_texture(std::vector<char> const& data)
        {            
            auto tex = make_texture2d();
            tex.bind();
            tex.set_image(data);
            tex.mipmap();
            tex.set_wrap_s(GL_REPEAT);
            tex.set_wrap_t(GL_REPEAT);
            tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
            tex.set_mag_filter(GL_LINEAR);
            tex.unbind();
            return tex;
        }

        template<bool delay>
        texture2d to_texture_2(texture_data_t<delay>& data)
        {
            auto tex = make_texture2d();

            tex.bind();
            tex.set_image(data);
            tex.mipmap();
            tex.set_wrap_s(GL_REPEAT);
            tex.set_wrap_t(GL_REPEAT);
            tex.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
            tex.set_mag_filter(GL_LINEAR);
            tex.unbind();

            data.clear();

            return tex;
        }
    public:
        //std::vector<mesh_list> _vec_mesh_data;
        std::vector<mesh_list_2> _vec_mesh_data;
    private:
        std::vector < std::vector<texture2d> > _vec_mesh_tex;
        std::string _dir;
        std::string _name;
    };
}

namespace freeze
{
    using modeless = model_t<model_vertex, true>;
    using modeled = model_t<model_vertex, false>;
}

#endif