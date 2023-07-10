#include <map>
#include <string>
#include <dlfcn.h>
#include <ripple/json/json_reader.h>
#include <ripple/plugin/exports.h>
#include <ripple/protocol/Serializer.h>

using namespace ripple;

std::map<int, STypeExport> sTypeMap{};

STypeExport
getSTypeExport(int type)
{
    if (auto const it = sTypeMap.find(type); it != sTypeMap.end())
    {
        return it->second;
    }
    throw std::runtime_error(
        "Type " + std::to_string(type) + " does not exist");
}

int
main(int argc, char* argv[])
{
    if (argc < 4) {
        std::cout << "Usage: ./helper /path/to/plugin SFieldName element" << std::endl;
        return 1;
    }
    char const* dylib = argv[1];
    std::string fieldName = argv[2];
    std::string initialElement = argv[3];

    Json::Value jv;
    Json::Reader reader;
    std::string fakeJson = "{\"" + fieldName + "\": " + initialElement + "}";
    if (!reader.parse(fakeJson, jv)) {
        std::cout << "Couldn't parse " << fakeJson << " as JSON" << std::endl;
        std::cout << "Note: strings need to be written in the command line as '\"string\"'" << std::endl;
        return 1;
    }
    Json::Value element = jv[fieldName.c_str()];

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
        sTypeMap.insert({stype.typeId, stype});
    }
    auto const sFields = ((getSFieldsPtr)dlsym(handle, "getSFields"))();
    for (int i = 0; i < sFields.size; i++)
    {
        auto const sField = *(sFields.data + i);
        registerSField(sField);
    }

    auto const& field = SField::getField(fieldName);
    int const type = field.fieldType;
    auto const functions = getSTypeExport(type);
    
    Json::Value error;
    // use the parser to get a buffer
    Buffer buf = functions.parsePtr(field, "test", fieldName, &field, element, error);
    if (error) {
        std::cout << "Error in parsing: " << error.asString() << std::endl;
        return 1;
    }
    std::cout << "Parsed buffer: " << strHex(buf) << std::endl;
    // pass the buffer into the serializer
    Serializer s;
    functions.toSerializer(type, buf, s);
    std::cout << "Serializer: " << strHex(s).substr(2, strHex(s).length() - 2) << std::endl;
    // pass the Serializer data into the SerialIter
    Slice const slice = s.slice();
    std::cout << "Serializer Slice: " << strHex(slice).substr(2, strHex(slice).length() - 2) << std::endl;
    SerialIter* si = new SerialIter(slice);
    // can't print the contents of SerialIter without removing them from the iterator
    std::cout << "Created SerialIter" << std::endl;
    // fromSerialIter
    Buffer end = functions.fromSerialIter(type, *si);
    std::cout << "End buffer: " << strHex(end) << std::endl;

    dlclose(handle);
}
