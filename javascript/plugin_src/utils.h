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

using namespace v8;

// Reads a file into a v8 string.
MaybeLocal<String>
ReadFile(Isolate* isolate, const std::string& name)
{
    FILE* file = fopen(name.c_str(), "rb");
    if (file == NULL)
        return MaybeLocal<String>();

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    std::unique_ptr<char> chars(new char[size + 1]);
    chars.get()[size] = '\0';
    for (size_t i = 0; i < size;)
    {
        i += fread(&chars.get()[i], 1, size - i, file);
        if (ferror(file))
        {
            fclose(file);
            return MaybeLocal<String>();
        }
    }
    fclose(file);
    MaybeLocal<String> result = String::NewFromUtf8(
        isolate, chars.get(), NewStringType::kNormal, static_cast<int>(size));
    return result;
}

// Convert a JavaScript string to a std::string.  To not bother too
// much with string encodings we just use ascii.
std::string
ObjectToString(v8::Isolate* isolate, Local<Value> value)
{
    String::Utf8Value utf8_value(isolate, value);
    return std::string(*utf8_value);
}