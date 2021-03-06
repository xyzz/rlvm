// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 Elliot Glaysher
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

#include "Modules/Module_EventLoop.hpp"

#include "MachineBase/GeneralOperations.hpp"
#include "MachineBase/RLMachine.hpp"
#include "Systems/Base/GraphicsSystem.hpp"
#include "Systems/Base/System.hpp"
#include "Systems/Base/TextSystem.hpp"

namespace {

struct setOverride : public RLOp_Void_1< IntConstant_T > {
  bool value_;
  explicit setOverride(int value) : value_(value) { }

  void operator()(RLMachine& machine, int window) {
    machine.system().text().setVisualOverride(window, value_);
  }
};

}  // namespace

EventLoopModule::EventLoopModule()
    : RLModule("EventLoop", 0, 4) {
  addUnsupportedOpcode(120, 0, "SetInterrupt");
  addUnsupportedOpcode(121, 0, "ClearInterrupt");
  addUnsupportedOpcode(303, 0, "yield");

  // Theoretically the same as rtl, but we don't really know.
  addOpcode(300, 0, "rtlButton", callFunction(&RLMachine::returnFromFarcall));
  addOpcode(301, 0, "rtlCancel", callFunction(&RLMachine::returnFromFarcall));
  addOpcode(302, 0, "rtlSystem", callFunction(&RLMachine::returnFromFarcall));

  addOpcode(1000, 0, "ShowBackground",
            callFunction(&GraphicsSystem::toggleInterfaceHidden));
  addOpcode(1100, 0, "SetSkipMode",
            callFunctionWith(&TextSystem::setSkipMode, 1));
  addOpcode(1101, 0, "ClearSkipMode",
            callFunctionWith(&TextSystem::setSkipMode, 0));
  addOpcode(1102, 0, "SkipMode", returnIntValue(&TextSystem::skipMode));

  // opcode<0:4:1202, 0> and opcode<0:4:1200, 0> are used in the CLANNAD menu
  // system; no idea what they do.
  addOpcode(1200, 0, "TextwindowOverrideShow", new setOverride(true));
  addOpcode(1200, 2, "TextwindowOverrideShow",
            callFunctionWith(&TextSystem::setVisualOverrideAll, true));
  addOpcode(1201, 0, "TextwindowOverrideHide", new setOverride(false));
  addOpcode(1201, 2, "TextwindowOverrideHide",
            callFunctionWith(&TextSystem::setVisualOverrideAll, false));
  addOpcode(1202, 0, "ClearTextwindowOverrides",
            callFunction(&TextSystem::clearVisualOverrides));
}
