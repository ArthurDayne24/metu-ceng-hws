#include "helper.h"
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/vec4.hpp"
#include "glm/vec3.hpp"

static GLFWwindow * win = nullptr;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;

int speed; // moving speed of the camera, initially zero

GLfloat* triangle_vertices;
// This will identify our vertex buffer
GLuint vertexbuffer;

static void errorCallback(int error, const char * description) {
    fprintf(stderr, "Error: %s\n", description);
}

/* Set Camera */
int centerx = 0, centery = 0, centerz = 1;
int upx = 0, upy = 0, upz = 1;

void setCamera() {
    glViewport(0, 0, widthTexture, heightTexture); // TODO ??

    glMatrixMode(GL_MODELVIEW);
    // Position, direction it is pointing, up vector
    glLoadIdentity();
    gluLookAt(widthTexture / 2, widthTexture / 10, -widthTexture / 4, centerx, centery, centerz, upx, upy, upz);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 0.1, 1000); // TODO radians or degrees ?
}

void drawObject() {
}

// Keyboard functions, they are called in keyboard() function which is the key listener
void increaseHeightFactor(){}
void decreaseHeightFactor(){}

void pitchUp(){}
void pitchDown(){}

void yawLeft(){}
void yawRight(){}

void increaseSpeed(){
    speed++;
}
void decreaseSpeed(){
    if (speed > 0) speed--;
}

void fullscreenToggle(){}

// Key event function which listens all key events
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mods){
    // height factor
    if (key == GLFW_KEY_O && action == GLFW_PRESS) increaseHeightFactor();
    else if (key == GLFW_KEY_L && action == GLFW_PRESS) decreaseHeightFactor();
    // pitch
    else if (key == GLFW_KEY_W && action == GLFW_PRESS) pitchUp();
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) pitchDown();
    // yaw
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) yawLeft();
    else if (key == GLFW_KEY_D && action == GLFW_PRESS) yawRight();
    // speed
    else if (key == GLFW_KEY_U && action == GLFW_PRESS) increaseSpeed();
    else if (key == GLFW_KEY_J && action == GLFW_PRESS) decreaseSpeed();
    // fullscreen
    else if (key == GLFW_KEY_F && action == GLFW_PRESS) fullscreenToggle();
}
 
// Window resize callback
void resize(GLFWwindow* window, int width, int height){

}

// XXX: Call this before windows is created and before any other OpenGL call
// function that initiates the vertex array in user domain
void initVertexArr(int width, int height) {
    GLuint VertexArrayId;
    glGenVertexArrays(1, &VertexArrayId);
    glBindVertexArray(VertexArrayId);

    // An array of 3 vectors which represents 3 vertices
    // TODO fill triangle_vertices
    // See http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/

    // Identify triangles to OpenGL
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
}

// Call this in while loop
void drawTriangles(int numberOfTriangles) {
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
            0,                     // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3 * numberOfTriangles, // size
            GL_FLOAT,              // type
            GL_FALSE,              // normalized?
            0,                     // stride
            (void*) 0              // array buffer offset
    );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, 3 * numberOfTriangles);
    glDisableVertexAttribArray(0);
}

int main(int argc, char * argv[]) {

    if (argc != 2) {
        printf("Only one texture image expected!\n");
        exit(-1);
    }

    glfwSetErrorCallback(errorCallback);

    if (!glfwInit()) {
        exit(-1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    win = glfwCreateWindow(600, 600, "CENG477 - HW3", nullptr, nullptr);

    if (!win) {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(win);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

        glfwTerminate();
        exit(-1);
    }

    initVertexArr(widthTexture, heightTexture);

    initShaders();
    glUseProgram(idProgramShader);
    initTexture(argv[1], & widthTexture, & heightTexture);

    glfwSetKeyCallback(win, keyboard); // register key callback
    glfwSetWindowSizeCallback(win, resize); // register resize callback

    while (!glfwWindowShouldClose(win)) {
        glfwSwapBuffers(win);
        glfwPollEvents();
        
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}
