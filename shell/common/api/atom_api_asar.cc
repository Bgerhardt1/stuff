// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include <stddef.h>

#include <vector>

#include "native_mate/arguments.h"
#include "native_mate/dictionary.h"
#include "native_mate/object_template_builder.h"
#include "native_mate/wrappable.h"
#include "shell/common/asar/archive.h"
#include "shell/common/native_mate_converters/callback.h"
#include "shell/common/native_mate_converters/file_path_converter.h"
#include "shell/common/node_includes.h"
#include "third_party/electron_node/src/node_native_module.h"

namespace {

class Archive : public mate::Wrappable<Archive> {
 public:
  static v8::Local<v8::Value> Create(v8::Isolate* isolate,
                                     const base::FilePath& path) {
    auto archive = std::make_unique<asar::Archive>(path);
    if (!archive->Init())
      return v8::False(isolate);
    return (new Archive(isolate, std::move(archive)))->GetWrapper();
  }

  static void BuildPrototype(v8::Isolate* isolate,
                             v8::Local<v8::FunctionTemplate> prototype) {
    prototype->SetClassName(mate::StringToV8(isolate, "Archive"));
    mate::ObjectTemplateBuilder(isolate, prototype->PrototypeTemplate())
        .SetProperty("path", &Archive::GetPath)
        .SetMethod("getFileInfo", &Archive::GetFileInfo)
        .SetMethod("stat", &Archive::Stat)
        .SetMethod("readdir", &Archive::Readdir)
        .SetMethod("realpath", &Archive::Realpath)
        .SetMethod("copyFileOut", &Archive::CopyFileOut)
        .SetMethod("getFd", &Archive::GetFD);
  }

 protected:
  Archive(v8::Isolate* isolate, std::unique_ptr<asar::Archive> archive)
      : archive_(std::move(archive)) {
    Init(isolate);
  }

  // Returns the path of the file.
  base::FilePath GetPath() { return archive_->path(); }

  // Reads the offset and size of file.
  v8::Local<v8::Value> GetFileInfo(v8::Isolate* isolate,
                                   const base::FilePath& path) {
    asar::Archive::FileInfo info;
    if (!archive_ || !archive_->GetFileInfo(path, &info))
      return v8::False(isolate);
    mate::Dictionary dict(isolate, v8::Object::New(isolate));
    dict.Set("size", info.size);
    dict.Set("unpacked", info.unpacked);
    dict.Set("offset", info.offset);
    return dict.GetHandle();
  }

  // Returns a fake result of fs.stat(path).
  v8::Local<v8::Value> Stat(v8::Isolate* isolate, const base::FilePath& path) {
    asar::Archive::Stats stats;
    if (!archive_ || !archive_->Stat(path, &stats))
      return v8::False(isolate);
    mate::Dictionary dict(isolate, v8::Object::New(isolate));
    dict.Set("size", stats.size);
    dict.Set("offset", stats.offset);
    dict.Set("isFile", stats.is_file);
    dict.Set("isDirectory", stats.is_directory);
    dict.Set("isLink", stats.is_link);
    return dict.GetHandle();
  }

  // Returns all files under a directory.
  v8::Local<v8::Value> Readdir(v8::Isolate* isolate,
                               const base::FilePath& path) {
    std::vector<base::FilePath> files;
    if (!archive_ || !archive_->Readdir(path, &files))
      return v8::False(isolate);
    return mate::ConvertToV8(isolate, files);
  }

  // Returns the path of file with symbol link resolved.
  v8::Local<v8::Value> Realpath(v8::Isolate* isolate,
                                const base::FilePath& path) {
    base::FilePath realpath;
    if (!archive_ || !archive_->Realpath(path, &realpath))
      return v8::False(isolate);
    return mate::ConvertToV8(isolate, realpath);
  }

  // Copy the file out into a temporary file and returns the new path.
  v8::Local<v8::Value> CopyFileOut(v8::Isolate* isolate,
                                   const base::FilePath& path) {
    base::FilePath new_path;
    if (!archive_ || !archive_->CopyFileOut(path, &new_path))
      return v8::False(isolate);
    return mate::ConvertToV8(isolate, new_path);
  }

  // Return the file descriptor.
  int GetFD() const {
    if (!archive_)
      return -1;
    return archive_->GetFD();
  }

 private:
  std::unique_ptr<asar::Archive> archive_;

  DISALLOW_COPY_AND_ASSIGN(Archive);
};

void InitAsarSupport(v8::Isolate* isolate, v8::Local<v8::Value> require) {
  // Evaluate asar_init.js.
  std::vector<v8::Local<v8::String>> asar_init_params = {
      node::FIXED_ONE_BYTE_STRING(isolate, "require")};
  std::vector<v8::Local<v8::Value>> asar_init_args = {require};
  node::per_process::native_module_loader.CompileAndCall(
      isolate->GetCurrentContext(), "electron/js2c/asar_init",
      &asar_init_params, &asar_init_args, nullptr);
}

void Initialize(v8::Local<v8::Object> exports,
                v8::Local<v8::Value> unused,
                v8::Local<v8::Context> context,
                void* priv) {
  mate::Dictionary dict(context->GetIsolate(), exports);
  dict.SetMethod("createArchive", &Archive::Create);
  dict.SetMethod("initAsarSupport", &InitAsarSupport);
}

}  // namespace

NODE_LINKED_MODULE_CONTEXT_AWARE(atom_common_asar, Initialize)
