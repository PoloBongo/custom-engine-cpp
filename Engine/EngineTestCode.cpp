#include "EngineTestCode.h"

void EngineTestCode::TestCode()
{
    SceneManager sceneManager;

    sceneManager.CreateScene("Bongo", false);
    sceneManager.CreateScene("OLYSHEET", false);

    std::cout << "SCENE ACTIVE NAME : " + sceneManager.GetActiveScene() << std::endl;

    std::cout << "SCENE COUNT : " + std::to_string(sceneManager.SceneCount());
}