#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <GLFW/glfw3.h>
#include <unordered_map>

class InputManager {
public:
    InputManager(GLFWwindow* window);
    ~InputManager();

    void processInput();

    bool isKeyPressed(int key);
    bool isMouseButtonPressed(int button);
    void getMousePosition(double& xPos, double& yPos);

private:
    GLFWwindow* m_window;
    std::unordered_map<int, bool> m_keys;
    std::unordered_map<int, bool> m_mouseButtons;
    double m_mouseX, m_mouseY;

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
    static void joystickCallback(int jid, int event);
    static void gamepadInput(int jid, const GLFWgamepadstate* state);
};

#endif // INPUT_MANAGER_H