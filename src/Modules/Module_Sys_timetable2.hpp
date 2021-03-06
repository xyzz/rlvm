// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2013 Elliot Glaysher
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// -----------------------------------------------------------------------

#ifndef SRC_MODULES_MODULE_SYS_TIMETABLE2_HPP_
#define SRC_MODULES_MODULE_SYS_TIMETABLE2_HPP_

class RLModule;

#include "MachineBase/RLOperation.hpp"
#include "MachineBase/RLOperation/Argc_T.hpp"
#include "MachineBase/RLOperation/Complex_T.hpp"
#include "MachineBase/RLOperation/RLOp_Store.hpp"
#include "MachineBase/RLOperation/Special_T.hpp"

// Maps the time table commands high tags down to normal.
struct TimeTableMapper {
  static int GetTypeForTag(const libReallive::SpecialExpressionPiece& sp);
};

// Defines timetable2's input pattern.
typedef Complex2_T<IntConstant_T, IntConstant_T> TT_Move2;
typedef Complex3_T<IntConstant_T, IntConstant_T, IntConstant_T> TT_Move3;
typedef IntConstant_T TT_Set;
typedef IntConstant_T TT_Wait;
typedef Complex3_T<IntConstant_T, IntConstant_T, IntConstant_T> TT_Turn;
typedef Complex3_T<IntConstant_T, IntConstant_T, IntConstant_T> TT_Turnup;
typedef Complex3_T<IntConstant_T, IntConstant_T, IntConstant_T> TT_Loop;
typedef Complex3_T<IntConstant_T, IntConstant_T, IntConstant_T> TT_Jump;
typedef Complex2_T<IntConstant_T, IntConstant_T> TT_WaitSet;

typedef Special_T< TimeTableMapper,
                   TT_Move2,
                   TT_Move3,
                   TT_Set,
                   TT_Wait,
                   TT_Turn,
                   TT_Turnup,
                   TT_Loop,
                   TT_Jump,
                   TT_WaitSet > TimeTable2Entry;
typedef Argc_T< TimeTable2Entry > TimeTable2List;

// Implementation of the math performing timetable2 command.
struct Sys_timetable2
    : public RLOp_Store_5<IntConstant_T, IntConstant_T, IntConstant_T,
                          IntConstant_T, TimeTable2List> {
  // Main entrypoint
  virtual int operator()(RLMachine& machine, int now_time, int rep_time,
                         int start_time, int start_num,
                         TimeTable2List::type index_list);

  int Jump(int start_time, int now_time, int end_time,
           int start_num, int end_num, int count);
};

// index_series has its own file.
void addTimetable2Opcode(RLModule& module);

#endif  // SRC_MODULES_MODULE_SYS_TIMETABLE2_HPP_
