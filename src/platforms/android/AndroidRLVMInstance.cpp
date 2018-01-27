#include "AndroidRLVMInstance.hpp"

#include <SDL2/SDL.h>

bool AndroidRLVMInstance::AskUserPrompt(const std::string& message_text,
                                        const std::string& informative_text,
                                        const std::string& true_button,
                                        const std::string& false_button) {
	return false;
}

// this is very hacky
bool global_texture_reload = false;

void AndroidRLVMInstance::ReloadAllTextures() {
	global_texture_reload = true;
}

void AndroidRLVMInstance::ReportFatalError(const std::string& message_text,
                                    const std::string& informative_text) {
  std::string message = message_text + ": " + informative_text;
  SDL_ShowSimpleMessageBox(0, "rlvm: Fatal error", message.c_str(), NULL);
}
