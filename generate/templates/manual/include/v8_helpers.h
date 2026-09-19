#ifndef NODEGIT_V8_HELPERS_H
#define NODEGIT_V8_HELPERS_H

#include <napi.h>
#include <string>
#include <type_traits>

namespace nodegit {
  Napi::Value safeGetField(Napi::Object containerObj, std::string fieldName);

  template <typename T, typename std::enable_if<std::is_arithmetic<T>::value && !std::is_same<T, bool>::value, int>::type = 0>
  inline Napi::Value ToV8(Napi::Env env, T value) {
    return Napi::Number::New(env, static_cast<double>(value));
  }

  inline Napi::Value ToV8(Napi::Env env, bool value) { return Napi::Boolean::New(env, value); }
  inline Napi::Value ToV8(Napi::Env env, const char *value) { return value ? Napi::String::New(env, value) : env.Null(); }
}

#endif
