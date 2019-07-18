#include "game_instance.hpp"
#include "common_data.hpp"
#include "states/loading.hpp"

namespace YGOpen
{

GameInstance::GameInstance()
{
	SDL_Log("GameInstance constructor");
}

GameInstance::~GameInstance()
{
	state.reset();
	if(window != nullptr)
		SDL_DestroyWindow(window);
	Drawing::API::UnloadBackend();
	SDL_Log("GameInstance destructor");
}

int GameInstance::Init(Drawing::Backend backend)
{
	data = std::make_shared<CommonData>(*this);
	window = SDL_CreateWindow(DEFAULT_WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED,
	                          SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH,
	                          DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_OPENGL |
	                          SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	if(window == nullptr)
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
		             "Unable to create SDL Window: %s", SDL_GetError());
		return -1;
	}
	if(!Drawing::API::LoadBackend(window, backend))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION,
		                "Unable to load selected backend");
		return -1;
	}
	auto SetDPI = [this]() -> bool
	{
		// NOTE: not checking number of displays because we already were able
		// to create a window which means we have at least 1 display available.
		int displayIndex = SDL_GetWindowDisplayIndex(window);
		if(displayIndex < 0)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
			            "Unable to get display for current window: %s",
			            SDL_GetError());
			return false;
		}
	    if(SDL_GetDisplayDPI(displayIndex, &data->dpi, nullptr, nullptr) < 0)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
			            "Unable to get display DPI: %s",
			            SDL_GetError());
			return false;
		}
		return true;
	};
	if(!SetDPI())
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
		            "Unable to set display DPI. Using default.");
		data->dpi = DEFAULT_DPI;
	}
	SDL_Log("Current DPI: %.2f", static_cast<double>(data->dpi));
// TODO: move this to the API
// 	if(SDL_GL_SetSwapInterval(-1) == -1)
// 	{
// 		SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
// 		            "Unable to set adaptive vsync: %s", SDL_GetError());
// 		// TODO: either make all of this a option or fallback to vsync
// 	}
	Drawing::API::UpdateDrawableSize(&data->canvasWidth, &data->canvasHeight);
	Drawing::API::Clear();
	Drawing::API::Present();
	SDL_ShowWindow(window);
	state = std::make_shared<State::Loading>(data);
	return 0;
}

bool GameInstance::IsExiting() const
{
	return exiting;
}

void GameInstance::Exit()
{
	exiting = true;
}

void GameInstance::PropagateEvent(const SDL_Event& e)
{
	if(e.type == SDL_QUIT)
		exiting = true;
	
	// If the event is a window resize/size change event and the changed window
	// is the one managed by this game instance then change the viewport/extent
	// to match the new size, also, save the new size.
	if(e.type == SDL_WINDOWEVENT &&
	   (e.window.event == SDL_WINDOWEVENT_RESIZED ||
	   e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) &&
	   e.window.windowID == SDL_GetWindowID(window))
	{
		Drawing::API::UpdateDrawableSize(&data->canvasWidth,
		                                 &data->canvasHeight);
	}
	state->OnEvent(e);
}

void GameInstance::TickOnce()
{
	state->Tick();
}

void GameInstance::DrawOnce()
{
	state->Draw();
}

void GameInstance::SetState(std::shared_ptr<State::IState> newState)
{
	state = newState;
}


} // YGOpen
