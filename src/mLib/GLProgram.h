
class GLProgram
{
public:
    void load(const string &vertexShaderFilename, const string &fragmentShaderFilename)
    {
        int isCompiledVS, isCompiledFS, isLinked;
        int maxLength;
        char *vertexInfoLog;
        char *fragmentInfoLog;
        char *shaderProgramInfoLog;

        vertexSource = util::getFileData(vertexShaderFilename);
        vertexSource.push_back(0);

        fragmentSource = util::getFileData(fragmentShaderFilename);
        fragmentSource.push_back(0);

        /* Create an empty vertex shader handle */
        vertexShader = glCreateShader(GL_VERTEX_SHADER);

        /* Send the vertex shader source code to GL */
        /* Note that the source code is NULL character terminated. */
        /* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
        GLchar *vertexSourceData = (GLchar*)vertexSource.data();
        glShaderSource(vertexShader, 1, (const GLchar**)&vertexSourceData, 0);

        glCompileShader(vertexShader);

        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiledVS);
        if (!isCompiledVS)
        {
            glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

            /* The maxLength includes the NULL character */
            vertexInfoLog = (char *)malloc(maxLength);

            glGetShaderInfoLog(vertexShader, maxLength, &maxLength, vertexInfoLog);

            cout << "failed to compile vertex shader:" << endl << vertexInfoLog << endl;

            MLIB_ERROR("failed to compile vertex shader");
        }

        /* Create an empty fragment shader handle */
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        /* Send the fragment shader source code to GL */
        /* Note that the source code is NULL character terminated. */
        /* GL will automatically detect that therefore the length info can be 0 in this case (the last parameter) */
        GLchar *fragmentSourceData = (GLchar*)fragmentSource.data();
        glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSourceData, 0);

        /* Compile the fragment shader */
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiledFS);
        if (!isCompiledFS)
        {
            glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

            /* The maxLength includes the NULL character */
            fragmentInfoLog = (char *)malloc(maxLength);

            glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, fragmentInfoLog);

            cout << "failed to compile fragment shader:" << endl << fragmentInfoLog << endl;

            MLIB_ERROR("failed to compile fragment shader");
        }

        /* If we reached this point it means the vertex and fragment shaders compiled and are syntax error free. */
        /* We must link them together to make a GL shader program */
        /* GL shader programs are monolithic. It is a single piece made of 1 vertex shader and 1 fragment shader. */
        /* Assign our program handle a "name" */
        shaderProgram = glCreateProgram();

        /* Attach our shaders to our program */
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);

        /* Bind attribute index 0 (coordinates) to in_Position and attribute index 1 (color) to in_Color */
        /* Attribute locations must be setup before calling glLinkProgram. */
        glBindAttribLocation(shaderProgram, 0, "in_Position");
        glBindAttribLocation(shaderProgram, 1, "in_Color");

        /* Link our program */
        /* At this stage, the vertex and fragment programs are inspected, optimized and a binary code is generated for the shader. */
        /* The binary code is uploaded to the GPU, if there is no error. */
        glLinkProgram(shaderProgram);

        /* Again, we must check and make sure that it linked. If it fails, it would mean either there is a mismatch between the vertex */
        /* and fragment shaders. It might be that you have surpassed your GPU's abilities. Perhaps too many ALU operations or */
        /* too many texel fetch instructions or too many interpolators or dynamic loops. */

        glGetProgramiv(shaderProgram, GL_LINK_STATUS, (int *)&isLinked);
        if (!isLinked)
        {
            /* Noticed that glGetProgramiv is used to get the length for a shader program, not glGetShaderiv. */
            glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

            /* The maxLength includes the NULL character */
            shaderProgramInfoLog = (char *)malloc(maxLength);

            /* Notice that glGetProgramInfoLog, not glGetShaderInfoLog. */
            glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, shaderProgramInfoLog);

            cout << "failed to link program:" << endl << shaderProgramInfoLog << endl;

            MLIB_ERROR("failed to link program");
        }

        bind();
        transformLocation = glGetUniformLocation(shaderProgram, "transform");
        colorLocation = glGetUniformLocation(shaderProgram, "color");
    }

    void bind()
    {
        glUseProgram(shaderProgram);

        GLint samplerLocation = glGetUniformLocation(shaderProgram, "sampler");
        glProgramUniform1i(shaderProgram, samplerLocation, 0);
        glActiveTexture(GL_TEXTURE0);
    }

    void setTransform(const mat4f &transform)
    {
        glUniformMatrix4fv(transformLocation, 1, true, transform.matrix);
    }

    void setColor(const vec4f &color)
    {
        glUniform4f(colorLocation, color.x, color.y, color.z, color.w);
    }

private:

    //
    // shader source code
    //
    vector<BYTE> vertexSource, fragmentSource;

    // OpenGL shader handles
    GLuint vertexShader, fragmentShader;

    // OpenGL program handle
    GLuint shaderProgram;

    GLint transformLocation;
    GLint colorLocation;

    /*
    glUseProgram(0);
    glDetachShader(shaderprogram, vertexshader);
    glDetachShader(shaderprogram, fragmentshader);
    glDeleteProgram(shaderprogram);
    glDeleteShader(vertexshader);
    glDeleteShader(fragmentshader);

    */

};

