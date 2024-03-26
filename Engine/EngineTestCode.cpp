#include "EngineTestCode.h"
#include "RessourceManager.h"
#include "DebugManager.h"

void EngineTestCode::TestCode()
{
    SceneManager sceneManager;
    RessourceManager ressourceManager;

    sceneManager.CreateScene("Bongo", false);
    sceneManager.CreateScene("OLYSHEET", false);

    std::cout << "SCENE NAME AT INDEX 1 " + sceneManager.GetSceneAt(1).first << std::endl;

    std::cout << "SCENE ACTIVE NAME : " + sceneManager.GetActiveScene() << std::endl;

    std::cout << "SCENE COUNT : " + std::to_string(sceneManager.SceneCount()) << std::endl;

    std::shared_ptr<TextureManager> texture1 = ressourceManager.LoadTexture("texture1.png");

    DebugManager::LogInfo("Initialisation terminee avec succes.");
    DebugManager::LogWarning("Attention : Memoire faible.");
    DebugManager::LogError("Echec de l'ouverture du fichier.");
    DebugManager::LogSuccess("Succes de l'ouverture du fichier.");
}