// This file is part of RLVM, a RealLive virutal machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 El Riot
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
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
//  
// -----------------------------------------------------------------------

#include "Module_Obj.hpp"
#include "Module_ObjPosDims.hpp"

#include "MachineBase/RLOperation.hpp"

#include "Systems/Base/GraphicsSystem.hpp"
#include "Systems/Base/GraphicsObject.hpp"


/**
 * Theoretically implements objGetPos. People don't actually
 * understand what's going on in this module (more so then anything
 * else in rldev/rlvm.)
 *
 * This is probably wrong or overlooks all sorts of weird corner cases
 * that aren't immediatly obvious.
 */
struct Obj_objGetPos
  : public RLOp_Void_3< IntConstant_T, IntReference_T, IntReference_T >
{
  int m_layer;
  Obj_objGetPos(int layer) : m_layer(layer) {}

  void operator()(RLMachine& machine, int objNum, IntReferenceIterator xIt,
                  IntReferenceIterator yIt)
  {
    GraphicsObject& obj = getGraphicsObject(machine, m_layer, objNum);
    *xIt = obj.x();
    *yIt = obj.y();
  }
};

// -----------------------------------------------------------------------

ObjPosDimsModule::ObjPosDimsModule()
  : RLModule("ObjPosDims", 1, 84)
{
  addOpcode(1000, 0, new Obj_objGetPos(OBJ_FG_LAYER));
}