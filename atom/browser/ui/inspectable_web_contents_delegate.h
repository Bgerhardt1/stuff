// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Copyright (c) 2013 Adam Roben <adam@roben.org>. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE-CHROMIUM file.

#ifndef ATOM_BROWSER_UI_INSPECTABLE_WEB_CONTENTS_DELEGATE_H_
#define ATOM_BROWSER_UI_INSPECTABLE_WEB_CONTENTS_DELEGATE_H_

#include <string>

#include "base/files/file_path.h"

namespace atom {

class InspectableWebContentsDelegate {
 public:
  virtual ~InspectableWebContentsDelegate() {}

  // Requested by WebContents of devtools.
  virtual void DevToolsReloadPage() {}
  virtual void DevToolsSaveToFile(const std::string& url,
                                  const std::string& content,
                                  bool save_as) {}
  virtual void DevToolsAppendToFile(const std::string& url,
                                    const std::string& content) {}
  virtual void DevToolsRequestFileSystems() {}
  virtual void DevToolsAddFileSystem(const std::string& type,
                                     const base::FilePath& file_system_path) {}
  virtual void DevToolsRemoveFileSystem(
      const base::FilePath& file_system_path) {}
  virtual void DevToolsIndexPath(int request_id,
                                 const std::string& file_system_path,
                                 const std::string& excluded_folders) {}
  virtual void DevToolsStopIndexing(int request_id) {}
  virtual void DevToolsSearchInPath(int request_id,
                                    const std::string& file_system_path,
                                    const std::string& query) {}
};

}  // namespace atom

#endif  // ATOM_BROWSER_UI_INSPECTABLE_WEB_CONTENTS_DELEGATE_H_
