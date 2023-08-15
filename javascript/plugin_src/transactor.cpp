// Copyright 2012 the V8 project authors. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials provided
//       with the distribution.
//     * Neither the name of Google Inc. nor the names of its
//       contributors may be used to endorse or promote products derived
//       from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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

#include "globals.h"

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
// --- P r o c e s s o r ---
// -------------------------

/**
 * An http request processor that is scriptable using JavaScript.
 */
class JsProcessor
{
public:
    // Creates a new processor that processes requests by invoking the
    // Process function of the JavaScript script given as an argument.
    JsProcessor(Isolate* isolate, Local<String> script)
        : isolate_(isolate), script_(script)
    {
    }
    ~JsProcessor();

    bool
    Initialize(std::string functionName);

    template <typename T>
    T
    getReturn(Local<Value>& result);

    template <typename... Args>
    auto
    wrappedFunction(Args&&... args);

private:
    // Execute the script associated with this processor and extract the
    // Process function.  Returns true if this succeeded, otherwise false.
    bool
    ExecuteScript(Local<String> script);

    Isolate*
    GetIsolate()
    {
        return isolate_;
    }

    // Utility methods for wrapping C++ objects as JavaScript objects,
    // and going back again.
    template <typename T>
    Local<Object>
    Wrap(T const& ctx);
    // Local<Object> WrapPreclaimRequest(PreclaimContext const& ctx);

    Isolate* isolate_;
    Local<String> script_;
    Global<Context> context_;
    Global<Function> process_;
};

// Execute the script and fetch the preflight method.
bool
JsProcessor::Initialize(std::string functionName)
{
    // Create a handle scope to hold the temporary references.
    HandleScope handle_scope(GetIsolate());

    // Create a template for the global object where we set the
    // built-in global functions.
    Local<ObjectTemplate> global = getGlobals(GetIsolate());

    // Each processor gets its own context so different processors don't
    // affect each other. Context::New returns a persistent handle which
    // is what we need for the reference to remain after we return from
    // this method. That persistent handle has to be disposed in the
    // destructor.
    v8::Local<v8::Context> context = Context::New(GetIsolate(), NULL, global);
    context_.Reset(GetIsolate(), context);

    // Enter the new context so all the following operations take place
    // within it.
    Context::Scope context_scope(context);

    // Compile and run the script
    if (!ExecuteScript(script_))
        return false;

    // The script compiled and ran correctly.  Now we fetch out the
    // preflight function from the global object.
    Local<String> process_name = String::NewFromUtf8(
                                     GetIsolate(),
                                     functionName.c_str(),
                                     v8::NewStringType::kNormal,
                                     functionName.length())
                                     .ToLocalChecked();
    Local<Value> process_val;
    // If there is no preflight function, or if it is not a function,
    // bail out
    if (!context->Global()->Get(context, process_name).ToLocal(&process_val) ||
        !process_val->IsFunction())
    {
        return false;
    }

    // It is a function; cast it to a Function
    Local<Function> process_fun = process_val.As<Function>();

    // Store the function in a Global handle, since we also want
    // that to remain after this call returns
    process_.Reset(GetIsolate(), process_fun);

    // All done; all went well
    return true;
}

bool
JsProcessor::ExecuteScript(Local<String> script)
{
    HandleScope handle_scope(GetIsolate());

    // We're just about to compile the script; set up an error handler to
    // catch any exceptions the script might throw.
    TryCatch try_catch(GetIsolate());

    Local<Context> context(GetIsolate()->GetCurrentContext());

    // Compile the script and check for errors.
    Local<Script> compiled_script;
    if (!Script::Compile(context, script).ToLocal(&compiled_script))
    {
        String::Utf8Value error(GetIsolate(), try_catch.Exception());
        Log(*error);
        // The script failed to compile; bail out.
        return false;
    }

    // Run the script!
    Local<Value> result;
    if (!compiled_script->Run(context).ToLocal(&result))
    {
        // The TryCatch above is still in effect and will have caught the error.
        String::Utf8Value error(GetIsolate(), try_catch.Exception());
        Log(*error);
        // Running the script failed; bail out.
        return false;
    }

    return true;
}

template <>
ripple::NotTEC
JsProcessor::getReturn<ripple::NotTEC>(Local<Value>& result)
{
    if (result->IsInt32())
    {
        int resultValue = result->ToInt32(isolate_->GetCurrentContext())
                              .ToLocalChecked()
                              ->Value();
        return ripple::NotTEC::fromInt(resultValue);
    }
    else
    {
        throw std::runtime_error("Expected to return int that becomes NotTEC");
    }
}

template <>
int
JsProcessor::getReturn<int>(Local<Value>& result)
{
    if (result->IsInt32())
    {
        int resultValue = result->ToInt32(isolate_->GetCurrentContext())
                              .ToLocalChecked()
                              ->Value();
        return resultValue;
    }
    else
    {
        throw std::runtime_error("Expected to return int");
    }
}

template <typename... Args>
auto
JsProcessor::wrappedFunction(Args&&... args)
{
    Isolate* isolate = GetIsolate();
    HandleScope handle_scope(isolate);

    v8::Local<v8::Context> context =
        v8::Local<v8::Context>::New(isolate, context_);

    // Enter this processor's context so all the remaining operations
    // take place there
    Context::Scope context_scope(context);

    // Set up an exception handler before calling the Process function
    TryCatch try_catch(isolate);

    // Invoke the process function, giving the global object as 'this'
    // and one argument, the request.
    const int argc = sizeof...(args);
    Local<Value> argv[argc];
    int argNum = 0;

    (
        [&] {
            argv[argNum] = {Wrap(args)};
            ++argNum;
        }(),
        ...);

    Local<Function> process = Local<Function>::New(isolate, process_);
    Local<Value> result;
    if (!process->Call(context, context->Global(), argc, argv).ToLocal(&result))
    {
        String::Utf8Value error(isolate, try_catch.Exception());
        Log(*error);
        return ripple::NotTEC(ripple::temUNKNOWN);
    }

    return getReturn<decltype(wrappedFunction(args...))>(result);
}

JsProcessor::~JsProcessor()
{
    // Dispose the persistent handles.  When no one else has any
    // references to the objects stored in the handles they will be
    // automatically reclaimed.
    context_.Reset();
    process_.Reset();
}

// -------------------------------------------
// --- A c c e s s i n g   R e q u e s t s ---
// -------------------------------------------

/**
 * Utility function that wraps a C++ http request object in a
 * JavaScript object.
 */
template <typename T>
Local<Object>
JsProcessor::Wrap(T const& ctx)
{
    // Local scope for temporary handles.
    EscapableHandleScope handle_scope(GetIsolate());
    // auto wrapper = test::GetPreflightContext();
    // Local<ObjectTemplate> templ = wrapper.MakeTemplate(GetIsolate());
    Local<ObjectTemplate> templ = GetTemplate<T>(GetIsolate());

    // Create an empty http request wrapper.
    Local<Object> result =
        templ->NewInstance(GetIsolate()->GetCurrentContext()).ToLocalChecked();

    // Wrap the raw C++ pointer in an External so it can be referenced
    // from within JavaScript.
    Local<External> ptr = External::New(GetIsolate(), (void*)&ctx);

    // Store the request pointer in the JavaScript wrapper.
    result->SetInternalField(0, ptr);

    // Return the result through the current handle scope.  Since each
    // of these handles will go away when the handle scope is deleted
    // we need to call Close to let one, the result, escape into the
    // outer handle scope.
    return handle_scope.Escape(result);
}

// --- Test ---

auto static const filename = "/Users/mvadari/Documents/v8-cmake/transactor.js";
static const std::unique_ptr<v8::Platform> v8Platform =
    v8::platform::NewDefaultPlatform();
static bool initialized = false;

void
initPlatform()
{
    if (initialized)
        return;
    v8::V8::InitializePlatform(v8Platform.get());
    v8::V8::Initialize();
    initialized = true;
}

template <typename... Args>
auto
ProcessJSFunction(
    v8::Isolate* isolate,
    v8::Platform* platform,
    std::string funcName,
    Args&&... args)
{
    Isolate::Scope isolate_scope(isolate);
    HandleScope scope(isolate);
    Local<String> source;
    if (!ReadFile(isolate, filename).ToLocal(&source))
    {
        fprintf(stderr, "Error reading '%s'.\n", filename);
        return ripple::NotTEC(ripple::temUNKNOWN);
    }
    JsProcessor processor(isolate, source);
    if (!processor.Initialize(funcName))
    {
        fprintf(stderr, "Error initializing processor.\n");
        return ripple::NotTEC(ripple::temUNKNOWN);
    }
    auto result = (processor.wrappedFunction)(args...);
    while (v8::platform::PumpMessageLoop(platform, isolate))
        continue;
    return result;
}

template <typename... Args>
auto
JSFunctionHandler(std::string funcName, Args&&... args)
{
    // set up external file
    v8::V8::InitializeICUDefaultLocation(filename);
    v8::V8::InitializeExternalStartupData(filename);
    initPlatform();

    Isolate::CreateParams create_params;
    create_params.array_buffer_allocator =
        v8::ArrayBuffer::Allocator::NewDefaultAllocator();
    Isolate* isolate = Isolate::New(create_params);
    auto retValue =
        ProcessJSFunction(isolate, v8Platform.get(), funcName, args...);

    isolate->Dispose();

    return retValue;
}

using namespace ripple;

NotTEC
preflight(PreflightContext const& ctx)
{
    return JSFunctionHandler("preflight", ctx);
}

TER
preclaim(PreclaimContext const& ctx)
{
    return JSFunctionHandler("preclaim", ctx);
}

TER
doApply(ApplyContext& ctx, XRPAmount mPriorBalance, XRPAmount mSourceBalance)
{
    return JSFunctionHandler("doApply", ctx, mPriorBalance, mSourceBalance);
}

extern "C" Container<TransactorExport>
getTransactors()
{
    static SOElementExport format[] = {
        {sfRegularKey.getCode(), soeOPTIONAL},
        {sfTicketSequence.getCode(), soeOPTIONAL},
    };
    SOElementExport* formatPtr = format;
    static TransactorExport list[] = {
        {"DummyTx",
         50,
         {formatPtr, 2},
         Transactor::ConsequencesFactoryType::Normal,
         NULL,
         NULL,
         preflight,
         preclaim,
         doApply,
         NULL,
         NULL,
         NULL,
         NULL}};
    TransactorExport* ptr = list;
    return {ptr, 1};
}

static uint256 pluginAmendment;

extern "C" Container<AmendmentExport>
getAmendments()
{
    AmendmentExport const amendment = {
        "featurePluginTest",
        true,
        VoteBehavior::DefaultNo,
    };
    pluginAmendment = registerPluginAmendment(amendment);
    static AmendmentExport list[] = {amendment};
    AmendmentExport* ptr = list;
    return {ptr, 1};
}
