// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2011 Elliot Glaysher
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
// -----------------------------------------------------------------------

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/export.hpp>

#include "Systems/Base/ColourFilterObjectData.hpp"

#include <ostream>
#include <iostream>
using namespace std;

#include "Systems/Base/Colour.hpp"
#include "Systems/Base/ColourFilter.hpp"
#include "Systems/Base/GraphicsObject.hpp"
#include "Systems/Base/GraphicsSystem.hpp"
#include "Systems/Base/System.hpp"
#include "Utilities/Exception.hpp"

ColourFilterObjectData::ColourFilterObjectData(GraphicsSystem& system,
                                               const Rect& screen_rect)
    : graphics_system_(system),
      screen_rect_(screen_rect) {
}

ColourFilterObjectData::~ColourFilterObjectData() {}

ColourFilter* ColourFilterObjectData::GetColourFilter() {
  if (!colour_filer_)
    colour_filer_.reset(graphics_system_.BuildColourFiller());
  return colour_filer_.get();
}

void ColourFilterObjectData::render(const GraphicsObject& go,
                                    const GraphicsObject* parent,
                                    std::ostream* tree) {
  if (go.width() != 100 || go.height() != 100) {
    static bool printed = false;
    if (!printed) {
      printed = true;
      cerr << "We can't yet scaling colour filters." << endl;
    }
  }

  RGBAColour colour = go.colour();
  GetColourFilter()->Fill(go, screen_rect_, colour);

  if (tree) {
    *tree << "  ColourFilterObjectData" << std::endl
          << "  Screen rect: " << screen_rect_ << std::endl
          << "  Colour: " << colour << std::endl
          << "  Properties: ";
    PrintGraphicsObjectToTree(go, tree);
    *tree << endl;
  }
}

int ColourFilterObjectData::pixelWidth(
    const GraphicsObject& rendering_properties) {
  throw rlvm::Exception("There is no sane value for this!");
}

int ColourFilterObjectData::pixelHeight(
    const GraphicsObject& rendering_properties) {
  throw rlvm::Exception("There is no sane value for this!");
}

GraphicsObjectData* ColourFilterObjectData::clone() const {
  return new ColourFilterObjectData(graphics_system_, screen_rect_);
}

void ColourFilterObjectData::execute(RLMachine& machine) {
  // Nothing to do.
}

bool ColourFilterObjectData::isAnimation() const {
  return false;
}

void ColourFilterObjectData::playSet(int set) {
  // No op
}

boost::shared_ptr<const Surface> ColourFilterObjectData::currentSurface(
    const GraphicsObject& rp) {
  return boost::shared_ptr<const Surface>();
}

void ColourFilterObjectData::objectInfo(std::ostream& tree) {
  tree << "ColourFilterObjectData(" << screen_rect_ << ")" << std::endl;
}

ColourFilterObjectData::ColourFilterObjectData(System& system)
    : graphics_system_(system.graphics()) {
}

template<class Archive>
void ColourFilterObjectData::serialize(Archive& ar, unsigned int version) {
  ar & boost::serialization::base_object<GraphicsObjectData>(*this);
  ar & screen_rect_;
}

// -----------------------------------------------------------------------

// Explicit instantiations for text archives (since we hide the
// implementation)

template void ColourFilterObjectData::serialize<boost::archive::text_iarchive>(
  boost::archive::text_iarchive& ar, unsigned int version);
template void ColourFilterObjectData::serialize<boost::archive::text_oarchive>(
  boost::archive::text_oarchive& ar, unsigned int version);

BOOST_CLASS_EXPORT(ColourFilterObjectData);
