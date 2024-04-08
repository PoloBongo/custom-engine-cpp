#include "CoreEngine.h"
#include "EngineTestCode.h"
#include "first_app.h"
#include "Modules/SoundSystemModule.h"

#include <cstdlib>
#include <iostream>

int main()
{
	EngineTestCode engine_test_code;
	engine_test_code.TestCode();

	//SoundSystemModule sound;
	//SoundClass soundSample;
	//SoundGroup soundGroup;
	//Channel channel;

	//sound.createSound(&soundSample, "");
	//sound.playSound(soundSample, true, 5, 0.9f, &channel);

	/**
	 * @brief Fonction principale du programme.
	 *
	 * Cette fonction cr�e une instance de l'application FirstApp et lance son ex�cution en appelant la m�thode Run().
	 *
	 * @return EXIT_SUCCESS si l'application s'est termin�e avec succ�s, EXIT_FAILURE sinon.
	 */
	//lve::FirstApp app{};

	const Engine* engine = Engine::GetInstance();

	engine->Init();

	//SceneManager* scene_module = engine->GetModuleManager()->GetModule<SceneManager>();
	//scene_module->SetScene<MenuScene>();

	/*try
	{*/
		engine->Run();
		//app.Run();
	/*}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}*/

	//sound.releaseSound(soundSample);


	return EXIT_SUCCESS;
}
