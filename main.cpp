#define BUFFER_OFFSET(offset) ((void *)(offset))
#define GLM_SWIZZLE
#include "shader.h"
#include <GLFW/glfw3.h>
#include <tiffio.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>
#include 

using namespace std;
//using namespace vmath;

const GLuint WIDTH = 1024, HEIGHT = 1024;

GLFWwindow* window;
GLuint base_prog;

GLuint VBO, VAO, CBO, TBO;

float aspect;

GLuint vao;

GLuint quad_vbo;

GLuint tex;

Shader shader;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void read_image_to_block(string path, GLuint **raster, GLuint& width, GLuint& height, GLuint& depth) {

    TIFFSetWarningHandler(0);
    TIFF *tif = TIFFOpen(path.c_str(), "r");

    if (tif) {
        size_t npixels;

        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
        depth = TIFFNumberOfDirectories(tif);
        npixels = width * height * depth;
        *raster = new GLuint[npixels];
        GLuint *slice = new GLuint[width*height];
        if (*raster != NULL) {
            for(GLuint i=0; i<depth;++i){
                TIFFReadRGBAImageOriented(tif, width, height, slice, ORIENTATION_TOPLEFT);
                memcpy((*raster)+(i*width*height), slice, width*height*sizeof(GLuint)); //这行坑惨我了,第三个参数是拷贝多少各byte到目标数组
//                for(GLuint j=0;j<width*height;++j){
//                    (*raster)[i*width*height+j] = slice[j];
//                }
                TIFFReadDirectory(tif); //这个读下一张
            }
        }

        delete[] slice;
        TIFFClose(tif);
    }
}

void init()
{
    base_prog = glCreateProgram();

    glViewport(0, 0, WIDTH, HEIGHT);
    GLuint *raster = NULL;
    GLuint width=512, height = 512 , depth = 512;//height的值是不确定的
    read_image_to_block("/d/data/for_wzy.tif", &raster, width, height, depth);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_3D, tex);
//    glTexStorage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    // Set texture filtering
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);

    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, 0, GL_RGBA, GL_UNSIGNED_BYTE, raster);

    glBindTexture(GL_TEXTURE_3D, tex);
//    glGenerateMipmap(GL_TEXTURE_3D);

//    GLuint tex;
//    glGenSamplers(1, &tex);
//    glBindSampler(1, tex);

//    glUniform1i(glGetUniformLocation(shader.Program, "tex"), tex);

    delete[] raster;
    //_TIFFfree(raster); //书上说在glTexImage2D之后已经有数据了，所以不需要图像了

    shader =Shader(base_prog);
//    shader.init("../../vertex.shader", "../../fragment.shader");
    shader.attachShaderSource(GL_VERTEX_SHADER, "shader/vertex.shader");
    shader.attachShaderSource(GL_FRAGMENT_SHADER, "shader/fragment.shader");
    shader.link();

    static const GLfloat g_vertex_buffer_data[] ={
            0.5f, 0.5f,
            0.5f, -0.5f,
            -0.5f, -0.5f,
            -0.5f, 0.5f,
//            -0.5f, -0.5f,
//            0.5f, 0.5f
    };

    GLfloat colors[] = {
            -0.5f,-0.5f,-0.5f, //triangle 1 : begin
            -0.5f,-0.5f,0.5f,
            -0.5f,0.5f,0.5f,  //triangle 1 : end
            0.5f, 0.5f,-0.5f, // triangle 2 : begin
    };

    GLfloat texture_position[] = {

            1.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &CBO);
    glGenBuffers(1, &TBO);


    glBindVertexArray(VAO);

    // Position attribute
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
//    glBindAttribLocation(base_prog, 0, &VBO);
    glEnableVertexAttribArray(0);

    // Color attribute
    glBindBuffer(GL_ARRAY_BUFFER, CBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);

    // TexCoord attribute
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texture_position), texture_position, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(2);
}
static float a = 0.01f;

void display()
{
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable( GL_BLEND );
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnable( GL_ALPHA_TEST );
    glAlphaFunc( GL_GREATER, 0.03f );

    glBindTexture(GL_TEXTURE_3D, tex);

    // Activate shader
    shader.Use();

    // Draw container
    glBindVertexArray(VAO);

//    static const unsigned int start_time = GetTickCount();
    GLfloat t = (GLfloat)glfwGetTime();
//    cout<<t<<endl;
//    static const vmath::vec3 X(1.0f, 0.0f, 0.0f);
//    static const vmath::vec3 Y(0.0f, 1.0f, 0.0f);
//    static const vmath::vec3 Z(0.0f, 0.0f, 1.0f);
//
//    mat4 tc_matrix(mat4::identity());
//
//    tc_matrix = rotate(t * 170.0f, X) * rotate(t * 137.0f, Y) * rotate(t * 93.0f, Z);
//
//    GLint tc_rotate_loc = glGetUniformLocation(base_prog, "tc_rotate");
//    glUniformMatrix4fv(tc_rotate_loc, 1, GL_FALSE, tc_matrix);

    using namespace glm;

    vec3 x(1.0f, 0.0f, 0.0f);
    vec3 y(0.0f, 1.0f, 0.0f);
    vec3 z(0.0f, 0.0f, 1.0f);

    mat4 tc_matrix(1.0f);

    tc_matrix = rotate(tc_matrix, a, x);
    a += 0.01f;

    mat4 project = ortho(-1.0f,1.0f,-1.0f,1.0f,-1.0f,1.0f);

    tc_matrix *= project;

    GLint tc_rotate_loc = glGetUniformLocation(base_prog, "tc_rotate");
    glUniformMatrix4fv(tc_rotate_loc, 1, GL_FALSE, value_ptr(tc_matrix));

    mat4 otho_mat = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -2.0f, 2.0f);
    GLint otho_loc = glGetUniformLocation(base_prog, "otho_mat");
    glUniformMatrix4fv( otho_loc, 1, GL_FALSE, value_ptr(otho_mat));

    GLfloat z_position= -0.5f-1.0f/1024;
    for(int i=0;i<512;++i){
        z_position += 1.0f/512;
        glUniform1f(glGetUniformLocation(shader.Program, "z_position"), z_position);
//        cout<<z_position<<endl;
        glDrawArrays(GL_TRIANGLE_FAN, 0, 2*2);
    }
}


int main(void)
{

    int a[10] = {0};
    int b[2] = {1,2};
    memcpy(a+5, b, 8);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); //是否可以调整窗口大小
    glfwSetErrorCallback(error_callback);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Simple example", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;

    //glew一定要在调用任何opengl方法之前初始化，否则会报segment fault
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    const GLubyte  *renderer = glGetString( GL_RENDERER );
    const GLubyte *vendor = glGetString( GL_VENDOR );
    const GLubyte *version = glGetString( GL_VERSION );
    const GLubyte *glslVersion = glGetString( GL_SHADING_LANGUAGE_VERSION );
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("GL Vendor: %s\n", vendor);
    printf("GL Renderer : %s\n", renderer);
    printf("GL Version (string) : %s\n", version);
    printf("GL Version (integer) : %d.%d\n", major, minor);
    printf("GLSL Version : %s\n", glslVersion);

    init();
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        display();
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
