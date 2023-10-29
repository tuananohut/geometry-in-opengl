// Link statically with GLEW
#define GLEW_STATIC

// Headers
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Vertex shader
const GLchar* vertexShaderSrc = R"glsl(
    #version 150 core

    in vec2 pos;
    in vec3 color;
    in float sides;

    out vec3 vColor;
    out float vSides;

    void main()
    {
        gl_Position = vec4(pos, 0.0, 1.0);
        vColor = color;
        vSides = sides;
    }
)glsl";

// Fragment shader
const GLchar* fragmentShaderSrc = R"glsl(
    #version 150 core

    in vec3 fColor;

    out vec4 outColor;

    void main()
    {
        outColor = vec4(fColor, 1.0);
    }
)glsl";

const char* geometryShaderSrc = R"glsl(
    #version 150 core
    
    layout(points) in;
    layout(line_strip, max_vertices = 64) out;
    
    in vec3 vColor[];
    in float vSides[];    

    out vec3 fColor;    

    const float PI = 3.1415926;

    void main()
    {
        fColor = vColor[0];

        for (int i = 0; i <= vSides[0]; i++) 
        {
            // Angle between each side in radians
            float ang = PI * 2.0 / vSides[0] * i;

            // Offset from center of point (0.3 to accomodate for aspect ratio)
            vec4 offset = vec4(cos(ang) * 0.3, -sin(ang) * 0.4, 0.0, 0.0);
            gl_Position = gl_in[0].gl_Position + offset;

            EmitVertex();
        }

        EndPrimitive();
    }
)glsl";

// Shader creation helper
GLuint createShader(GLenum type, const GLchar* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    return shader;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        // fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "Circles", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        // fprintf(stderr, "Failed to initialize GLEW\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Set the required callback functions
    glfwSetKeyCallback(window, keyCallback);

    // Compile and activate shaders
    GLuint vertexShader = createShader(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fragmentShader = createShader(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    GLuint geometryShader = createShader(GL_GEOMETRY_SHADER, geometryShaderSrc);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glAttachShader(shaderProgram, geometryShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Create VBO with point coordinates
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat points[] = {
        -0.45f,  0.45f, 1.0f, 0.0f, 0.0f,  4.0f, 
         0.45f,  0.45f, 0.0f, 1.0f, 0.0f,  8.0f,
         0.45f, -0.45f, 0.0f, 0.0f, 1.0f, 16.0f,
        -0.45f, -0.45f, 1.0f, 1.0f, 0.0f, 32.0f,
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "pos");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
    glEnableVertexAttribArray(colAttrib);
    glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));

    GLint sidesAttrib = glGetAttribLocation(shaderProgram, "sides");
    glEnableVertexAttribArray(sidesAttrib);
    glVertexAttribPointer(sidesAttrib, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));

    while (!glfwWindowShouldClose(window)) {
        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render frame
        glDrawArrays(GL_POINTS, 0, 4);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
