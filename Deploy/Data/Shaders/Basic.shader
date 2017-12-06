#version 330

#if defined(VERTEX_SHADER)
    uniform mat4 vertexTransform;

    layout(location = 0) in vec3 vertexPosition;
    layout(location = 1) in vec2 vertexTexture;

    out vec2 fragmentTexture;

    void main()
    {
        gl_Position = vertexTransform * vec4(vertexPosition, 1.0f);
        fragmentTexture = vertexTexture;
    }
#endif

#if defined(FRAGMENT_SHADER)
    uniform sampler2D textureDiffuse;

    in vec2 fragmentTexture;

    out vec4 finalColor;

    void main()
    {
        finalColor = texture2D(textureDiffuse, fragmentTexture);
    }
#endif
