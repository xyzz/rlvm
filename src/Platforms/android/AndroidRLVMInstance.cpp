#include "AndroidRLVMInstance.hpp"
#include "log.h"

bool AndroidRLVMInstance::AskUserPrompt(const std::string& message_text,
                                        const std::string& informative_text,
                                        const std::string& true_button,
                                        const std::string& false_button) {
	return false;
}

// this is very hacky
bool global_texture_reload = false;

void AndroidRLVMInstance::ReloadAllTextures() {
// call SDLSurface Observe
	LOGD("called ReloadAllTextures\n");
	global_texture_reload = true;
}
