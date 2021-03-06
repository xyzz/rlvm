// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006, 2007 Elliot Glaysher
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

#include "Module_Sys_Frame.hpp"

#include <boost/numeric/conversion/cast.hpp>

#include "LongOperations/WaitLongOperation.hpp"
#include "MachineBase/LongOperation.hpp"
#include "MachineBase/RLMachine.hpp"
#include "MachineBase/RLModule.hpp"
#include "MachineBase/RLOperation.hpp"
#include "MachineBase/RLOperation/DefaultValue.hpp"
#include "MachineBase/RLOperation/RLOp_Store.hpp"
#include "Systems/Base/EventListener.hpp"
#include "Systems/Base/EventSystem.hpp"
#include "Systems/Base/FrameCounter.hpp"
#include "Systems/Base/GraphicsSystem.hpp"
#include "Systems/Base/System.hpp"

using namespace std;
using boost::numeric_cast;

namespace {

struct ResetTimer : public RLOp_Void_1< DefaultIntValue_T< 0 > > {
  const int layer_;
  explicit ResetTimer(const int in) : layer_(in) {}

  void operator()(RLMachine& machine, int counter) {
    EventSystem& es = machine.system().event();
    es.getTimer(layer_, counter).set(es);
  }
};

bool TimerIsDone(RLMachine& machine, int layer, int counter,
                 unsigned int target_time) {
  EventSystem& es = machine.system().event();
  return es.getTimer(layer, counter).read(es) > target_time;
}

struct Sys_time : public RLOp_Void_2< IntConstant_T, DefaultIntValue_T< 0 > > {
  const int layer_;
  const bool in_time_c_;
  Sys_time(const int in, const bool timeC) : layer_(in), in_time_c_(timeC) {}

  void operator()(RLMachine& machine, int time, int counter) {
    EventSystem& es = machine.system().event();

    if (es.getTimer(layer_, counter).read(es) <
        numeric_cast<unsigned int>(time)) {
      WaitLongOperation* wait_op = new WaitLongOperation(machine);
      if (in_time_c_)
        wait_op->breakOnClicks();
      wait_op->breakOnEvent(std::bind(
          TimerIsDone, std::ref(machine), layer_, counter, time));
      machine.pushLongOperation(wait_op);
    }
  }
};

struct Timer : public RLOp_Store_1< DefaultIntValue_T<0> > {
  const int layer_;
  explicit Timer(const int in) : layer_(in) {}

  int operator()(RLMachine& machine, int counter) {
    EventSystem& es = machine.system().event();
    return es.getTimer(layer_, counter).read(es);
  }
};

struct CmpTimer
    : public RLOp_Store_2< IntConstant_T, DefaultIntValue_T<0> > {
  const int layer_;
  explicit CmpTimer(const int in) : layer_(in) {}

  int operator()(RLMachine& machine, int val, int counter) {
    EventSystem& es = machine.system().event();
    return es.getTimer(layer_, counter).read(es) > val;
  }
};

struct SetTimer
    : public RLOp_Void_2< IntConstant_T, DefaultIntValue_T<0> > {
  const int layer_;
  explicit SetTimer(const int in) : layer_(in) {}

  void operator()(RLMachine& machine, int val, int counter) {
    EventSystem& es = machine.system().event();
    es.getTimer(layer_, counter).set(es, val);
  }
};

}  // namespace

// -----------------------------------------------------------------------

void addSysTimerOpcodes(RLModule& m) {
  m.addOpcode(110, 0, "ResetTimer", new ResetTimer(0));
  m.addOpcode(110, 1, "ResetTimer", new ResetTimer(0));
  m.addOpcode(111, 0, "time", new Sys_time(0, false));
  m.addOpcode(111, 1, "time", new Sys_time(0, false));
  m.addOpcode(112, 0, "timeC", new Sys_time(0, true));
  m.addOpcode(112, 1, "timeC", new Sys_time(0, true));
  m.addOpcode(114, 0, "Timer", new Timer(0));
  m.addOpcode(114, 1, "Timer", new Timer(0));
  m.addOpcode(115, 0, "CmpTimer", new CmpTimer(0));
  m.addOpcode(115, 1, "CmpTimer", new CmpTimer(0));
  m.addOpcode(116, 0, "CmpTimer", new SetTimer(0));
  m.addOpcode(116, 1, "CmpTimer", new SetTimer(0));

  m.addOpcode(120, 0, "ResetExTimer", new ResetTimer(1));
  m.addOpcode(120, 1, "ResetExTimer", new ResetTimer(1));
  m.addOpcode(121, 0, "timeEx", new Sys_time(1, false));
  m.addOpcode(121, 1, "timeEx", new Sys_time(1, false));
  m.addOpcode(122, 0, "timeExC", new Sys_time(1, true));
  m.addOpcode(122, 1, "timeExC", new Sys_time(1, true));
  m.addOpcode(124, 0, "ExTimer", new Timer(1));
  m.addOpcode(124, 1, "ExTimer", new Timer(1));
  m.addOpcode(125, 0, "CmpExTimer", new CmpTimer(1));
  m.addOpcode(125, 1, "CmpExTimer", new CmpTimer(1));
  m.addOpcode(126, 0, "SetExTimer", new SetTimer(1));
  m.addOpcode(126, 1, "SetExTimer", new SetTimer(1));
}
