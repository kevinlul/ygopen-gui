#include "game_instance.hpp"
#include <memory>
#include <SDL.h>
#include <SDL_image.h>

#if defined(__ANDROID__)
static constexpr Drawing::Backend DEFAULT_BACKEND = Drawing::OPENGL_ES;
#else
static constexpr Drawing::Backend DEFAULT_BACKEND = Drawing::OPENGL_CORE;
#endif

extern "C" int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
{
	int exitCode = 0;
	std::unique_ptr<YGOpen::GameInstance> gi;
	if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to initialize SDL: %s\n", SDL_GetError());
		exitCode = 1;
		goto quit;
	}
	{
		static constexpr auto IMG_INIT_FLAGS = IMG_INIT_PNG;
		if((IMG_Init(IMG_INIT_FLAGS) & IMG_INIT_FLAGS) != IMG_INIT_FLAGS)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			                "Unable to initialize SDL_image: %s",
			                IMG_GetError());
			exitCode = 2;
			goto quit;
		}
	}
	{
		if(TTF_Init() != 0)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
			                "Unable to initialize SDL_ttf: %s",
			                TTF_GetError());
			exitCode = 3;
		}
	}
	try
	{
		gi = std::make_unique<YGOpen::GameInstance>(DEFAULT_BACKEND);
	}
	catch(const std::exception& e)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "%s", e.what());
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, u8"Critical Error",
		                         e.what(), nullptr);
		exitCode = 4;
		goto quit;
	}
	gi->Run();
quit:
	gi.reset();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return exitCode;
}
