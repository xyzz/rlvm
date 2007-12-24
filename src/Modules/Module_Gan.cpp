// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2007 Elliot Glaysher
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

#include "Precompiled.hpp"

// -----------------------------------------------------------------------

#include "Modules/Module_Gan.hpp"
#include "Modules/Module_Obj.hpp"
#include "MachineBase/RLMachine.hpp"
#include "MachineBase/RLOperation.hpp"
#include "MachineBase/RLModule.hpp"
#include "MachineBase/LongOperation.hpp"
#include "Systems/Base/System.hpp"
#include "Systems/Base/GraphicsSystem.hpp"
#include "Systems/Base/GraphicsObject.hpp"
#include "Systems/Base/GanGraphicsObjectData.hpp"

#include <boost/shared_ptr.hpp>

using namespace std;
using namespace boost;
using namespace libReallive;

// -----------------------------------------------------------------------

struct Gan_ganPlay : public RLOp_Void_2<IntConstant_T, IntConstant_T>
{
  struct WaitForGanToFinish : public LongOperation
  {
    int m_layer;
    int m_buf;
    WaitForGanToFinish(int inLayer, int inBuf) 
      : m_layer(inLayer), m_buf(inBuf) {}

    bool operator()(RLMachine& machine)
    {
      GraphicsObject& obj = getGraphicsObject(machine, m_layer, m_buf);

      if(!obj.visible())
        cerr << "WARNING, OBJECT INVISIBLE!" << endl;

      if(obj.hasObjectData())
      {
        const GraphicsObjectData& data = obj.objectData();
        if(data.isAnimation())
          return !data.currentlyPlaying();
        else
          return true;
      }
      else
        return true;
    }
  };

  bool m_block;
  int m_layer;
  GraphicsObjectData::AfterAnimation m_afterEffect;

  Gan_ganPlay(bool block, int layer,
              GraphicsObjectData::AfterAnimation after) 
    : m_block(block), m_layer(layer), m_afterEffect(after) {}

  void operator()(RLMachine& machine, int buf, int animationSet)
  {
    GraphicsObject& obj = getGraphicsObject(machine, m_layer, buf);

    if(obj.hasObjectData())
    {
      GraphicsObjectData& data = obj.objectData();
      if(data.isAnimation())
      {
        data.playSet(machine, animationSet);
        data.setAfterAction(m_afterEffect);

        if(m_block)
          machine.pushLongOperation(new WaitForGanToFinish(m_layer, buf));
      }
    }
  }
};

// -----------------------------------------------------------------------

void addGanOperationsTo(RLModule& m, int layer)
{
  m.addUnsupportedOpcode(1000, 0, "objStop");
  m.addUnsupportedOpcode(1000, 1, "objStop");

  m.addOpcode(1001, 0, "ganLoop",
              new Gan_ganPlay(false, layer, GraphicsObjectData::AFTER_LOOP));
  m.addOpcode(1003, 0, "ganPlay",
              new Gan_ganPlay(false, layer, GraphicsObjectData::AFTER_NONE));
  m.addOpcode(1005, 0, "ganPlayOnce",
              new Gan_ganPlay(false, layer, GraphicsObjectData::AFTER_CLEAR));
  m.addOpcode(1006, 0, "ganPlayEx",
              new Gan_ganPlay(true, layer, GraphicsObjectData::AFTER_NONE));
  m.addOpcode(1007, 0, "ganPlayOnceEx",
              new Gan_ganPlay(true, layer, GraphicsObjectData::AFTER_CLEAR));

  m.addOpcode(2001, 0, "objLoop",
              new Gan_ganPlay(false, layer, GraphicsObjectData::AFTER_LOOP));
  m.addUnsupportedOpcode(2003, 0, "objPlay");

  m.addOpcode(3001, 0, "ganLoop2",
              new Gan_ganPlay(false, layer, GraphicsObjectData::AFTER_LOOP));
  m.addOpcode(3003, 0, "ganPlay2", 
              new Gan_ganPlay(false, layer, GraphicsObjectData::AFTER_NONE));
  m.addOpcode(3005, 0, "ganPlayOnce2",
              new Gan_ganPlay(false, layer, GraphicsObjectData::AFTER_CLEAR));
}

// -----------------------------------------------------------------------

GanFgModule::GanFgModule()
  : RLModule("GanFg", 1, 73)
{
  addGanOperationsTo(*this, OBJ_FG_LAYER);
}

// -----------------------------------------------------------------------

GanBgModule::GanBgModule()
  : RLModule("GanBg", 1, 74)
{
  addGanOperationsTo(*this, OBJ_BG_LAYER);
}
