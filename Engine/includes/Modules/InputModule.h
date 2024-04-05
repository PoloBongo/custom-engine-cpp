#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <unordered_map>
#include <GLFW/glfw3.h>

#include "Module.h"

class InputModule final : public Module
{
	public:
		InputModule(GLFWwindow* window);
		~InputModule();

		void processInput();

		bool isKeyPressed(int key);
		bool isMouseButtonPressed(int button);
		void getMousePosition(double& xPos, double& yPos);

		/**
			* @brief Initialise le module.
			*/
		void Init() override;

		/**
		 * @brief Démarre le module.
		 */
		void Start() override;

		/**
		 * @brief Effectue une mise à jour fixe du module.
		 */
		void FixedUpdate() override;

		/**
		 * @brief Met à jour le module.
		 */
		void Update() override;

		/**
		 * @brief Fonction pré-rendu du module.
		 */
		void PreRender() override;

		/**
		 * @brief Rendu du module.
		 */
		void Render() override;

		/**
		 * @brief Rendu de l'interface graphique du module.
		 */
		void RenderGui() override;

		/**
		 * @brief Fonction post-rendu du module.
		 */
		void PostRender() override;

		/**
		 * @brief Libère les ressources utilisées par le module.
		 */
		void Release() override;

		/**
		 * @brief Finalise le module.
		 */
		void Finalize() override;

	private:
		GLFWwindow*                   m_window;
		std::unordered_map<int, bool> m_keys;
		std::unordered_map<int, bool> m_mouseButtons;
		double                        m_mouseX, m_mouseY;

		static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void cursorPositionCallback(GLFWwindow* window, double xPos, double yPos);
		static void joystickCallback(int jid, int event);
		static void gamepadInput(int jid, const GLFWgamepadstate* state);
};

#endif // INPUT_MANAGER_H
