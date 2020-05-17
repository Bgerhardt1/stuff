// Copyright (c) 2015 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/common/native_mate_converters/gfx_converter.h"

#include "native_mate/dictionary.h"
#include "ui/display/display.h"
#include "ui/display/screen.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace mate {

v8::Local<v8::Value> Converter<gfx::Point>::ToV8(v8::Isolate* isolate,
                                                 const gfx::Point& val) {
  mate::Dictionary dict = mate::Dictionary::CreateEmpty(isolate);
  dict.SetHidden("simple", true);
  dict.Set("x", val.x());
  dict.Set("y", val.y());
  return dict.GetHandle();
}

bool Converter<gfx::Point>::FromV8(v8::Isolate* isolate,
                                   v8::Local<v8::Value> val,
                                   gfx::Point* out) {
  mate::Dictionary dict;
  if (!ConvertFromV8(isolate, val, &dict))
    return false;
  double x, y;
  if (!dict.Get("x", &x) || !dict.Get("y", &y))
    return false;
  *out = gfx::Point(static_cast<int>(std::round(x)),
                    static_cast<int>(std::round(y)));
  return true;
}

v8::Local<v8::Value> Converter<gfx::PointF>::ToV8(v8::Isolate* isolate,
                                                  const gfx::PointF& val) {
  mate::Dictionary dict = mate::Dictionary::CreateEmpty(isolate);
  dict.SetHidden("simple", true);
  dict.Set("x", val.x());
  dict.Set("y", val.y());
  return dict.GetHandle();
}

bool Converter<gfx::PointF>::FromV8(v8::Isolate* isolate,
                                    v8::Local<v8::Value> val,
                                    gfx::PointF* out) {
  mate::Dictionary dict;
  if (!ConvertFromV8(isolate, val, &dict))
    return false;
  float x, y;
  if (!dict.Get("x", &x) || !dict.Get("y", &y))
    return false;
  *out = gfx::PointF(x, y);
  return true;
}

v8::Local<v8::Value> Converter<gfx::Size>::ToV8(v8::Isolate* isolate,
                                                const gfx::Size& val) {
  mate::Dictionary dict = mate::Dictionary::CreateEmpty(isolate);
  dict.SetHidden("simple", true);
  dict.Set("width", val.width());
  dict.Set("height", val.height());
  return dict.GetHandle();
}

bool Converter<gfx::Size>::FromV8(v8::Isolate* isolate,
                                  v8::Local<v8::Value> val,
                                  gfx::Size* out) {
  mate::Dictionary dict;
  if (!ConvertFromV8(isolate, val, &dict))
    return false;
  int width, height;
  if (!dict.Get("width", &width) || !dict.Get("height", &height))
    return false;
  *out = gfx::Size(width, height);
  return true;
}

v8::Local<v8::Value> Converter<gfx::Rect>::ToV8(v8::Isolate* isolate,
                                                const gfx::Rect& val) {
  mate::Dictionary dict = mate::Dictionary::CreateEmpty(isolate);
  dict.SetHidden("simple", true);
  dict.Set("x", val.x());
  dict.Set("y", val.y());
  dict.Set("width", val.width());
  dict.Set("height", val.height());
  return dict.GetHandle();
}

bool Converter<gfx::Rect>::FromV8(v8::Isolate* isolate,
                                  v8::Local<v8::Value> val,
                                  gfx::Rect* out) {
  mate::Dictionary dict;
  if (!ConvertFromV8(isolate, val, &dict))
    return false;
  int x, y, width, height;
  if (!dict.Get("x", &x) || !dict.Get("y", &y) || !dict.Get("width", &width) ||
      !dict.Get("height", &height))
    return false;
  *out = gfx::Rect(x, y, width, height);
  return true;
}

template <>
struct Converter<display::Display::AccelerometerSupport> {
  static v8::Local<v8::Value> ToV8(
      v8::Isolate* isolate,
      const display::Display::AccelerometerSupport& val) {
    switch (val) {
      case display::Display::AccelerometerSupport::AVAILABLE:
        return StringToV8(isolate, "available");
      case display::Display::AccelerometerSupport::UNAVAILABLE:
        return StringToV8(isolate, "unavailable");
      default:
        return StringToV8(isolate, "unknown");
    }
  }
};

template <>
struct Converter<display::Display::TouchSupport> {
  static v8::Local<v8::Value> ToV8(v8::Isolate* isolate,
                                   const display::Display::TouchSupport& val) {
    switch (val) {
      case display::Display::TouchSupport::AVAILABLE:
        return StringToV8(isolate, "available");
      case display::Display::TouchSupport::UNAVAILABLE:
        return StringToV8(isolate, "unavailable");
      default:
        return StringToV8(isolate, "unknown");
    }
  }
};

v8::Local<v8::Value> Converter<display::Display>::ToV8(
    v8::Isolate* isolate,
    const display::Display& val) {
  mate::Dictionary dict = mate::Dictionary::CreateEmpty(isolate);
  dict.SetHidden("simple", true);
  dict.Set("id", val.id());
  dict.Set("bounds", val.bounds());
  dict.Set("workArea", val.work_area());
  dict.Set("accelerometerSupport", val.accelerometer_support());
  dict.Set("monochrome", val.is_monochrome());
  dict.Set("colorDepth", val.color_depth());
  dict.Set("colorSpace", val.color_space().ToString());
  dict.Set("depthPerComponent", val.depth_per_component());
  dict.Set("size", val.size());
  dict.Set("workAreaSize", val.work_area_size());
  dict.Set("scaleFactor", val.device_scale_factor());
  dict.Set("rotation", val.RotationAsDegree());
  dict.Set("internal", val.IsInternal());
  dict.Set("touchSupport", val.touch_support());
  return dict.GetHandle();
}

}  // namespace mate
