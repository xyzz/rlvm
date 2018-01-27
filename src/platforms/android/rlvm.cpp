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

#include <unistd.h>

#include <SDL2/SDL.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#include "AndroidRLVMInstance.hpp"

using namespace std;

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

__attribute__((visibility("default"))) int main(int argc, char* argv[]) {
  // SDL_ANDROID_SetApplicationPutToBackgroundCallback(&appPutToBackground, &appPutToForeground);

  // // get path to game, this call is approximately 2x more disgusting than another one
  // JNIEnv *env = SDL_ANDROID_JniEnv();
  // jobject obj = SDL_ANDROID_JniVideoObject();
  // jclass clazz = env->FindClass("is/xyz/rlvm/DemoRenderer");
  // jmethodID methodID = env->GetMethodID(clazz, "getGamepath", "()Ljava/lang/String;");

  // jobject result = env->CallObjectMethod(obj, methodID);

  // const char* str;
  // jboolean isCopy;
  // str = env->GetStringUTFChars((jstring)result, &isCopy);
  // std::string root_path(str);
  // env->ReleaseStringUTFChars((jstring)result, str);

  // g_root_path = root_path;

  g_root_path = "/sdcard/CLANNAD/"; 

  fs::path gamerootPath(g_root_path);

  instance.Run(gamerootPath);

  return 0;
}
