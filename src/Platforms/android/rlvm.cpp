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

// We include this here because SDL is retarded and works by #define
// main(inat argc, char* agrv[]). Loosers.
#include <SDL/SDL.h>

#include <boost/program_options.hpp>
#include <iostream>
#include <string>

#include <unistd.h>
#include <SDL_android.h>

#include "AndroidRLVMInstance.hpp"

#include "log.h"
#include "GLES/gl.h"

using namespace std;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

AndroidRLVMInstance instance;

bool background = false;

static void appPutToBackground()
{
  // TODO(xyz) wtf, this repeatedly gets called
  if (background) {
    usleep(100);
    return;
  }
  background = true;
  LOGD("got put to background\n");
  SDL_ANDROID_PauseAudioPlayback();
}

static void appPutToForeground()
{
  background = false;
  LOGD("got put to foreground\n");
  instance.ReloadAllTextures();
  SDL_ANDROID_ResumeAudioPlayback();

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


int main(int argc, char* argv[]) {
  sleep(3);

  SDL_ANDROID_SetApplicationPutToBackgroundCallback(&appPutToBackground, &appPutToForeground);

  fs::path gamerootPath("/sdcard/vn/");

  instance.Run(gamerootPath);

  return 0;
}
