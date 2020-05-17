// Copyright (c) 2017 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_UI_WIN_ATOM_DESKTOP_NATIVE_WIDGET_AURA_H_
#define ATOM_BROWSER_UI_WIN_ATOM_DESKTOP_NATIVE_WIDGET_AURA_H_

#include "atom/browser/native_window_views.h"
#include "ui/views/widget/desktop_aura/desktop_native_widget_aura.h"

namespace views {
class DesktopWindowTreeHost;
}

namespace atom {

class AtomDesktopNativeWidgetAura : public views::DesktopNativeWidgetAura {
 public:
  explicit AtomDesktopNativeWidgetAura(NativeWindowViews* native_window_view);

  // views::DesktopNativeWidgetAura:
  void InitNativeWidget(const views::Widget::InitParams& params) override;

  // internal::NativeWidgetPrivate:
  void Activate() override;

 private:
  void OnWindowActivated(wm::ActivationChangeObserver::ActivationReason reason,
                         aura::Window* gained_active,
                         aura::Window* lost_active) override;

  NativeWindowViews* native_window_view_;

  // Owned by DesktopNativeWidgetAura.
  views::DesktopWindowTreeHost* desktop_window_tree_host_;

  DISALLOW_COPY_AND_ASSIGN(AtomDesktopNativeWidgetAura);
};

}  // namespace atom

#endif  // ATOM_BROWSER_UI_WIN_ATOM_DESKTOP_NATIVE_WIDGET_AURA_H_
