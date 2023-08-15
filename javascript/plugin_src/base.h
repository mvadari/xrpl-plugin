#include <stdlib.h>
#include <string.h>

#include <map>
#include <string>
#include <type_traits>

#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/basics/Log.h>
#include <ripple/basics/XRPAmount.h>
#include <ripple/ledger/ApplyView.h>
#include <ripple/ledger/View.h>
#include <ripple/plugin/exports.h>
#include <ripple/protocol/Feature.h>
#include <ripple/protocol/TER.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/st.h>
#include "include/libplatform/libplatform.h"
#include "include/v8-array-buffer.h"
#include "include/v8-context.h"
#include "include/v8-exception.h"
#include "include/v8-external.h"
#include "include/v8-function.h"
#include "include/v8-initialization.h"
#include "include/v8-isolate.h"
#include "include/v8-json.h"
#include "include/v8-local-handle.h"
#include "include/v8-object.h"
#include "include/v8-persistent-handle.h"
#include "include/v8-primitive.h"
#include "include/v8-script.h"
#include "include/v8-snapshot.h"
#include "include/v8-template.h"
#include "include/v8-value.h"

#include "utils.h"

using std::map;
using std::pair;
using std::string;
using std::vector;

using v8::Context;
using v8::EscapableHandleScope;
using v8::External;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Global;
using v8::HandleScope;
using v8::Isolate;
using v8::JSON;
using v8::Local;
using v8::MaybeLocal;
using v8::Name;
using v8::NamedPropertyHandlerConfiguration;
using v8::NewStringType;
using v8::Object;
using v8::ObjectTemplate;
using v8::PropertyCallbackInfo;
using v8::Script;
using v8::String;
using v8::TryCatch;
using v8::Value;

/**
 * Utility function that extracts the C++ http request object from a
 * wrapper object.
 */
template <typename T>
T*
Unwrap(Local<Object> obj)
{
    Local<External> field = obj->GetInternalField(0).As<External>();
    void* ptr = field->Value();
    return static_cast<T*>(ptr);
}

template <typename T>
void
MakeTemplate(Isolate* isolate, Local<ObjectTemplate>& result){}

template <typename T>
void
AddProperty(
    T const& object,
    Local<Object>& result,
    const PropertyCallbackInfo<Value>& info)
{
    // Wrap the raw C++ pointer in an External so it can be referenced
    // from within JavaScript.
    Local<External> ptr = External::New(info.GetIsolate(), (void*)&object);
    result->SetInternalField(0, ptr);
}

template <typename T>
void
AddProperty(
    T const& object,
    Local<Object>& result,
    const FunctionCallbackInfo<Value>& info)
{
    // Wrap the raw C++ pointer in an External so it can be referenced
    // from within JavaScript.
    Local<External> ptr = External::New(info.GetIsolate(), (void*)&object);
    result->SetInternalField(0, ptr);
}

template <typename T>
Local<ObjectTemplate>
BuildTemplate(Isolate* isolate)
{
    EscapableHandleScope handle_scope(isolate);

    Local<ObjectTemplate> initial_template = ObjectTemplate::New(isolate);
    initial_template->SetInternalFieldCount(1);

    MakeTemplate<T>(isolate, initial_template);

    // Again, return the template through the current handle scope.
    return handle_scope.Escape(initial_template);
}

template <typename T>
Local<ObjectTemplate>
GetTemplate(Isolate* isolate)
{
    // Fetch the template for creating JavaScript wrappers.
    Local<ObjectTemplate> raw_template = BuildTemplate<T>(isolate);
    Global<ObjectTemplate> object_template;
    object_template.Reset(isolate, raw_template);
    return Local<ObjectTemplate>::New(isolate, object_template);
}

template <typename T>
void
SetReturnValue(const PropertyCallbackInfo<Value>& info, T& value)
{
    Local<ObjectTemplate> templ = GetTemplate<T>(info.GetIsolate());
    Local<Object> result =
        templ->NewInstance(info.GetIsolate()->GetCurrentContext())
            .ToLocalChecked();
    AddProperty(value, result, info);
    info.GetReturnValue().Set(result);
}

template <typename T>
void
SetReturnValue(const PropertyCallbackInfo<Value>& info, T const& value)
{
    Local<ObjectTemplate> templ = GetTemplate<T>(info.GetIsolate());
    Local<Object> result =
        templ->NewInstance(info.GetIsolate()->GetCurrentContext())
            .ToLocalChecked();
    AddProperty(value, result, info);
    info.GetReturnValue().Set(result);
}

template <>
void
SetReturnValue<std::string>(const PropertyCallbackInfo<Value>& info, std::string& value)
{
    info.GetReturnValue().Set(String::NewFromUtf8(
                                  info.GetIsolate(),
                                  value.c_str(),
                                  NewStringType::kNormal,
                                  static_cast<int>(value.length()))
                                  .ToLocalChecked());
}

template <>
void
SetReturnValue<int>(const PropertyCallbackInfo<Value>& info, int& value)
{
    info.GetReturnValue().Set(value);
}

template <typename T>
void
SetReturnValue(const FunctionCallbackInfo<Value>& info, T& value)
{
    Local<ObjectTemplate> templ = GetTemplate<T>(info.GetIsolate());
    Local<Object> result =
        templ->NewInstance(info.GetIsolate()->GetCurrentContext())
            .ToLocalChecked();
    AddProperty(value, result, info);
    info.GetReturnValue().Set(result);
}

template <typename T>
void
SetReturnValue(const FunctionCallbackInfo<Value>& info, T const& value)
{
    Local<ObjectTemplate> templ = GetTemplate<T>(info.GetIsolate());
    Local<Object> result =
        templ->NewInstance(info.GetIsolate()->GetCurrentContext())
            .ToLocalChecked();
    AddProperty(value, result, info);
    info.GetReturnValue().Set(result);
}

template <>
void
SetReturnValue<std::string>(const FunctionCallbackInfo<Value>& info, std::string& value)
{
    info.GetReturnValue().Set(String::NewFromUtf8(
                                  info.GetIsolate(),
                                  value.c_str(),
                                  NewStringType::kNormal,
                                  static_cast<int>(value.length()))
                                  .ToLocalChecked());
}

template <>
void
SetReturnValue<std::string>(const FunctionCallbackInfo<Value>& info, std::string const& value)
{
    info.GetReturnValue().Set(String::NewFromUtf8(
                                  info.GetIsolate(),
                                  value.c_str(),
                                  NewStringType::kNormal,
                                  static_cast<int>(value.length()))
                                  .ToLocalChecked());
}

template <>
void
SetReturnValue<std::uint32_t>(const FunctionCallbackInfo<Value>& info, std::uint32_t const& value)
{
    info.GetReturnValue().Set(value);
}

#define GET_PROPERTY(Parent, Member) \
    [](Local<String> name, const PropertyCallbackInfo<Value>& info) { \
        auto& parent = *Unwrap<Parent>(info.Holder()); \
        auto const& member = parent.Member; \
        SetReturnValue(info, member); \
    }

#define GET_PROPERTY_LAMBDA(Parent, Lambda) \
    [](Local<String> name, const PropertyCallbackInfo<Value>& info) { \
        auto& parent = *Unwrap<Parent>(info.Holder()); \
        auto const& member = Lambda(parent); \
        SetReturnValue(info, member); \
    }

#define GET_INSTANCE_METHOD(Parent, function, numParameters) \
    [](const FunctionCallbackInfo<Value>& info) { \
        if (info.Length() != numParameters) throw std::runtime_error("function expects 0 arguments"); \
        auto& parent = *Unwrap<Parent>(info.Holder()); \
        SetReturnValue(info, parent.function()); \
    }

#define GET_INSTANCE_METHOD_LAMBDA(Parent, lambda, numParameters) \
    [](const FunctionCallbackInfo<Value>& info) { \
        if (info.Length() != numParameters) throw std::runtime_error("function expects 0 arguments"); \
        auto& parent = *Unwrap<Parent>(info.Holder()); \
        SetReturnValue(info, lambda(parent)); \
    }


template <>
void
MakeTemplate<ripple::STBase>(Isolate* isolate, Local<ObjectTemplate>& result)
{   
    result->Set(isolate, "toString", FunctionTemplate::New(
        isolate,
        GET_INSTANCE_METHOD(ripple::STTx, getFullText, 0)));
}

template <>
void
MakeTemplate<ripple::STObject>(Isolate* isolate, Local<ObjectTemplate>& result)
{   
    MakeTemplate<ripple::STBase>(isolate, result);
    result->Set(isolate, "getFlags", FunctionTemplate::New(
        isolate,
        GET_INSTANCE_METHOD(ripple::STTx, getFlags, 0)));
}


template <>
void
MakeTemplate<ripple::STTx>(Isolate* isolate, Local<ObjectTemplate>& result)
{
    MakeTemplate<ripple::STObject>(isolate, result);
}

template <>
void
MakeTemplate<ripple::PreflightContext>(Isolate* isolate, Local<ObjectTemplate>& result)
{
    // Add accessors for each of the fields of the PreflightContext.
    result->SetAccessor(
        String::NewFromUtf8Literal(isolate, "tx", NewStringType::kInternalized),
        GET_PROPERTY(ripple::PreflightContext, tx));
}

template <>
void
MakeTemplate<ripple::PreclaimContext>(Isolate* isolate, Local<ObjectTemplate>& result)
{
    // Add accessors for each of the fields of the PreclaimContext.
    result->SetAccessor(
        String::NewFromUtf8Literal(isolate, "tx", NewStringType::kInternalized),
        GET_PROPERTY(ripple::PreclaimContext, tx));
}

template <>
void
MakeTemplate<ripple::ApplyContext>(Isolate* isolate, Local<ObjectTemplate>& result)
{
    // Add accessors for each of the fields of the ApplyContext.
    result->SetAccessor(
        String::NewFromUtf8Literal(isolate, "tx", NewStringType::kInternalized),
        GET_PROPERTY(ripple::ApplyContext, tx));
}

template <>
void
MakeTemplate<ripple::XRPAmount>(Isolate* isolate, Local<ObjectTemplate>& result)
{}

template <>
void
MakeTemplate<ripple::uint256>(Isolate* isolate, Local<ObjectTemplate>& result)
{
    // Add accessors for each of the fields of the ApplyContext.
    result->Set(isolate, "toString", FunctionTemplate::New(
        isolate,
        GET_INSTANCE_METHOD_LAMBDA(
            ripple::uint256,
            [](const ripple::uint256 &uint256) {
                return ripple::to_string(uint256);
            },
            0)));
}
