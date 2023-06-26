#include <string>
#include <dlfcn.h>
#include <ripple/plugin/exports.h>
#include <ripple/protocol/Serializer.h>

using namespace ripple;

int main(int argc, char* argv[]) {
    char const* dylib = "/Users/mvadari/Documents/plugin_transactor/cpp/build/libplugin_transactor.dylib";
    auto const element = "100";

    void* handle = dlopen(dylib, RTLD_LAZY);
    auto const sTypes = ((getSTypesPtr)dlsym(handle, "getSTypes"))();
    for (int i = 0; i < sTypes.size; i++)
    {
        auto const stype = *(sTypes.data + i);
        registerSType(
            {stype.typeId,
                stype.toString,
                stype.toJson,
                stype.toSerializer,
                stype.fromSerialIter});
        registerLeafType(stype.typeId, stype.parsePtr);
    }
    auto const sFields = ((getSFieldsPtr)dlsym(handle, "getSFields"))();
    for (int i = 0; i < sFields.size; i++)
    {
        auto const sField = *(sFields.data + i);
        registerSField(sField);
    }

    // get the SType from the dylib
    auto const stype = *(sTypes.data);
    // use the parser to get a buffer
    std::string fieldName = "QualityIn2";
    auto const& field = SField::getField(fieldName);
    Json::Value error;
    Buffer buf = stype.parsePtr(field, "test", fieldName, &field, element, error);
    std::cout << "Parsed buffer: " << strHex(buf) << std::endl;
    // pass the buffer into the serializer
    Serializer s;
    stype.toSerializer(buf, s);
    std::cout << "Serializer: " << strHex(s) << std::endl;
    // pass the Serializer data into the SerialIter
    Slice const slice = s.slice();
    std::cout << "Serializer Slice: " << strHex(slice) << std::endl;
    SerialIter* si = new SerialIter(slice);
    std::cout << "Created SerialIter" << std::endl;
    // std::cout << "SerialIter: " << strHex(si->getBitString()) << std::endl;
    // fromSerialIter
    Buffer end = stype.fromSerialIter(*si);
    std::cout << "End buffer: " << strHex(end) << std::endl;

    dlclose(handle);
}