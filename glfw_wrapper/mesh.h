//
// Created by freezestudio on 2017/10/12.
//

#ifndef FREEGL_MESH_H
#define FREEGL_MESH_H

namespace freeze 
{
    template<typename T>
    inline std::string to_string(T value) 
    {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
}

namespace freeze
{

    struct mesh_vertex 
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    struct delay_vertex 
    {
        float x, y, z;   //Position
        float nx, ny, nz;//Normals;
        float u, v;      //TexCoords;
        float tx, ty, tz;//Tangent 切线
        float bx, by, bz;//Bitangent 副切线
    };

    enum class texture_type 
    {
        none = aiTextureType_NONE,
        diffuse = aiTextureType_DIFFUSE,
        specular = aiTextureType_SPECULAR,
        ambient = aiTextureType_AMBIENT,
        emissive = aiTextureType_EMISSIVE,
        height = aiTextureType_HEIGHT,
        normals = aiTextureType_NORMALS,
        shininess = aiTextureType_SHININESS,
        opacity = aiTextureType_OPACITY,
        displacement = aiTextureType_DISPLACEMENT,
        lightmap = aiTextureType_LIGHTMAP,
        reflection = aiTextureType_REFLECTION,
        unknown = aiTextureType_UNKNOWN,
    };

    struct texture_desc 
    {
        texture2d instance;
        std::string file;
        texture_type type;
    };

    struct texture_data 
    {
        int x;
        int y;
        GLenum format;
        std::vector<char> data;
    };

    //延迟装载
    struct texture_desc_delay 
    {
        texture_data instance_data;
        texture2d instance;
        std::string file;
        texture_type type;
    };

    struct texture_desc_delay2 
    {
        texture2d instance;
        std::string file;
        texture_type type;
        std::vector<char> instance_data;
    };

    struct texture_desc_delay3 
    {
        texture2d instance;
        std::string file;
        texture_type type;
    };

    template<typename Type, typename Name>
    struct texture_map 
    {
        texture_map() 
        {
            map_type_name.insert(std::make_pair(Type::none,         "none"));
            map_type_name.insert(std::make_pair(Type::diffuse,      "diffuse"));
            map_type_name.insert(std::make_pair(Type::specular,     "specular"));
            map_type_name.insert(std::make_pair(Type::ambient,      "ambient"));
            map_type_name.insert(std::make_pair(Type::emissive,     "emissive"));
            map_type_name.insert(std::make_pair(Type::height,       "height"));
            map_type_name.insert(std::make_pair(Type::normals,      "normals"));
            map_type_name.insert(std::make_pair(Type::shininess,    "shininess"));
            map_type_name.insert(std::make_pair(Type::opacity,      "opacity"));
            map_type_name.insert(std::make_pair(Type::displacement, "displacement"));
            map_type_name.insert(std::make_pair(Type::lightmap,     "lightmap"));
            map_type_name.insert(std::make_pair(Type::reflection,   "reflection"));
            map_type_name.insert(std::make_pair(Type::unknown,      "unknown"));
        }

        ~texture_map()
        {
            if (!map_type_name.empty())
            {
                map_type_name.clear();
            }            
        }

        Name get(Type type) const 
        {
            auto iter = map_type_name.find(type);
            if (iter != map_type_name.end()) 
            {
                return iter->second;
            } 
            else
            {
                return Name{};
            }
        }

        Name get(Type type, int index) const
        {
            auto name = get(type);
            if (!name.empty()) 
            {
                name += to_string(index);
            }
            return name;
        }

        Name get(std::string const& struct_name,Type type) const
        {
            auto name=struct_name +".";
            auto type_name = get(type);
            if(!name.empty())
            {
                name += type_name;
            }
            return name;
        }

    private:
        std::map<Type, Name> map_type_name;
    };

    //c++17 inline variables
    //inline texture_map<texture_type, std::string> g_tmap;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#if defined(_MSC_VER)
    //__declspec(selectany) texture_map<texture_type, std::string> g_tmap;
#endif
#endif

    template<typename T,typename U>
    texture_map<T, U> g_tmap = {};
}

namespace freeze 
{
    template<typename Vertex, typename TextureDesc>
    struct spec_mesh 
    {
        spec_mesh(std::vector<Vertex> const &vertices,
                  std::vector<GLuint> const &indices,
                  std::vector<TextureDesc> const &texture_descs,
                  bool delay = true) 
            : mDelay{delay} 
        {
            mVertices = vertices;
            mIndices = indices;
            mTextureDescs = texture_descs;

            if (!delay) 
            {
                setup();
            }
        }

        //延迟载入
        bool is_delay() const 
        {
            return mDelay;
        }

        void draw(program const &shader) {

            if(material_name.empty())
            {
                set_textures(shader);
            }
            else
            {
                set_material_textures(shader);
            }

            mVao.bind();
            glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, 0);
            mVao.unbind();

            glActiveTexture(GL_TEXTURE0);
        }

        void setup() 
        {
            load_texture();
            load_vertices();
        }

        void set_material(std::string const& name)
        {
            material_name = name;
        }

    private:
        void load_texture() 
        {
            for (auto &tex : mTextureDescs) 
            {
                tex.instance = make_texture2d();
                if (!tex.instance) 
                {
                    LOGE("create texture 2d failed! name:<%s>",tex.file.c_str());
                    continue;
                }
                tex.instance.bind();
                tex.instance.set_image(tex.instance_data);
                tex.instance.mipmap();
                tex.instance.set_wrap_s(GL_REPEAT);
                tex.instance.set_wrap_t(GL_REPEAT);
                tex.instance.set_min_filter(GL_NEAREST_MIPMAP_LINEAR);
                tex.instance.set_mag_filter(GL_LINEAR);
                tex.instance.unbind();
            }
        }

        void load_vertices()
        {

            mVao = make_vertex_array_buffer();
            mVao.bind();

            /*auto*/ mVbo = make_vertex_buffer();
            mVbo.bind();
            mVbo.copy_data(&mVertices[0], mVertices.size() * sizeof(Vertex));

            /*auto*/ mEbo = make_element_buffer();
            mEbo.bind();
            mEbo.copy_data(&mIndices[0], mIndices.size() * sizeof(GLuint));

            auto this_vertex = make_vertex();
            this_vertex.set(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Position)/*0*/);
            this_vertex.set(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Normal));
            this_vertex.set(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, TexCoords));
            this_vertex.set(3, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Tangent));
            this_vertex.set(4, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, Bitangent));

            //解绑ebo将不会显示
            //ebo.unbind();
            mVbo.unbind();

            mVao.unbind();
        }

        void set_textures(program const& shader)
        {
            auto gtmap = g_tmap<texture_type, std::string>;

            auto none = 1;
            auto diffuse = 1;
            auto specular = 1;
            auto ambient = 1;
            auto emissive = 1;
            auto height = 1;
            auto normals = 1;
            auto shininess = 1;
            auto opacity = 1;
            auto displacement = 1;
            auto lightmap = 1;
            auto reflection = 1;
            auto unknown = 1;

            auto index = 0;
            for (auto &texture_desc : mTextureDescs) 
            {
                glActiveTexture(GL_TEXTURE0 + index);
                std::string name;
                switch (texture_desc.type) 
                {
                    case texture_type::none         :
                        //name = tmap.get(texture_desc.type, none++);
                        break;
                    case texture_type::diffuse      :
                        name = gtmap.get(texture_desc.type, diffuse++);
                        break;
                    case texture_type::specular     :
                        name = gtmap.get(texture_desc.type, specular++);
                        break;
                    case texture_type::ambient      :
                        name = gtmap.get(texture_desc.type, ambient++);
                        break;
                    case texture_type::emissive     :
                        name = gtmap.get(texture_desc.type, emissive++);
                        break;
                    case texture_type::height       :
                        name = gtmap.get(texture_desc.type, height++);
                        break;
                    case texture_type::normals      :
                        name = gtmap.get(texture_desc.type, normals++);
                        break;
                    case texture_type::shininess     :
                        //name = tmap.get(texture_desc.type, shininess++);
                        break;
                    case texture_type::opacity      :
                        //name = tmap.get(texture_desc.type, opacity++);
                        break;
                    case texture_type::displacement :
                        //name = tmap.get(texture_desc.type, displacement++);
                        break;
                    case texture_type::lightmap     :
                        name = gtmap.get(texture_desc.type, lightmap++);
                        break;
                    case texture_type::reflection   :
                        name = gtmap.get(texture_desc.type, reflection++);
                        break;
                    case texture_type::unknown      :
                        //name = tmap.get(texture_desc.type, unknown++);
                        break;
                    default:
                        break;
                }

                shader.set_int(name, index);
                texture_desc.instance.bind();

                ++index;
            }
        }

        void set_material_textures(program const& shader)
        {
            auto gtmap = g_tmap<texture_type, std::string>;
            int index = 0;
            for (auto &texture_desc : mTextureDescs)
            {
                glActiveTexture(GL_TEXTURE0+index);

                auto name = gtmap.get(material_name,texture_desc.type);
                if(texture_desc.type == texture_type::shininess)
                {
                    shader.set_float(name,32.0f);
                }
                else
                {
                    shader.set_int(name,index);
                    texture_desc.instance.bind();
                }

                ++index;
            }
        }

    private:
        std::vector<Vertex> mVertices;
        std::vector<GLuint> mIndices;
        std::vector<TextureDesc> mTextureDescs;
        vertex_array_buffer mVao;
        vertex_buffer mVbo;
        element_buffer mEbo;
        bool mDelay; //是否为延迟的

        std::string material_name;
    };
}

namespace freeze 
{
    using mesh = spec_mesh<mesh_vertex, texture_desc>;
    using delay_mesh = spec_mesh<mesh_vertex, texture_desc_delay2>;
//    using delay_mesh = spec_mesh<mesh_vertex, texture_desc_delay3>;
    constexpr auto make_mesh = make<mesh>;
    constexpr auto make_mesh_delay = make<delay_mesh>;
}

namespace freeze
{
    template<typename MeshType, typename VertexType, typename TextureDesc>
    struct base_model 
    {
        base_model() 
        {
        }

        base_model(std::string const &path) 
        {
            load(path);
        }

        base_model(std::string const &path, std::string const &file) 
        {
            load(path, file);
        }

        void draw(program const &pid) 
        {
            for (auto &each_mesh : mMeshs) 
            {
                each_mesh.draw(pid);
            }
        }

        void set_material(std::string const& name)
        {
            material_name = name;
        }

        void load(std::string const &full_path_file)
        {
            auto pos = full_path_file.find_last_of("\\/");
            mDirectory = full_path_file.substr(0, pos);

            auto importer = make<Assimp::Importer>();
            //aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
            //aiProcessPreset_TargetRealtime_Fast
            //aiProcessPreset_TargetRealtime_Quality
            //aiProcessPreset_TargetRealtime_MaxQuality
            mScene = const_cast<aiScene *>(
                    importer.ReadFile(full_path_file, 
                        aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs));

            if (!mScene || mScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !mScene->mRootNode)
                return;

            process_node(mScene->mRootNode);
        }

        void load(std::string const &path, std::string const &file)
        {
            load(path + file);
        }

        void setup() 
        {
            for (auto& sub_mesh : mMeshs)
            {
                if (sub_mesh.is_delay()) 
                {
                    sub_mesh.setup();
                }

                if(!material_name.empty())
                {
                    sub_mesh.set_material(material_name);
                }
            }
        }

    private:
        void process_node(aiNode const *node)
        {
            for (auto i = 0U; i < node->mNumMeshes; ++i) 
            {
                auto sub_mesh = mScene->mMeshes[node->mMeshes[i]];
                //mMeshs.push_back(process_mesh(sub_mesh));
                mMeshs.push_back(process_mesh_delay(sub_mesh));
            }

            for (auto i = 0U; i < node->mNumChildren; ++i)
            {
                process_node(node->mChildren[i]);
            }
        }

        ////////////////////////////////////////////////////////////////////////////////////////////
        //延迟处理

        MeshType process_mesh_delay(aiMesh const *sub_mesh)
        {
            //顶点
            std::vector<VertexType> vertices;
            for (auto i = 0U; i < sub_mesh->mNumVertices; ++i)
            {
                VertexType vertex = {};
                vertex.Position = glm::vec3{sub_mesh->mVertices[i].x, sub_mesh->mVertices[i].y,
                                            sub_mesh->mVertices[i].z};
                vertex.Normal = glm::vec3{sub_mesh->mNormals[i].x, sub_mesh->mNormals[i].y,
                                          sub_mesh->mNormals[i].z};

                if (sub_mesh->mTextureCoords[0]) 
                {
                    vertex.TexCoords = glm::vec2{sub_mesh->mTextureCoords[0][i].x,
                                                 sub_mesh->mTextureCoords[0][i].y};
                }
                else
                {
                    vertex.TexCoords = glm::vec2{0.0f, 0.0f};
                }

                //切线与副切线，用于法线贴图
                vertex.Tangent = glm::vec3{sub_mesh->mTangents[i].x, sub_mesh->mTangents[i].y,
                                           sub_mesh->mTangents[i].z};
                vertex.Bitangent = glm::vec3{sub_mesh->mBitangents[i].x, sub_mesh->mBitangents[i].y,
                                             sub_mesh->mBitangents[i].z};

                vertices.push_back(vertex);
            }

            //索引
            std::vector<GLuint> indices;
            for (auto i = 0U; i < sub_mesh->mNumFaces; ++i) 
            {
                auto face = sub_mesh->mFaces[i];
                for (auto x = 0U; x < face.mNumIndices; ++x)
                {
                    indices.push_back(face.mIndices[x]);
                }
            }

            //材质
            aiMaterial *material = mScene->mMaterials[sub_mesh->mMaterialIndex];

            //环境贴图
            auto ambients = load_textures_delay(material, texture_type::ambient);
            //漫射贴图
            auto diffuses = load_textures_delay(material, texture_type::diffuse);
            //高光贴图
            auto speculars = load_textures_delay(material, texture_type::specular);
            //高度贴图
            auto heights = load_textures_delay(material, texture_type::height);
            //放射光贴图
            auto emissives = load_textures_delay(material,texture_type::emissive);
            //法线贴图
            auto normals = load_textures_delay(material,texture_type::normals);

            //纹理
            std::vector<TextureDesc> textures;
            if(ambients.size()>0) textures.insert(textures.end(), ambients.begin(), ambients.end());
            if(diffuses.size()>0) textures.insert(textures.end(), diffuses.begin(), diffuses.end());
            if(speculars.size()>0)textures.insert(textures.end(), speculars.begin(), speculars.end());
            if(heights.size()>0)  textures.insert(textures.end(), heights.begin(), heights.end());
            if(emissives.size()>0)textures.insert(textures.end(), emissives.begin(), emissives.end());
            if(normals.size()>0)  textures.insert(textures.end(), normals.begin(), normals.end());

            //返回网格对象
            return make<MeshType>(vertices, indices, textures);
        }

        std::vector<TextureDesc>
        load_textures_delay(aiMaterial const *material, texture_type type)
        {
            std::vector<TextureDesc> textures;
            auto aiType = static_cast<aiTextureType>(type);
            for (auto i = 0U; i < material->GetTextureCount(aiType); ++i)
            {
                aiString str;
                material->GetTexture(aiType, i, &str);
                std::string file = str.C_Str();

                auto skip = false;
                for (auto x = 0U; x < mCacheTextures.size(); ++x) 
                {
                    if (mCacheTextures[x].file == file) 
                    {
                        TextureDesc texture = {};
                        texture.type = type;
                        texture.file = file;
                        //TODO:消除冗余数据
                        texture.instance_data = mCacheTextures[x].instance_data;
                        textures.push_back(texture);
                        skip = true;
                        break;
                    }
                }

                if (!skip) 
                {
                    TextureDesc texture = { };
                    //texture_desc_delay
                    //texture.instance_data = texture_data_from_file(file);
                    //texture.type = type;
                    //texture.file = file;

                    //texture_desc_delay2
                    texture.type = type;
                    texture.file = file;
                    texture.instance_data = texture_data_from_file(file);

                    //texture_desc_delay3
                    //texture.type = type;
                    //texture.file = mDirectory + "/" + file;

                    textures.push_back(texture);
                    mCacheTextures.push_back(texture);
                }
            } //end for

            return textures;
        }

        std::vector<char> texture_data_from_file(std::string const &path) 
        {
            std::string filename = mDirectory + "/" + path;

            std::vector<char> vec_buffer;
            std::ifstream ifs{filename};
            if (ifs.is_open()) 
            {
                ifs.seekg(0,std::ios_base::end);
                std::size_t length = static_cast<std::size_t>(ifs.tellg());
                if(length <= 0)
                {
                    LOGE("Error : texture_data_from_file(%s) length = %d",filename.c_str(),static_cast<int>(length));
                    return vec_buffer;
                }

                ifs.seekg(0,std::ios_base::beg);
                vec_buffer.resize(length);
                ifs.read(vec_buffer.data(),length);

                ifs.close();
            }
            return vec_buffer;
        }

        ////////////////////////////////////////////////////////////////////////////////////////////

        MeshType process_mesh(aiMesh const *sub_mesh)
        {
            //顶点
            std::vector<VertexType> vertices;
            for (auto i = 0; i < sub_mesh->mNumVertices; ++i) 
            {
                VertexType vertex = {};
                vertex.Position = glm::vec3{sub_mesh->mVertices[i].x, sub_mesh->mVertices[i].y,
                                            sub_mesh->mVertices[i].z};
                vertex.Normal = glm::vec3{sub_mesh->mNormals[i].x, sub_mesh->mNormals[i].y,
                                          sub_mesh->mNormals[i].z};

                if (sub_mesh->mTextureCoords[0]) 
                {
                    vertex.TexCoords = glm::vec2{sub_mesh->mTextureCoords[0][i].x,
                                                 sub_mesh->mTextureCoords[0][i].y};
                } 
                else
                {
                    vertex.TexCoords = glm::vec2{0.0f, 0.0f};
                }

                vertex.Tangent = glm::vec3{sub_mesh->mTangents[i].x, sub_mesh->mTangents[i].y,
                                           sub_mesh->mTangents[i].z};
                vertex.Bitangent = glm::vec3{sub_mesh->mBitangents[i].x, sub_mesh->mBitangents[i].y,
                                             sub_mesh->mBitangents[i].z};

                vertices.push_back(vertex);
            }

            //索引
            std::vector<GLuint> indices;
            for (auto i = 0; i < sub_mesh->mNumFaces; ++i)
            {
                auto face = sub_mesh->mFaces[i];
                for (auto x = 0; x < face.mNumIndices; ++x) 
                {
                    indices.push_back(face.mIndices[x]);
                }
            }

            //材质
            aiMaterial *material = mScene->mMaterials[sub_mesh->mMaterialIndex];

            auto diffuses = load_textures(material, texture_type::diffuse);
            auto speculars = load_textures(material, texture_type::specular);
            auto normals = load_textures(material, texture_type::ambient);
            auto heights = load_textures(material, texture_type::height);

            //纹理
            std::vector<TextureDesc> textures;
            textures.insert(textures.end(), diffuses.begin(), diffuses.end());
            textures.insert(textures.end(), speculars.begin(), speculars.end());
            textures.insert(textures.end(), normals.begin(), normals.end());
            textures.insert(textures.end(), heights.begin(), heights.end());

            //返回网格对象
            return make<MeshType>(vertices, indices, textures);
        }

        std::vector<TextureDesc> load_textures(aiMaterial const *material, texture_type type)
        {
            std::vector<TextureDesc> textures;
            auto aiType = static_cast<aiTextureType>(type);
            for (auto i = 0; i < material->GetTextureCount(aiType); ++i) 
            {
                aiString str;
                material->GetTexture(aiType, i, &str);
                std::string file = str.C_Str();

                auto skip = false;
                for (auto x = 0; x < mCacheTextures.size(); ++x)
                {
                    if (mCacheTextures[x].file == file)
                    {
                        TextureDesc texture = {};
                        texture.instance = mCacheTextures[x].instance;
                        texture.type = type;
                        texture.file = file;
                        textures.push_back(texture);
                        skip = true;
                        break;
                    }
                }

                if (!skip)
                {
                    TextureDesc texture = {};
                    texture.instance = texture_from_file(file);
                    texture.type = type;
                    texture.file = file;

                    textures.push_back(texture);
                    mCacheTextures.push_back(texture);
                }
            } //end for

            return textures;
        }

        auto texture_from_file(std::string const &path) 
        {
            std::string filename = mDirectory + "/" + path;
            return load_texture_from_sdcard<decltype(TextureDesc::instance)>(filename);
        }

    private:
        std::vector<TextureDesc> mCacheTextures;
        std::vector<MeshType> mMeshs;
        std::string mDirectory;
        aiScene *mScene;
        std::string material_name;
    };
}

////////////////////////////////////////////////////////////////////////////////////////////////////

namespace freeze 
{
    template<typename Vertex = delay_vertex, typename DelayTexture = texture_desc_delay2>
    struct mesh2 
    {
        void draw(program const &shader) {

        }

        //延迟加载gl
        void setup() 
        {
            setup_vertices();
            setup_textures();
        }

    private:
        void setup_vertices()
        {
            vao = make_vertex_array_buffer();
            vao.bind();

            auto vbo = make_vertex_buffer();
            vbo.bind();
            vbo.copy_data(vec_vertices.data(), vec_vertices.size() * sizeof(Vertex));

            auto ebo = make_element_buffer();
            ebo.bind();
            ebo.copy_data(vec_indices.data(), vec_indices.size() * sizeof(GLuint));

            auto this_vertex = make_vertex();
            this_vertex.set(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, x)/*0*/);
            this_vertex.set(1, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, nx));
            this_vertex.set(2, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, u));
            this_vertex.set(3, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, tx));
            this_vertex.set(4, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, bx));

            //解绑ebo将不会显示
            //ebo.unbind();
            vbo.unbind();

            vao.unbind();
        }

        void setup_textures() 
        {

            for (auto &tex : vec_textures) 
            {
                if (tex.instance_data.empty()) 
                {
                    continue;
                }
                tex.instance = make_texture2d();
                if (!tex.instance) 
                {
                    LOGE("delay mesh setup texture name:<%s> failed", tex.file.c_str());
                    continue;
                }
                tex.instance.bind();

                tex.instance.set_image(tex.instance_data);
                tex.instance_data.clear();

                tex.instance.mipmap();

                tex.instance.set_wrap_s(GL_REPEAT);
                tex.instance.set_wrap_t(GL_REPEAT);
                tex.instance.set_min_filter(GL_LINEAR_MIPMAP_LINEAR);
                tex.instance.set_mag_filter(GL_LINEAR);

            }
        }

    private:
        std::vector<Vertex> vec_vertices;
        std::vector<GLuint> vec_indices;
        std::vector<DelayTexture> vec_textures;
        vertex_array_buffer vao;
    };

    using delay_mesh2 = mesh2<>;
    constexpr auto make_delay_mesh2 = make<delay_mesh>;
}

namespace freeze 
{
    using model = base_model<mesh, mesh_vertex, texture_desc>;
    //using delay_model = base_model<delay_mesh, mesh_vertex, texture_desc_delay3>;
    using delay_model = base_model<delay_mesh, mesh_vertex, texture_desc_delay2>;
    constexpr auto make_model = make<model>;
    constexpr auto make_model_delay = make<delay_model>;
}


#endif //FREEGL_MESH_H
