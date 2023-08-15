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

#include "base.h"

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

// -------------------------
// ---- Global Elements ----
// -------------------------

void
Log(const char* event)
{
    printf("Logged from JS: %s\n", event);
}

void
LogJSON(const FunctionCallbackInfo<Value>& args)
{
    if (args.Length() < 1)
        return;
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Value> arg = args[0];
    if (arg->IsObject())
    {
        MaybeLocal<String> jsonOutput = JSON::Stringify(
            isolate->GetCurrentContext(),
            arg->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        if (jsonOutput.IsEmpty())
        {
            String::Utf8Value jsonOutput(isolate, arg);
            Log(*jsonOutput);
        }
        else
        {
            String::Utf8Value value(isolate, jsonOutput.ToLocalChecked());
            Log(*value);
        }
    }
    else
    {
        String::Utf8Value value(isolate, arg);
        Log(*value);
    }
}

void
LogCallback(const FunctionCallbackInfo<Value>& args)
{
    if (args.Length() < 1) {
        return;
    }

    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);

    Local<Value> arg = args[0];
    String::Utf8Value value(isolate, arg);

    if (*value == "[object Object]")
    {
        LogJSON(args);
    }

    printf("Logged from JS: %s\n", *value);
}

void
preflight1(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1)
        throw std::runtime_error("preflight1 expects 1 argument");
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Value> arg = args[0];
    if (arg->IsObject())
    {
        ripple::PreflightContext& ctx = *Unwrap<ripple::PreflightContext>(
            arg->ToObject(isolate->GetCurrentContext()).ToLocalChecked());
        ripple::NotTEC preflight1ret = ripple::preflight1(ctx);
        int returnCode = Json::Value{preflight1ret}
                             .asInt();  // TODO: get TERtoInt to work here
        args.GetReturnValue().Set(returnCode);
    }
    else
    {
        throw std::runtime_error("preflight1 expects PreflightContext");
    }
}

void
isTesSuccess(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    if (args.Length() != 1)
        throw std::runtime_error("isTesSuccess expects 1 argument");
    Isolate* isolate = args.GetIsolate();
    HandleScope scope(isolate);
    Local<Value> arg = args[0];
    if (arg->IsInt32())
    {
        int argValue = arg->ToInt32(isolate->GetCurrentContext())
                           .ToLocalChecked()
                           ->Value();

        bool retValue = ripple::isTesSuccess(
            ripple::TER::fromInt(argValue));  // TODO: get TERtoInt to work here
        args.GetReturnValue().Set(retValue);
    }
    else
    {
        throw std::runtime_error("isTesSuccess expects int");
    }
}

#define GET_GLOBAL_VARIABLE(Variable) \
    [](Local<String> name, const PropertyCallbackInfo<Value>& info) { \
        SetReturnValue(info, Variable); \
    }

Local<ObjectTemplate>
getGlobals(Isolate* isolate)
{
    Local<ObjectTemplate> global = ObjectTemplate::New(isolate);
    global->Set(isolate, "log", FunctionTemplate::New(isolate, LogCallback));
    global->Set(
        isolate, "preflight1", FunctionTemplate::New(isolate, preflight1));
    global->Set(
        isolate, "isTesSuccess", FunctionTemplate::New(isolate, isTesSuccess));
    global->SetAccessor(
        String::NewFromUtf8Literal(isolate, "fixMasterKeyAsRegularKey", NewStringType::kInternalized),
        GET_GLOBAL_VARIABLE(ripple::fixMasterKeyAsRegularKey));
    return global;
}