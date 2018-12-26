#include "helper.h"

static GLFWwindow * win = NULL;

// Shaders
GLuint idProgramShader;
GLuint idFragmentShader;
GLuint idVertexShader;
GLuint idJpegTexture;
GLuint idMVPMatrix;

int widthTexture, heightTexture;

static void errorCallback(int error,
    const char * description) {
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

void increaseSpeed(){}
void decreaseSpeed(){}

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

    win = glfwCreateWindow(600, 600, "CENG477 - HW3", NULL, NULL);

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

    initShaders();
    glUseProgram(idProgramShader);
    initTexture(argv[1], & widthTexture, & heightTexture);

    glfwSetKeyCallback(win, keyboard);

    while (!glfwWindowShouldClose(win)) {
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    return 0;
}
