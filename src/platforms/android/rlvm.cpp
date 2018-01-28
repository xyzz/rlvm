// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2014 Ilya Zhuravlev
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
//
// -----------------------------------------------------------------------

#include <SDL2/SDL.h>
#include <jni.h>

#include <boost/program_options.hpp>
#include <iostream>
#include <string>
#include <atomic>

#include <unistd.h>

#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "AndroidRLVMInstance.hpp"

#include "libreallive/gameexe.h"
#include "systems/base/rect.h"
#include "utilities/file.h"
#include "utilities/graphics.h"

using namespace std;

#define SDL_JAVA_PREFIX                                 org_libsdl_app
#define CONCAT1(prefix, class, function)                CONCAT2(prefix, class, function)
#define CONCAT2(prefix, class, function)                Java_ ## prefix ## _ ## class ## _ ## function
#define SDL_JAVA_INTERFACE(function)                    CONCAT1(SDL_JAVA_PREFIX, SDLActivity, function)

namespace po = boost::program_options;
namespace fs = boost::filesystem;

AndroidRLVMInstance instance;
std::string g_root_path;

bool g_background = false;

static void appPutToBackground()
{
  // TODO(xyz) wtf, this repeatedly gets called
  g_background = true;
  // SDL_ANDROID_PauseAudioPlayback();
}

static void appPutToForeground()
{
  g_background = false;
  instance.ReloadAllTextures();
  // SDL_ANDROID_ResumeAudioPlayback();

  // TODO(xyz): remove the copypasta here
  glEnable(GL_TEXTURE_2D);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Enable Texture Mapping ( NEW )
  glEnable(GL_TEXTURE_2D);

  // Enable smooth shading
  glShadeModel(GL_SMOOTH);

  // Set the background black
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Depth buffer setup
  glClearDepthf(1.0f);

  // Enables Depth Testing
  glEnable(GL_DEPTH_TEST);

  glEnable(GL_BLEND);

  // The Type Of Depth Test To Do
  glDepthFunc(GL_LEQUAL);

  // Really Nice Perspective Calculations
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  // Full Brightness, 50% Alpha ( NEW )
  glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
}

void ScreenSizeCallback(const Size& size) {
  JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

  // retrieve the Java instance of the SDLActivity
  jobject activity = (jobject)SDL_AndroidGetActivity();

  // find the Java class of the activity. It should be SDLActivity or a subclass of it.
  jclass clazz(env->GetObjectClass(activity));

  // find the identifier of the method to call
  jmethodID method_id = env->GetMethodID(clazz, "setScreenSize", "(II)V");

  // effectively call the Java method
  env->CallVoidMethod(activity, method_id, size.width(), size.height());

  // clean up the local references.
  env->DeleteLocalRef(activity);
  env->DeleteLocalRef(clazz);
}

// has to be duplicated here because we need to set ScreenSize ASAP
boost::filesystem::path FindGameFile(
    const boost::filesystem::path& gamerootPath,
    const std::string& filename) {
  fs::path search_for = gamerootPath / filename;
  fs::path corrected_path = CorrectPathCase(search_for);
  if (corrected_path.empty()) {
    throw std::runtime_error("Corrected path is empty");
  }

  return corrected_path;
}

static std::atomic<bool> ready;

extern "C" JNIEXPORT void JNICALL SDL_JAVA_INTERFACE(nativeReady)(JNIEnv* env, jclass cls) {
  ready = true;
}

__attribute__((visibility("default"))) int main(int argc, char* argv[]) {
  g_root_path = "/storage/5170-FBC7/CLANNAD/";

  fs::path gameexePath = FindGameFile(g_root_path, "Gameexe.ini");

  ready = false;

  Gameexe gameexe(gameexePath);
  ScreenSizeCallback(GetScreenSize(gameexe));

  // we want the surface to get the changes and resize itself before we create SDL2 context and everything
  while (!ready)
    sleep(1);

  fs::path gamerootPath(g_root_path);

  instance.Run(gamerootPath);

  return 0;
}

