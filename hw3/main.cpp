#include "helper.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

static GLFWwindow *window = nullptr;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;
int numberOfTriangles;

int speed; // moving speed of the camera, initially zero

/* Data and their buffers */
// Vertices
GLfloat *g_vertex_buffer_data; // old triangle vertices
GLuint vertexbuffer;
// Colors
GLfloat *g_color_buffer_data;
GLuint colorbuffer;

// Our ModelViewProjection : multiplication of our 3 matrices
glm::mat4 *mvp;

/* Camera params */
glm::vec3 *camera_pos;
glm::vec3 *camera_gaze;
glm::vec3 *camera_up;

static void errorCallback(int error, const char * description) {
    fprintf(stderr, "Error: %s\n", description);
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
void resize(GLFWwindow* window, int width, int height){

}

// XXX: Call this before windows is created and before any other OpenGL call
// function that initiates the vertex array in user domain
void initVertexArray(int width, int height) {
    GLuint VertexArrayId;
    glGenVertexArrays(1, &VertexArrayId);
    glBindVertexArray(VertexArrayId);
}

void fillVertexBuffersData() {
    // An array of 3 vectors which represents 3 vertices
    // TODO fill g_vertex_buffer_data
    // See http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/

    // Identify triangles to OpenGL
    // Generate 1 buffer, put the resulting identifier in vertexbuffer
    glGenBuffers(1, &vertexbuffer);
    // The following commands will talk about our 'vertexbuffer' buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // Give our vertices to OpenGL.
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data,
                 GL_STATIC_DRAW);
}

void initVertexBuffers() {
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);
}

// Call this in while loop
void setBuffers() {
    // 1st attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // TODO not sure on parameters
    glVertexAttribPointer(
            0,                     // attribute 0. No particular reason for 0, but must match the layout in the shader.
            3,                     // size
            GL_FLOAT,              // type
            GL_FALSE,              // normalized?
            0,                     // stride
            (void *) 0             // array buffer offset
    );

    // TODO bunu en sona mı almalıyız ?
    // Draw the triangles !
    glDrawArrays(GL_TRIANGLES, 0, 3 * numberOfTriangles);

    glDisableVertexAttribArray(0);

    // 2nd attribute buffer : colors
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            3,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void *) 0                          // array buffer offset
    );
    glDisableVertexAttribArray(1);

    // TODO put here other buffers as well
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
    numberOfTriangles = 2 * widthTexture * heightTexture; // TODO sure ?

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

    fillVertexBuffersData();

    glfwSetKeyCallback(window, keyboard); // register key callback
    glfwSetWindowSizeCallback(window, resize); // register resize callback

    while (!glfwWindowShouldClose(window)) {
        // Clear the screen TODO desired ?
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(idProgramShader);

        // Send our transformation to the currently bound shader, in the "MVP" uniform
        // This is done in the main loop since each model will have a different MVP matrix (At least for the M part)
        glUniformMatrix4fv(idMVPMatrix, 1, GL_FALSE, &(*mvp)[0][0]);

        setBuffers();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
