//
// Created by freezestudio on 2017/10/11.
//

#ifndef FREEGL_SHADER_PROGRAM_H
#define FREEGL_SHADER_PROGRAM_H

namespace freeze
{

    inline std::string source_from_file(std::string const& file)
    {
        std::ifstream ifs{ file };
        if (!ifs.good())
        {
            ifs.clear();
            ifs.close();
            ifs.open(file); if (ifs.bad() || !ifs.is_open())
                return ""s;
        }

        std::stringstream ss;
        ss << ifs.rdbuf();
        auto source = ss.str();
        ifs.close();
        return source;
    }
}

namespace freeze
{
    template<typename = void>
    struct program_t
        : make_object<program_t<void>, false>
    {
        GLuint create()
        {
            return glCreateProgram();
            assert_error();
        }

        void destroy(GLuint program)
        {
            glDeleteProgram(program);
            assert_error();
        }

        template<GLenum Target, typename = void>
        struct shader_t
            : make_object<shader_t<Target, void>, false>
        {
            GLuint create()
            {
                auto id = glCreateShader(Target);
                assert_error();
                if (id == 0)
                {
                    //GL_NO_ERROR 0
                    //GL_INVALID_ENUM 0x0500
                    std::string st;
                    if (Target == GL_VERTEX_SHADER)st = "vertex";
                    else if (Target == GL_GEOMETRY_SHADER)st = "geometry";
                    else if (Target == GL_FRAGMENT_SHADER)st = "fragment";

                    auto error = glGetError();
                    LOGE("Create %s Shader Error -- %d", st.c_str(), error);
                }
                return id;
            }

            void destroy(GLuint shader)
            {
                glDeleteShader(shader);
                assert_error();
            }

            void source(std::string const& source)
            {
                auto code = source.c_str();
                glShaderSource(this->ref(), 1, &code, nullptr);
                assert_error();

                glCompileShader(this->ref());
                GLint success;
                glGetShaderiv(this->ref(), GL_COMPILE_STATUS, &success);
                if (!success)
                {
                    GLint length;
                    glGetShaderiv(this->ref(), GL_INFO_LOG_LENGTH, &length);
                    auto info = (GLchar *)malloc(length);
                    glGetShaderInfoLog(this->ref(), length, nullptr, info);
                    std::string st;
                    if (Target == GL_VERTEX_SHADER)st = "vertex";
                    else if (Target == GL_GEOMETRY_SHADER)st = "geometry";
                    else if (Target == GL_FRAGMENT_SHADER)st = "fragment";
                    LOGE("Error: Compile %s Shader -- %s", st.c_str(), info);
                    free(info);
                }
            }
        };

        template<GLenum Target>
        using shader = shader_t<Target>;

        constexpr static auto make_vertex_shader = make<shader<GL_VERTEX_SHADER>>;
        constexpr static auto make_fragment_shader = make<shader<GL_FRAGMENT_SHADER>>;
        constexpr static auto make_geometry_shader = make<shader<GL_GEOMETRY_SHADER>>;


        void compile_and_link(std::string const& vs, 
            std::string const& fs =""s, 
            std::string const& gs = ""s)
        {
            compile(vs, fs, gs);
            link();
        }

        void compile_and_link(std::vector<std::string> const& shader_sources)
        {
            compile(shader_sources);
            link();
        }

        void compile_file_and_link(std::vector<std::string> const& shader_files)
        {
            compile_file(shader_files);
            link();
        }

        void compile_file_and_link(std::string const& vs, 
            std::string const& fs =""s, 
            std::string const& gs = ""s)
        {
            compile_file(vs, fs, gs);
            link();
        }

        void compile_file(std::vector<std::string> const& shader_files)
        {
            if (shader_files.size() < 1)
            {
                LOGE("compile file must at least 1 files");
                return;
            }

            compile_file(shader_files[0], shader_files[1], shader_files[2]);
        }

        void compile_file(std::string const& vs, 
            std::string const& fs =""s, 
            std::string const& gs = ""s)
        {
            auto vssource = source_from_file(vs);
            std::string fssource;
            if (!fs.empty())
            {
                fssource = source_from_file(fs);
            }
            std::string gssource;
            if (!gs.empty())
            {
                gssource = source_from_file(gs);
            }

            compile(vssource, fssource, gssource);
        }

        void compile(std::vector<std::string> const& shader_sources)
        {
            if (shader_sources.size() < 2)
            {
                LOGE("compile must at least 2 sources");
                return;
            }
            compile(shader_sources[0], shader_sources[1], shader_sources[2]);
        }

        void compile(std::string const& vs, 
            std::string const& fs = ""s, 
            std::string const& gs = ""s)
        {
            if (vs.empty())return;

            auto vs_shader = make_vertex_shader();
            vs_shader.source(vs);
            glAttachShader(this->ref(), vs_shader.ref());
            
            if (!gs.empty()) 
            {
                auto gs_shader = make_geometry_shader();
                gs_shader.source(gs);
                glAttachShader(this->ref(), gs_shader.ref());
            }

            if (!fs.empty())
            {
                auto fs_shader = make_fragment_shader();
                fs_shader.source(fs);
                glAttachShader(this->ref(), fs_shader.ref());
            }
        }

        void link()
        {
            glLinkProgram(this->ref());

            GLint success = 0;
            glGetProgramiv(this->ref(), GL_LINK_STATUS, &success);
            if (!success)
            {
                GLint length;
                glGetProgramiv(this->ref(), GL_INFO_LOG_LENGTH, &length);
                auto info = (GLchar*)malloc(length);
                glGetProgramInfoLog(this->ref(), length, nullptr, info);
                LOGE("Error: Link Program -- %s", info);
                free(info);
            }
        }

        void use()
        {
            glUseProgram(this->ref());
        }

        void set_bool(std::string const& name, bool value) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform1i(loc, value ? GL_TRUE : GL_FALSE);
            assert_error();
        }

        void set_int(std::string const& name, GLint value) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform1i(loc, value);
            assert_error();
        }

        void set_float(const std::string &name, float value) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform1f(loc, value);
            assert_error();
        }

        void set_vec2(const std::string &name, const glm::vec2 &value) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform2fv(loc, 1, &value[0]);
            assert_error();
        }

        void set_vec2(const std::string &name, float x, float y) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform2f(loc, x, y);
            assert_error();
        }

        void set_vec3(const std::string &name, const glm::vec3 &value) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform3fv(loc, 1, &value[0]);
            assert_error();
        }

        void set_vec3(const std::string &name, float x, float y, float z) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform3f(loc, x, y, z);
            assert_error();
        }

        void set_vec4(const std::string &name, const glm::vec4 &value) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform4fv(loc, 1, &value[0]);
            assert_error();
        }

        void set_vec4(const std::string &name, float x, float y, float z, float w) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniform4f(loc, x, y, z, w);
            assert_error();
        }

        void set_mat2(const std::string &name, const glm::mat2 &mat) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniformMatrix2fv(loc, 1, GL_FALSE, &mat[0][0]);
            assert_error();
        }

        void set_mat3(const std::string &name, const glm::mat3 &mat) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
            assert_error();
        }

        void set_mat4(const std::string &name, const glm::mat4 &mat) const
        {
            auto loc = get_loc(name);
            if (loc < 0)return;
            glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
            assert_error();
        }

        void set_mat4(const std::string &name, const GLfloat* value) const
        {
            auto loc = get_loc(name);
            if (loc < 0) return;
            glUniformMatrix4fv(loc, 1, GL_FALSE, value);
            assert_error();
        }

		//bufferMode:
		//   GL_INTERLEAVED_ATTRIBS,GL_SEPARATE_ATTRIBS
		void set_varyings(GLchar const** varyings, GLsizei count, GLenum bufferMode = GL_INTERLEAVED_ATTRIBS)
		{
			glTransformFeedbackVaryings(this->ref(), count, varyings, bufferMode);
			assert_error();
		}

    private:
        auto get_loc(std::string const& name)  const
        {
            auto loc = glGetUniformLocation(this->ref(), name.c_str());
            assert_name_error(name.c_str());
            return loc;
        }

    };
}

namespace freeze
{
    using program = program_t<>;
    constexpr auto make_program = make<program>;
}

#endif //FREEGL_SHADER_PROGRAM_H
