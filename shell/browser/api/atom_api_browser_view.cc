// Copyright (c) 2017 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "shell/browser/api/atom_api_browser_view.h"

#include "native_mate/constructor.h"
#include "native_mate/dictionary.h"
#include "shell/browser/api/atom_api_web_contents.h"
#include "shell/browser/browser.h"
#include "shell/browser/native_browser_view.h"
#include "shell/common/color_util.h"
#include "shell/common/native_mate_converters/gfx_converter.h"
#include "shell/common/native_mate_converters/value_converter.h"
#include "shell/common/node_includes.h"
#include "shell/common/options_switches.h"
#include "ui/gfx/geometry/rect.h"

namespace mate {

template <>
struct Converter<electron::AutoResizeFlags> {
  static bool FromV8(v8::Isolate* isolate,
                     v8::Local<v8::Value> val,
                     electron::AutoResizeFlags* auto_resize_flags) {
    mate::Dictionary params;
    if (!ConvertFromV8(isolate, val, &params)) {
      return false;
    }

    uint8_t flags = 0;
    bool width = false;
    if (params.Get("width", &width) && width) {
      flags |= electron::kAutoResizeWidth;
    }
    bool height = false;
    if (params.Get("height", &height) && height) {
      flags |= electron::kAutoResizeHeight;
    }
    bool horizontal = false;
    if (params.Get("horizontal", &horizontal) && horizontal) {
      flags |= electron::kAutoResizeHorizontal;
    }
    bool vertical = false;
    if (params.Get("vertical", &vertical) && vertical) {
      flags |= electron::kAutoResizeVertical;
    }

    *auto_resize_flags = static_cast<electron::AutoResizeFlags>(flags);
    return true;
  }
};

}  // namespace mate

namespace electron {

namespace api {

BrowserView::BrowserView(v8::Isolate* isolate,
                         v8::Local<v8::Object> wrapper,
                         const mate::Dictionary& options) {
  Init(isolate, wrapper, options);
}

void BrowserView::Init(v8::Isolate* isolate,
                       v8::Local<v8::Object> wrapper,
                       const mate::Dictionary& options) {
  mate::Dictionary web_preferences = mate::Dictionary::CreateEmpty(isolate);
  options.Get(options::kWebPreferences, &web_preferences);
  web_preferences.Set("type", "browserView");
  mate::Handle<class WebContents> web_contents =
      WebContents::Create(isolate, web_preferences);

  web_contents_.Reset(isolate, web_contents.ToV8());
  api_web_contents_ = web_contents.get();
  Observe(web_contents->web_contents());

  view_.reset(
      NativeBrowserView::Create(api_web_contents_->managed_web_contents()));

  InitWith(isolate, wrapper);
}

BrowserView::~BrowserView() {
  if (api_web_contents_) {  // destroy() is called
    // Destroy WebContents asynchronously unless app is shutting down,
    // because destroy() might be called inside WebContents's event handler.
    api_web_contents_->DestroyWebContents(!Browser::Get()->is_shutting_down());
  }
}

void BrowserView::WebContentsDestroyed() {
  api_web_contents_ = nullptr;
  web_contents_.Reset();
}

// static
mate::WrappableBase* BrowserView::New(mate::Arguments* args) {
  if (!Browser::Get()->is_ready()) {
    args->ThrowError("Cannot create BrowserView before app is ready");
    return nullptr;
  }

  mate::Dictionary options = mate::Dictionary::CreateEmpty(args->isolate());
  args->GetNext(&options);

  return new BrowserView(args->isolate(), args->GetThis(), options);
}

int32_t BrowserView::ID() const {
  return weak_map_id();
}

void BrowserView::SetAutoResize(AutoResizeFlags flags) {
  view_->SetAutoResizeFlags(flags);
}

void BrowserView::SetBounds(const gfx::Rect& bounds) {
  view_->SetBounds(bounds);
}

gfx::Rect BrowserView::GetBounds() {
  return view_->GetBounds();
}

void BrowserView::SetBackgroundColor(const std::string& color_name) {
  view_->SetBackgroundColor(ParseHexColor(color_name));
}

v8::Local<v8::Value> BrowserView::GetWebContents() {
  if (web_contents_.IsEmpty()) {
    return v8::Null(isolate());
  }

  return v8::Local<v8::Value>::New(isolate(), web_contents_);
}

// static
void BrowserView::BuildPrototype(v8::Isolate* isolate,
                                 v8::Local<v8::FunctionTemplate> prototype) {
  prototype->SetClassName(mate::StringToV8(isolate, "BrowserView"));
  gin_helper::Destroyable::MakeDestroyable(isolate, prototype);
  mate::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
      .SetMethod("setAutoResize", &BrowserView::SetAutoResize)
      .SetMethod("setBounds", &BrowserView::SetBounds)
      .SetMethod("getBounds", &BrowserView::GetBounds)
      .SetMethod("setBackgroundColor", &BrowserView::SetBackgroundColor)
      .SetProperty("webContents", &BrowserView::GetWebContents)
      .SetProperty("id", &BrowserView::ID);
}

}  // namespace api

}  // namespace electron

namespace {

using electron::api::BrowserView;

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  v8::Isolate* isolate = context->GetIsolate();
  BrowserView::SetConstructor(isolate, base::BindRepeating(&BrowserView::New));

  mate::Dictionary browser_view(isolate, BrowserView::GetConstructor(isolate)
                                             ->GetFunction(context)
                                             .ToLocalChecked());
  browser_view.SetMethod("fromId",
                         &mate::TrackableObject<BrowserView>::FromWeakMapID);
  browser_view.SetMethod("getAllViews",
                         &mate::TrackableObject<BrowserView>::GetAll);
  mate::Dictionary dict(isolate, exports);
  dict.Set("BrowserView", browser_view);
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(atom_browser_browser_view, Initialize)
