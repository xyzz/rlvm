
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

#ifndef SRC_SYSTEMS_BASE_GRAPHICSOBJECT_HPP_
#define SRC_SYSTEMS_BASE_GRAPHICSOBJECT_HPP_

#include "Systems/Base/Colour.hpp"
#include "Systems/Base/Rect.hpp"

#include <string>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/version.hpp>

class RLMachine;
class GraphicsObject;
class GraphicsObjectSlot;
class GraphicsObjectData;
class ObjectMutator;

// Describes an independent, movable graphical object on the
// screen. GraphicsObject, internally, references a copy-on-write
// datastructure, which in turn has optional components to save
// memory.
//
// @todo I want to put index checks on a lot of these accessors.
class GraphicsObject {
 public:
  GraphicsObject();
  GraphicsObject(const GraphicsObject& obj);
  ~GraphicsObject();
  GraphicsObject& operator=(const GraphicsObject& obj);

  // Object Position Accessors

  // This code, while a boolean, uses an int so that we can get rid
  // of one template parameter in one of the generic operation
  // functors.
  int visible() const { return impl_->visible_; }
  void setVisible(const int in);

  int x() const { return impl_->x_; }
  void setX(const int x);

  int y() const { return impl_->y_; }
  void setY(const int y);

  int xAdjustment(int idx) const { return impl_->adjust_x_[idx]; }
  int xAdjustmentSum() const;
  void setXAdjustment(int idx, int x);

  int yAdjustment(int idx) const { return impl_->adjust_y_[idx]; }
  int yAdjustmentSum() const;
  void setYAdjustment(int idx, int y);
  void setXYAdjustments(int idx, int x, int y);

  int vert() const { return impl_->whatever_adjust_vert_operates_on_; }
  void setVert(const int vert);

  int xOrigin() const { return impl_->origin_x_; }
  void setXOrigin(const int x);

  int yOrigin() const { return impl_->origin_y_; }
  void setYOrigin(const int y);

  int xRepOrigin() const { return impl_->rep_origin_x_; }
  void setXRepOrigin(const int x);

  int yRepOrigin() const { return impl_->rep_origin_y_; }
  void setYRepOrigin(const int y);

  // Note: width/height are object scale percentages.
  int width() const { return impl_->width_; }
  void setWidth(const int in);
  int height() const { return impl_->height_; }
  void setHeight(const int in);

  // Note: width/height are object scale factors out of 1000.
  int hqWidth() const { return impl_->hq_width_; }
  void setHqWidth(const int in);
  int hqHeight() const { return impl_->hq_height_; }
  void setHqHeight(const int in);

  float getWidthScaleFactor() const;
  float getHeightScaleFactor() const;

  int rotation() const { return impl_->rotation_; }
  void setRotation(const int in);

  int pixelWidth() const;
  int pixelHeight() const;

  // Object attribute accessors
  int pattNo() const;
  void setPattNo(const int in);

  int mono() const { return impl_->mono_; }
  void setMono(const int in);

  int invert() const { return impl_->invert_; }
  void setInvert(const int in);

  int light() const { return impl_->light_; }
  void setLight(const int in);

  const RGBColour& tint() const { return impl_->tint_; }
  void setTint(const RGBColour& colour);
  void setTintR(const int in);
  void setTintG(const int in);
  void setTintB(const int in);

  const RGBAColour& colour() const { return impl_->colour_; }
  void setColour(const RGBAColour& colour);
  void setColourR(const int in);
  void setColourG(const int in);
  void setColourB(const int in);
  void setColourLevel(const int in);

  int compositeMode() const { return impl_->composite_mode_; }
  void setCompositeMode(const int in);

  int scrollRateX() const { return impl_->scroll_rate_x_; }
  void setScrollRateX(const int x);

  int scrollRateY() const { return impl_->scroll_rate_y_; }
  void setScrollRateY(const int y);

  // Three level zorder.
  int zOrder() const { return impl_->z_order_; }
  void setZOrder(const int in);
  int zLayer() const { return impl_->z_layer_; }
  void setZLayer(const int in);
  int zDepth() const { return impl_->z_depth_; }
  void setZDepth(const int in);

  int computedAlpha() const;
  int rawAlpha() const { return impl_->alpha_; }
  void setAlpha(const int alpha);
  int alphaAdjustment(int idx) const { return impl_->adjust_alpha_[idx]; }
  void setAlphaAdjustment(int idx, int alpha);

  bool hasClip() const {
    return impl_->clip_.width() >= 0 || impl_->clip_.height() >= 0;
  }
  void clearClip();
  void setClip(const Rect& rec);
  const Rect& clipRect() const { return impl_->clip_; }

  bool hasOwnClip() const {
    return impl_->own_clip_.width() >= 0 || impl_->own_clip_.height() >= 0;
  }
  void clearOwnClip();
  void setOwnClip(const Rect& rec);
  const Rect& ownClipRect() const { return impl_->own_clip_; }

  bool hasObjectData() const { return object_data_.get(); }

  GraphicsObjectData& objectData();
  void setObjectData(GraphicsObjectData* obj);

  // Render!
  void render(int objNum,
              const GraphicsObject* parent,
              std::ostream* tree);

  // Deletes the object data. Corresponds to the RLAPI command obj_delete.
  void deleteObject();

  // Clears the impl data without deleting the loaded graphics object.
  void resetProperties();

  // Deletes the object data and resets all values in this
  // GraphicsObject. Corresponds to the RLAPI command obj_clear.
  void clearObject();

  int wipeCopy() const { return impl_->wipe_copy_; }
  void setWipeCopy(const int wipe_copy);

  // Called each pass through the gameloop to see if this object needs
  // to force a redraw, or something.
  void execute(RLMachine& machine);

  // Text Object accessors
  void setTextText(const std::string& utf8str);
  const std::string& textText() const;

  void setTextOps(int size, int xspace, int yspace, int char_count, int colour,
                  int shadow);
  int textSize() const;
  int textXSpace() const;
  int textYSpace() const;
  int textCharCount() const;
  int textColour() const;
  int textShadowColour() const;

  // Drift object accessors
  void setDriftOpts(int count, int use_animation, int start_pattern,
                    int end_pattern, int total_animation_time_ms, int yspeed,
                    int period, int amplitude, int use_drift,
                    int unknown_drift_property, int driftspeed,
                    Rect driftarea);

  int driftParticleCount() const;
  int driftUseAnimation() const;
  int driftStartPattern() const;
  int driftEndPattern() const;
  int driftAnimationTime() const;
  int driftYSpeed() const;
  int driftPeriod() const;
  int driftAmplitude() const;
  int driftUseDrift() const;
  int driftUnknown() const;
  int driftDriftSpeed() const;
  Rect driftArea() const;

  // Digit object accessors
  void setDigitValue(int value);
  void setDigitOpts(int digits, int zero, int sign, int pack, int space);

  int digitValue() const;
  int digitDigits() const;
  int digitZero() const;
  int digitSign() const;
  int digitPack() const;
  int digitSpace() const;

  // Button object accessors
  void setButtonOpts(int action, int se, int group, int button_number);
  void setButtonState(int state);

  int isButton() const;
  int buttonAction() const;
  int buttonSe() const;
  int buttonGroup() const;
  int buttonNumber() const;
  int buttonState() const;

  // Called only from ButtonObjectSelectLongOperation. Sets override
  // properties.
  void setButtonOverrides(int override_pattern,
                          int override_x_offset,
                          int override_y_offset);
  void clearButtonOverrides();

  bool buttonUsingOverides() const;
  int buttonPatternOverride() const;
  int buttonXOffsetOverride() const;
  int buttonYOffsetOverride() const;

  // Adds a mutator to the list of active mutators. GraphicsSystem takes
  // ownership of the passed in object.
  void AddObjectMutator(ObjectMutator* mutator);

  // Returns true if a mutator matching the following parameters is currently
  // running.
  bool IsMutatorRunningMatching(int repno, const char* name);

  // Ends all mutators that match the given parameters.
  void EndObjectMutatorMatching(RLMachine& machine, int repno,
                                const char* name, int speedup);

  // Returns the number of GraphicsObject instances sharing the
  // internal copy-on-write object. Only used in unit testing.
  int32_t referenceCount() const { return impl_.use_count(); }

  // Whether we have the default shared data. Only used in unit testing.
  bool isCleared() const { return impl_ == s_empty_impl; }

 private:
  // Makes the ineternal copy for our copy-on-write semantics. This function
  // checks to see if our Impl object has only one reference to it. If it
  // doesn't, a local copy is made.
  void makeImplUnique();

  // Immediately delete all mutators; doesn't run their SetToEnd() method.
  void deleteObjectMutators();

  // Implementation data structure. GraphicsObject::Impl is the internal data
  // store for GraphicsObjects' copy-on-write semantics.
  struct Impl {
    Impl();
    Impl(const Impl& rhs);
    ~Impl();

    Impl& operator=(const Impl& rhs);

    // Visiblitiy. Different from whether an object is in the bg or fg layer
    bool visible_;

    // The positional coordinates of the object
    int x_, y_;

    // Eight additional parameters that are added to x and y during
    // rendering.
    int adjust_x_[8], adjust_y_[8];

    // Whatever obj_adjust_vert operates on; what's this used for?
    int whatever_adjust_vert_operates_on_;

    // The origin
    int origin_x_, origin_y_;

    // "Rep" origin. This second origin is added to the normal origin
    // only in cases of rotating and scaling.
    int rep_origin_x_, rep_origin_y_;

    // The size of the object, given in integer percentages of [0,
    // 100]. Used for scaling.
    int width_, height_;

    // A second scaling factor, given between [0, 1000].
    int hq_width_, hq_height_;

    // The rotation degree / 10
    int rotation_;

    // Object attributes.

    // The region ("pattern") in g00 bitmaps
    int patt_no_;

    // The source alpha for this image
    int alpha_;

    // Eight additional alphas that are averaged during rendering.
    int adjust_alpha_[8];

    // The clipping region for this image
    Rect clip_;

    // A second clipping region in the object's own space.
    Rect own_clip_;

    // The monochrome transformation
    int mono_;

    // The invert transformation
    int invert_;

    int light_;

    RGBColour tint_;

    // Applies a colour to the object by blending it directly at the
    // alpha components opacity.
    RGBAColour colour_;

    int composite_mode_;

    int scroll_rate_x_, scroll_rate_y_;

    // Three deep zordering.
    int z_order_, z_layer_, z_depth_;

    // Text Object properties
    struct TextProperties {
      TextProperties();

      std::string value;

      int text_size, xspace, yspace;

      int char_count;
      int colour;
      int shadow_colour;

      // boost::serialization support
      template<class Archive>
      void serialize(Archive& ar, unsigned int version);
    };

    void makeSureHaveTextProperties();
    boost::scoped_ptr<TextProperties> text_properties_;

    // Drift Object properties
    struct DriftProperties {
      DriftProperties();

      int count;

      int use_animation;
      int start_pattern;
      int end_pattern;
      int total_animation_time_ms;

      int yspeed;

      int period;
      int amplitude;

      int use_drift;
      int unknown_drift_property;
      int driftspeed;

      Rect drift_area;

      // boost::serialization support
      template<class Archive>
      void serialize(Archive& ar, unsigned int version);
    };

    void makeSureHaveDriftProperties();
    boost::scoped_ptr<DriftProperties> drift_properties_;

    // Digit Object properties
    struct DigitProperties {
      DigitProperties();

      int value;

      int digits;
      int zero;
      int sign;
      int pack;
      int space;

      // boost::serialization support
      template<class Archive>
      void serialize(Archive& ar, unsigned int version);
    };

    void makeSureHaveDigitProperties();
    boost::scoped_ptr<DigitProperties> digit_properties_;

    // Button Object properties
    struct ButtonProperties {
      ButtonProperties();

      int is_button;

      int action;
      int se;
      int group;
      int button_number;

      int state;

      bool using_overides;
      int pattern_override;
      int x_offset_override;
      int y_offset_override;

      // boost::serialization support
      template<class Archive>
      void serialize(Archive& ar, unsigned int version);
    };

    void makeSureHaveButtonProperties();
    boost::scoped_ptr<ButtonProperties> button_properties_;

    // The wipe_copy bit
    int wipe_copy_;

    friend class boost::serialization::access;

    // boost::serialization support
    template<class Archive>
    void serialize(Archive& ar, unsigned int version);
  };

  // Default empty GraphicsObject::Impl. This variable is allocated
  // once, and then is used as the initial value of impl_, where it
  // is cloned on write.
  static const boost::shared_ptr<GraphicsObject::Impl> s_empty_impl;

  // Our actual implementation data
  boost::shared_ptr<GraphicsObject::Impl> impl_;

  // The actual data used to render the object
  boost::scoped_ptr<GraphicsObjectData> object_data_;

  // Tasks that run every tick. Used to mutate object parameters over time (and
  // how we check from a blocking LongOperation if the mutation is ongoing).
  //
  // I think R23 mentioned that these were called "Parameter Events" in the
  // RLMAX SDK.
  std::vector<ObjectMutator*> object_mutators_;

  friend class boost::serialization::access;

  // boost::serialization support
  template<class Archive>
  void serialize(Archive& ar, unsigned int version);
};

BOOST_CLASS_VERSION(GraphicsObject::Impl, 7)

static const int OBJ_FG = 0;
static const int OBJ_BG = 1;

#endif  // SRC_SYSTEMS_BASE_GRAPHICSOBJECT_HPP_

