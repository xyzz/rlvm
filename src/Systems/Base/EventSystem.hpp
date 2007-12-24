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


#ifndef __EventSystem_hpp__
#define __EventSystem_hpp__

#include <vector>
#include <boost/scoped_ptr.hpp>

#include "Systems/Base/RLTimer.hpp"

class RLMachine;

class Gameexe;
class FrameCounter;
class EventHandler;

// -----------------------------------------------------------------------

struct EventSystemGlobals
{
  EventSystemGlobals();
  EventSystemGlobals(Gameexe& gexe);

  /// The two generic values that the reallive game has control over
  /// with the Generic1 and Generic2 functions.
  int generic1, generic2;

  /// boost::serialization support
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & generic1 & generic2;
  }
};

// -----------------------------------------------------------------------

/**
 * Generalization of an event system. Reallive's event model is a bit
 * weird; interpreted code will check the state of certain keyboard
 * modifiers, with functions such as CtrlPressed() or ShiftPressed().
 *
 * So what's the solution? Have two different event systems side by
 * side. One is exposed to Reallive and mimics what RealLive bytecode
 * expects. The other is based on event handlers and is sane.
 */
class EventSystem
{
protected:
  typedef std::vector<EventHandler*> Handlers;

private:
  boost::scoped_ptr<FrameCounter> m_frameCounters[255][2];
  RLTimer m_timers[255][2];

  /// Counter for the number of things that require realtime
  /// speed. Whenever this is zero, the system will wait 10ms between
  /// rendering frames to be nice to the user and the OS.
  int m_numberOfRealtimeTasks;

  int m_numberOfNiceAfterEachTaskItems;

  /// Helper function that verifies input
  void checkLayerAndCounter(int layer, int counter);

  Handlers m_eventHandlers;

  EventSystemGlobals m_globals;

protected:
  Handlers::iterator handlers_begin() { return m_eventHandlers.begin(); }
  Handlers::iterator handlers_end() { return m_eventHandlers.end(); }

public:
  EventSystem(Gameexe& gexe);
  virtual ~EventSystem();

  virtual void executeEventSystem(RLMachine& machine) = 0;

  virtual void addEventHandler(EventHandler* handler);
  virtual void removeEventHandler(EventHandler* handler);

  /** 
   * Returns whether shift is currently pressed.
   * 
   * @return 
   */
  virtual bool shiftPressed() const = 0;

  /** 
   * Returns whether ctrl has been presed since the last invocation of
   * ctrlPresesd().
   *
   * @todo Do I have to keep track of if the key was pressed between
   * two ctrlPressed() invocations?
   */
  virtual bool ctrlPressed() const = 0;

  /** 
   * Returns the number of milliseconds since the program
   * started. Used for timing things.
   */
  virtual unsigned int getTicks() const = 0;

  void setFrameCounter(int layer, int frameCounter, FrameCounter* counter);
  FrameCounter& getFrameCounter(int layer, int frameCounter);
  bool frameCounterExists(int layer, int frameCounter);

  RLTimer& getTimer(int layer, int counter) 
  { return m_timers[layer][counter]; }

  /** 
   * Idles the program for a certain amount of time in milliseconds.
   * 
   * @param milliseconds Time
   */
  virtual void wait(unsigned int milliseconds) const = 0;

  // -----------------------------------------------------------------------

  /**
   * @name Keyboard and Mouse Input (Event Handler style)
   * 
   * @{
   */


  /// @}


  /**
   * @name Keyboard and Mouse Input (Reallive style)
   * 
   * @{
   */
  virtual void getCursorPos(int& xPos, int& yPos) {}

  /** 
   * Gets the location of the mouse cursor and the button states.
   * 
   * The following values are used to indicate a button's status:
   * - 0 if unpressed
   * - 1 if being pressed
   * - 2 if pressed and released.
   */
  virtual void getCursorPos(int& xPos, int& yPos, int& button1, int& button2) {}

  /** 
   * Resets the state of the mouse buttons.
   */
  virtual void flushMouseClicks() {}

  /// @}

  // -----------------------------------------------------------------------

  /**
   * @name Generic values
   * 
   */
  void setGeneric1(const int in) { m_globals.generic1 = in; }
  int generic1() const { return m_globals.generic1; }

  void setGeneric2(const int in) { m_globals.generic2 = in; }
  int generic2() const { return m_globals.generic2; }
  /// @}

  // -----------------------------------------------------------------------

  EventSystemGlobals& globals() { return m_globals; }
};

#endif
