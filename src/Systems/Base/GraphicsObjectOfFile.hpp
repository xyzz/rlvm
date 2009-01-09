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
//
// -----------------------------------------------------------------------


#ifndef __GraphicsObjectOfFile_hpp__
#define __GraphicsObjectOfFile_hpp__

#include "Systems/Base/GraphicsObjectData.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

class Surface;
class RLMachine;

// -----------------------------------------------------------------------

/**
 * GraphicsObjectData class that encapsulates a G00 or ANM file.
 *
 * GraphicsObjectOfFile is used for loading individual bitmaps into an
 * object. It has support for normal display, and also
 *
 */
class GraphicsObjectOfFile : public GraphicsObjectData
{
public:
  GraphicsObjectOfFile();
  GraphicsObjectOfFile(RLMachine& machine, const std::string& filename);

  void loadFile(RLMachine& machine);

  const std::string& filename() const { return filename_; }

  virtual int pixelWidth(const GraphicsObject& rp);
  virtual int pixelHeight(const GraphicsObject& rp);

  virtual GraphicsObjectData* clone() const;

  virtual void execute(RLMachine& machine);

  virtual bool isAnimation() const;
  virtual void playSet(RLMachine& machine, int set);

protected:
  virtual void loopAnimation();
  virtual boost::shared_ptr<Surface> currentSurface(const GraphicsObject& go);
  virtual Rect srcRect(const GraphicsObject& go);
  virtual void objectInfo(std::ostream& tree);

private:
  // Private constructor for cloning
  GraphicsObjectOfFile(const GraphicsObjectOfFile& obj);

  /// The name of the graphics file that was loaded.
  std::string filename_;

  /// The encapsulated surface to render
  boost::shared_ptr<Surface> surface_;

  /// Number of miliseconds to spend on a single frame in the
  /// animation
  unsigned int frame_time_;

  /// Current frame displayed (when animating)
  int current_frame_;

  /// While currentlyPlaying() is true, this variable is used to store
  /// the time when the frame was switched last
  unsigned int time_at_last_frame_change_;

  /// boost::serialization support
  friend class boost::serialization::access;

  template<class Archive>
  void save(Archive & ar, const unsigned int file_version) const;

  template<class Archive>
  void load(Archive& ar, const unsigned int file_version);

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif