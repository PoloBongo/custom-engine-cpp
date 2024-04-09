🖥️ Moteur de jeu 3D avec Vulkan
============================

Notre moteur 3D en développement s'appuie sur la puissance de Vulkan et intègre plusieurs bibliothèques spécialisées pour améliorer le processus de développement. Actuellement, nous explorons l'utilisation de FMOD pour le rendu audio, GLFW pour la gestion des fenêtres, GLM pour les calculs mathématiques, JSON pour la configuration des données, TinyObjLoader pour le chargement des modèles 3D, STB pour divers utilitaires graphiques, et Winsock pour le support réseau. Combinaison qui permet un bon développement et des solutions plus rapides !

🔍 Technologies utilisées
----------------------

[![](https://skillicons.dev/icons?i=cpp,git)](https://skillicons.dev)

🔍 Librairies utilisées
----------------------

*   **[Vulkan](https://www.vulkan.org/)**: API graphique pour le rendu 3D.
*   **[Bullet Physics](https://github.com/bulletphysics/bullet3)**: Simulation de la physique en temps réel.
*   **[Dear ImGui](https://github.com/ocornut/imgui)**: Création d'interfaces utilisateur dans les applications 3D.
*   **[GLFW](https://github.com/glfw/glfw)**: Création de fenêtres et gestion des entrées.
*   **[FMOD](https://www.fmod.com/)**: Gestion audio multiplateforme.
*   **Winsock**: Programmation des sockets pour la communication réseau.
*   **[tinyObjLoader](https://github.com/tinyobjloader/tinyobjloader)**: Chargement de fichiers obj pour les modèles 3D.
*   **[JSON for Modern C++](https://github.com/nlohmann/json)**: Manipulation de fichiers JSON.
*   **[GLM](https://github.com/g-truc/glm)**: Bibliothèque mathématique pour les graphiques.

Comment utiliser
----------------

### ⚙️ Installation des dépendances

*   Assurez-vous d'avoir le [Vulkan SDK](https://vulkan.lunarg.com/sdk/home#windows) installé sur votre système.

### ⚒️ Compilation du projet (si vous utilisez le repository)

1.  Clonez ce dépôt sur votre machine.
2.  Lancez (les dépendences sont déjà faites normalement) Si vous avez vulkan installé autre part que sur le C **(C:\VulkanSDK)**, vous devrez refaire une dépendence [Tuto Vulkan](https://vulkan-tutorial.com/Development_environment) pour vous aider sur comment faire.

### 🔧 Exécution du moteur

*   Lancez l'application compilée.
*   Utilisez l'interface utilisateur pour charger des modèles 3D, définir les paramètres de simulation physique et interagir avec le moteur.

### 🎹 Les commandes

*   Z Q S D Pour les déplacements de la caméra
*   A E Pour les déplacements en hauteur
*   ⬆️ ⬇️ ➡️ ⬅️ pour la rotation de la caméra

Informations supplémentaires
----------------------------

### 🔗 Équipe de développement

*   **[@Cookiezikse](https://www.github.com/cookiezikse)**
*   **[@Quentin](https://github.com/MtPoison)**
*   **[@PoloBongo](https://github.com/PoloBongo)**
*   **[@DemonXlegenD](https://github.com/DemonXlegenD)**
*   **[@Kais](https://github.com/KakaTheRat)**
*   **[@Euexia](https://github.com/Euexia)**
*   **[@Mathias](https://github.com/0nnen)**
*   **[@Maurad](https://github.com/PoloBongo)**
