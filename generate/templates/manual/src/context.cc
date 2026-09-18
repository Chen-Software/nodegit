#include "../include/context.h"

namespace nodegit {
  std::map<v8::Isolate *, Context *> Context::contexts;

  AsyncContextCleanupHandle::AsyncContextCleanupHandle(v8::Isolate *isolate, Context *context)
    : context(context),
      handle(node::AddEnvironmentCleanupHook(isolate, AsyncCleanupContext, this))
  {}

  AsyncContextCleanupHandle::~AsyncContextCleanupHandle() {
    delete context;
    doneCallback(doneData);
  }

  void AsyncContextCleanupHandle::AsyncCleanupContext(void *data, void(*uvCallback)(void*), void *uvCallbackData) {
    std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle(static_cast<AsyncContextCleanupHandle *>(data));
    cleanupHandle->doneCallback = uvCallback;
    cleanupHandle->doneData = uvCallbackData;
    // the ordering of std::move and the call to Context::ShutdownThreadPool prohibits
    // us from referring to context on cleanupHandle if we're also intending to move
    // the unique_ptr into the method.
    Context *context = cleanupHandle->context;
    context->ShutdownThreadPool(std::move(cleanupHandle));
  }

  Context::Context(Napi::Env env)
    : env(env),
      isolate(v8::Isolate::GetCurrent()),
      threadPool(10, node::GetCurrentEventLoop(isolate), this)
  {
    Napi::Object storage = Napi::Object::New(env);
    persistentStorage = Napi::Persistent(storage);
    contexts[isolate] = this;
    new AsyncContextCleanupHandle(isolate, this);
  }

  Context::~Context() {
    nodegit::TrackerWrap::DeleteFromList(&trackerList);
    contexts.erase(isolate);
  }

  std::shared_ptr<CleanupHandle> Context::GetCleanupHandle(std::string key) {
    return cleanupHandles[key];
  }

  Context *Context::GetCurrentContext() {
    v8::Isolate *isolate = v8::Isolate::GetCurrent();
    return contexts[isolate];
  }

  Napi::Value Context::GetFromPersistent(std::string key) {
    Napi::Object storage = persistentStorage.Value();
    return storage.Get(key);
  }

  void Context::QueueWorker(nodegit::AsyncWorker *worker) {
    threadPool.QueueWorker(worker);
  }

  std::shared_ptr<CleanupHandle> Context::RemoveCleanupHandle(std::string key) {
    std::shared_ptr<CleanupHandle> cleanupItem = cleanupHandles[key];
    cleanupHandles.erase(key);
    return cleanupItem;
  }

  void Context::SaveToPersistent(std::string key, const Napi::Value &value) {
    Napi::Object storage = persistentStorage.Value();
    storage.Set(key, value);
  }

  void Context::SaveCleanupHandle(std::string key, std::shared_ptr<CleanupHandle> cleanupItem) {
    cleanupHandles[key] = cleanupItem;
  }

  void Context::ShutdownThreadPool(std::unique_ptr<AsyncContextCleanupHandle> cleanupHandle) {
    threadPool.Shutdown(std::move(cleanupHandle));
  }
}
