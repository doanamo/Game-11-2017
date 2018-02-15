#include "Precompiled.hpp"
#include "Graphics/Shader.hpp"
using namespace Graphics;

namespace
{
    // Constant definitions.
    const GLuint InvalidHandle = 0;
    const GLint  InvalidAttribute = -1;
    const GLint  InvalidUniform = -1;

    // Available shader types.
    struct ShaderType
    {
        const char* name;
        GLenum type;
    };

    const int ShaderTypeCount = 3;
    const ShaderType ShaderTypes[ShaderTypeCount] =
    {
        { "VERTEX_SHADER",   GL_VERTEX_SHADER   },
        { "GEOMETRY_SHADER", GL_GEOMETRY_SHADER },
        { "FRAGMENT_SHADER", GL_FRAGMENT_SHADER },
    };
}

Shader::Shader() :
    m_handle(InvalidHandle)
{
}

Shader::~Shader()
{
    this->DestroyHandle();
}

void Shader::DestroyHandle()
{
    // Release the program handle.
    if(m_handle != InvalidHandle)
    {
        glDeleteProgram(m_handle);
        m_handle = InvalidHandle;
    }
}

bool Shader::Load(std::string filepath)
{
    Log() << "Loading shader from \"" << filepath << "\" file..." << LogIndent();

    // Load the shader code from a file.
    std::string shaderCode = Utility::GetTextFileContent(Build::GetWorkingDir() + filepath);

    if(shaderCode.empty())
    {
        LogError() << "Could not read the file!";
        return false;
    }

    // Call the compile method.
    if(!this->Compile(shaderCode))
    {
        LogError() << "Could not compile the shader code!";
        return false;
    }

    // Success!
    LogInfo() << "Success!";

    return true;
}

bool Shader::Compile(std::string shaderCode)
{
    Log() << "Compiling shader code..." << LogIndent();

    // Check if handle has been already created.
    Verify(m_handle == InvalidHandle, "Shader instance has been already initialized!");

    // Validate arguments.
    if(shaderCode.empty())
    {
        LogError() << "Shader code cannot be empty!";
        return false;
    }

    // Setup a cleanup guard.
    bool initialized = false;

    // Create an array of shader objects for each type that can be linked.
    GLuint shaderObjects[ShaderTypeCount] = { 0 };

    SCOPE_GUARD_BEGIN();
    {
        for(int i = 0; i < ShaderTypeCount; ++i)
        {
            glDeleteShader(shaderObjects[i]);
        }
    }
    SCOPE_GUARD_END();

    // Extract shader version.
    std::string shaderVersion;

    std::size_t versionStart = shaderCode.find("#version ");
    std::size_t versionEnd = shaderCode.find('\n', versionStart);

    if(versionStart != std::string::npos)
    {
        shaderVersion = shaderCode.substr(versionStart, versionEnd - versionStart + 1);
        shaderCode.erase(versionStart, versionEnd + 1);
    }

    // Compile shader objects.
    bool shaderObjectsFound = false;

    for(unsigned int i = 0; i < ShaderTypeCount; ++i)
    {
        const ShaderType& shaderType = ShaderTypes[i];
        GLuint& shaderObject = shaderObjects[i];

        // Compile shader object if found.
        if(shaderCode.find(shaderType.name) != std::string::npos)
        {
            shaderObjectsFound = true;

            // Create a shader object.
            shaderObject = glCreateShader(shaderType.type);

            if(shaderObject == InvalidHandle)
            {
                LogError() << "Could not create a shader object!";
                return false;
            }

            // Prepare preprocessor define.
            std::string shaderDefine = "#define ";
            shaderDefine += shaderType.name;
            shaderDefine += "\n";

            // Compile shader object code.
            const char* shaderCodeSegments[] =
            {
                shaderVersion.c_str(),
                shaderDefine.c_str(),
                shaderCode.c_str(),
            };

            glShaderSource(shaderObject, Utility::ArraySize(shaderCodeSegments), (const GLchar**)&shaderCodeSegments, nullptr);
            glCompileShader(shaderObject);

            // Check compiling results.
            GLint compileStatus = 0;
            glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &compileStatus);

            if(compileStatus == GL_FALSE)
            {
                LogError() << "Could not compile a shader object!";

                GLint errorLength = 0;
                glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &errorLength);

                if(errorLength != 0)
                {
                    std::vector<char> errorText(errorLength);
                    glGetShaderInfoLog(shaderObject, errorLength, &errorLength, &errorText[0]);

                    LogError() << "Shader compile errors: \"" << errorText.data() << "\"";
                }

                return false;
            }
        }
    }

    // Check if any shader objects were found.
    if(shaderObjectsFound == false)
    {
        LogError() << "Could not find any shader objects!";
        return false;
    }

    // Create a shader program.
    SCOPE_GUARD_IF(!initialized, this->DestroyHandle());

    m_handle = glCreateProgram();

    if(m_handle == InvalidHandle)
    {
        LogError() << "Could not create a shader program!";
        return false;
    }

    // Attach compiled shader objects.
    for(unsigned int i = 0; i < ShaderTypeCount; ++i)
    {
        GLuint& shaderObject = shaderObjects[i];

        if(shaderObject != InvalidHandle)
        {
            glAttachShader(m_handle, shaderObject);
        }
    }

    // Link attached shader objects.
    glLinkProgram(m_handle);

    // Detach linked shader objects.
    for(unsigned int i = 0; i < ShaderTypeCount; ++i)
    {
        GLuint& shaderObject = shaderObjects[i];

        if(shaderObject != InvalidHandle)
        {
            glDetachShader(m_handle, shaderObject);
        }
    }

    // Check linking results.
    GLint linkStatus = 0;
    glGetProgramiv(m_handle, GL_LINK_STATUS, &linkStatus);

    if(linkStatus == GL_FALSE)
    {
        LogError()  << "Could not link a shader program!";

        GLint errorLength = 0;
        glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &errorLength);

        if(errorLength != 0)
        {
            std::vector<char> errorText(errorLength);
            glGetProgramInfoLog(m_handle, errorLength, &errorLength, &errorText[0]);

            LogError() << "Shader link errors: \"" << errorText.data() << "\"";
        }

        return false;
    }

    // Success!
    LogInfo() << "Success!";

    return initialized = true;
}

GLint Shader::GetAttribute(std::string name) const
{
    Verify(m_handle != InvalidHandle, "Shader program handle has not been created!");
    Verify(!name.empty(), "Attribute name cannot be empty!");

    GLint location = glGetAttribLocation(m_handle, name.c_str());

    return location;
}

GLint Shader::GetUniform(std::string name) const
{
    Verify(m_handle != InvalidHandle, "Shader program handle has not been created!");
    Verify(!name.empty(), "Uniform name cannot be empty!");

    GLint location = glGetUniformLocation(m_handle, name.c_str());

    return location;
}

GLuint Shader::GetHandle() const
{
    Verify(m_handle != InvalidHandle, "Shader program handle has not been created!");

    return m_handle;
}

bool Shader::IsValid() const
{
    return m_handle != InvalidHandle;
}
