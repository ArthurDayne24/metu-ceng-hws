#include "helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec3.hpp>
#include <vector>

static GLFWwindow *window = nullptr;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;

// TODO ?
int speed = 0; // moving speed of the camera, initially zero

/* Data and their buffers */
// Vertices
GLfloat *g_vertex_buffer_data; // old triangle vertices
GLuint vertexbuffer;

// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 *mvp;

/* Camera params */
glm::vec3 *camera_pos;
glm::vec3 *camera_gaze;
glm::vec3 *camera_up;

// Vertex array id
GLuint VertexArrayId;

static void errorCallback(int error, const char * description) {
    fprintf(stderr, "Error: %s with error no:%d\n", description, error);
}

// Call this in while loop, I guess
void setMatrices() {
    // Projection matrix
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 0.0f, 1.0f, 1000.0f);
    // Camera matrix
    glm::mat4 View = glm::lookAt(*camera_pos, *camera_gaze, *camera_up);
    // Model matrix : an identity matrix (model will be at the origin)
    glm::mat4 Model = glm::mat4(1.0f);
    // Our ModelViewProjection : multiplication of our 3 matrices
    *mvp = Projection * View * Model;
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

// TODO: hata olabilir bunlarda !
void increaseSpeed(){
    speed++;
}
void decreaseSpeed(){
    if (speed > 0) speed--;
}

void fullscreenToggle(){}

// Key event function which listens all key events
void keyboard(GLFWwindow *window, int key, int scancode, int action, int mods) {
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
void resize(GLFWwindow* window, int width, int height) {
    // TODO: implement
}

// XXX: Call this before windows is created and before any other OpenGL call
// function that initiates the vertex array in user domain
void initVertexArray(int width, int height) {
    glGenVertexArrays(1, &VertexArrayId);
    glBindVertexArray(VertexArrayId);
}

void fillVertexBuffersData(size_t size_g_vertex_buffer_data) {
    // An array of 3 vectors which represents 3 vertices

    g_vertex_buffer_data = (GLfloat*) malloc(size_g_vertex_buffer_data);

    // TODO: check loop
    GLfloat *p = g_vertex_buffer_data;
    for (int i = 0; i < heightTexture; i++) {
        for (int j = 0; j < widthTexture; j++) {
            // First triangle
            p[0] = i;
            p[1] = 0;
            p[2] = j;

            p[3] = i;
            p[4] = 0;
            p[5] = j+1;

            p[6] = i+1;
            p[7] = 0;
            p[8] = j;

            // Second triangle
            p[9] = i;
            p[10] = 0;
            p[11] = j+1;

            p[12] = i+1;
            p[13] = 0;
            p[14] = j+1;

            p[15] = i+1;
            p[16] = 0;
            p[17] = j;

            p += 18;
        }
    }

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, size_g_vertex_buffer_data, g_vertex_buffer_data, GL_STATIC_DRAW);
}

// Call this in while loop
void setBuffers(int numberOfTriangles) {
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // TODO not sure on these parameters
    glVertexAttribPointer(
            0,                     // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                     // size
            GL_FLOAT,              // type
            GL_FALSE,              // normalized?
            0,                     // stride
            (void *) 0             // array buffer offset
    );

    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, numberOfTriangles * 3);

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

    window = glfwCreateWindow(600, 600, "CENG477 - HW3", nullptr, nullptr);

    if (nullptr == window) {
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

        glfwTerminate();
        exit(-1);
    }

    // TODO not sure if we need ?
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background TODO not sure if it is desired
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Depth test TODO not sure if it is desired and where to put in code
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    initVertexArray(widthTexture, heightTexture);

    int numberOfTriangles = 2 * widthTexture * heightTexture;
    size_t size_g_vertex_buffer_data = sizeof(GLfloat) * numberOfTriangles * 3 * 3;

    initShaders();
    // Get a handle for our "MVP" uniform
    // Only during the initialisation
    idMVPMatrix = (GLuint) (glGetUniformLocation(idVertexShader,
                                                 "MVP")); // TODO check signed / unsigned
    initTexture(argv[1], &widthTexture, &heightTexture);

    camera_pos = new glm::vec3(widthTexture / 2, widthTexture / 10, -widthTexture / 4);
    camera_gaze = new glm::vec3(0, 0, 1);
    camera_up = new glm::vec3(0, 1, 0); // TODO don't know

    mvp = new glm::mat4();

    setMatrices();

    fillVertexBuffersData(size_g_vertex_buffer_data);

    glfwSetKeyCallback(window, keyboard); // register key callback
    glfwSetWindowSizeCallback(window, resize); // register resize callback

    while (!glfwWindowShouldClose(window)) {
        // Clear the screen TODO desired ?
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(idProgramShader);

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
        glUniformMatrix4fv(idMVPMatrix, 1, GL_FALSE, &(*mvp)[0][0]);

        setBuffers(numberOfTriangles);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup VBO and shader
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteProgram(idProgramShader);
    glDeleteVertexArrays(1, &VertexArrayId);

    // Close OpenGL window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
