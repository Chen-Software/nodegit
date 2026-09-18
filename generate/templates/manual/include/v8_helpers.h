#ifndef NODEGIT_V8_HELPERS_H
#define NODEGIT_V8_HELPERS_H

#include <napi.h>
#include <string>

namespace nodegit {
  Napi::Value safeGetField(Napi::Object containerObj, std::string fieldName);
  inline Napi::Value ToV8(Napi::Env env, int value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, unsigned int value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, int64_t value) { return Napi::Number::New(env, static_cast<double>(value)); }
  inline Napi::Value ToV8(Napi::Env env, uint64_t value) { return Napi::Number::New(env, static_cast<double>(value)); }
  inline Napi::Value ToV8(Napi::Env env, double value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, float value) { return Napi::Number::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, bool value) { return Napi::Boolean::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, const char *value) { return value ? Napi::String::New(env, value) : env.Null(); }
}

#endif
