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
    if (argc < 2) {
        std::cout << "Usage: ./helper /path/to/plugin" << std::endl;
        return 1;
    }

    // open dylib
    char const* dylib = argv[1];
    void* handle = dlopen(dylib, RTLD_LAZY);

    // print SType info
    if (dlsym(handle, "getSTypes") != NULL)
    {
        auto const sTypes = ((getSTypesPtr)dlsym(handle, "getSTypes"))();
        std::cout << "SType Info: ";
        for (int i = 0; i < sTypes.size; i++)
        {
            auto const stype = *(sTypes.data + i);
            std::cout << stype.typeId << " ";
        }
        std::cout << std::endl;
    }

    // print SField info
    if (dlsym(handle, "getSFields") != NULL)
    {
        auto const sFields = ((getSFieldsPtr)dlsym(handle, "getSFields"))();
        std::cout << "SField Info:"  << std::endl;
        for (int i = 0; i < sFields.size; i++)
        {
            auto const sField = *(sFields.data + i);
            std::cout << "  " << sField.txtName << ": " << "(type " << sField.typeId << ", value " << sField.fieldValue << ")" << std::endl;
        }
    }

    dlclose(handle);
}
