import os
import sys
import importlib
import subprocess
import tempfile


def generate_header(tx_name):
    return f"""//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.
    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.
    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#ifndef RIPPLE_TX_{tx_name.upper()}_H_INCLUDED
#define RIPPLE_TX_{tx_name.upper()}_H_INCLUDED

#include <ripple/app/tx/impl/Transactor.h>

namespace ripple {{

class {tx_name} : public Transactor
{{
public:
    static constexpr ConsequencesFactoryType ConsequencesFactory{{Normal}};

    explicit {tx_name}(ApplyContext& ctx) : Transactor(ctx)
    {{
    }}

    static NotTEC
    preflight(PreflightContext const& ctx);

    static TER
    preclaim(PreclaimContext const& ctx);

    static TER
    doApply(ApplyContext& ctx, XRPAmount mPriorBalance, XRPAmount mSourceBalance);
}};

//------------------------------------------------------------------------------

}}  // namespace ripple

#endif"""


def generate_cpp(tx_name, tx_type, module_name, python_folder):
    return f"""#include "{tx_name}.h"
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
#include <map>

#include <pybind11/embed.h> // everything needed for embedding
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

using namespace ripple;

struct WSF {{
  void *f_;
  operator SField const &() const {{
    return *static_cast<SField *>(f_);
  }};
}};

extern "C"
NotTEC
preflight(PreflightContext const& ctx)
{{

    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module module = py::module_::import("{module_name}");
    if (!hasattr(module, "preflight")) {{
        return tesSUCCESS;
    }}
    py::object preflight = module.attr("preflight");
    try {{
        py::object preflightReturn = preflight(py::cast(ctx, py::return_value_policy::reference));
        try {{
            return preflightReturn.cast<NotTEC>();
        }} catch (const py::cast_error &) {{ // TODO: figure out the exact error that is thrown
            return NotTEC::fromInt(preflightReturn.cast<int>());
        }}
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

extern "C"
TER
preclaim(PreclaimContext const& ctx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module module = py::module_::import("{module_name}");
    if (!hasattr(module, "preclaim")) {{
        return tesSUCCESS;
    }}
    py::object preclaim = module.attr("preclaim");
    try {{
        py::object preclaimReturn = preclaim(py::cast(ctx, py::return_value_policy::reference));
        return TER::fromInt(preclaimReturn.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

extern "C"
TER
doApply(ApplyContext& ctx, XRPAmount mPriorBalance, XRPAmount mSourceBalance)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module module = py::module_::import("{module_name}");
    if (!hasattr(module, "do_apply")) {{
        return tesSUCCESS;
    }}
    py::object doApplyFn = module.attr("do_apply");
    try {{
        py::object doApplyReturn = doApplyFn(
            py::cast(ctx, py::return_value_policy::reference),
            mPriorBalance,
            mSourceBalance);
        return TER::fromInt(doApplyReturn.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

extern "C"
char const*
getTxName()
{{
    static std::string const txName = []{{
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::object tx_name = py::module_::import("{module_name}").attr("tx_name");
        return tx_name.cast<std::string>();
    }}();
    return txName.c_str();
}}

extern "C"
std::uint16_t
getTxType()
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::object tx_type = py::module_::import("{module_name}").attr("tx_type");
    return tx_type.cast<std::uint16_t>();
}}

extern "C"
std::string
getTTName()
{{
    return "tt{module_name.upper()}";
}}

extern "C"
std::vector<SOElementExport>
getTxFormat()
{{
    static std::vector<SOElementExport> const txFormat = []{{
        std::vector<SOElementExport> temp = {{}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        auto tx_format = py::module_::import("{module_name}").attr("tx_format").cast<std::vector<py::object>>();
        for (py::object variable: tx_format)
        {{
            py::tuple tup = variable.cast<py::tuple>();
            WSF sfield = tup[0].cast<WSF>();
            SOEStyle varType = tup[1].cast<SOEStyle>();
            temp.emplace_back(SOElementExport{{static_cast<SField const&>(sfield).getCode(), varType}});
        }}
        return temp;
    }}();
    return txFormat;
}}

static std::map<int, std::string> parseSTypeFunctions;

Buffer
parseLeafTypePython(
    SField const& field,
    std::string const& json_name,
    std::string const& fieldName,
    SField const* name,
    Json::Value const& value,
    Json::Value& error)
{{
    if (auto it = parseSTypeFunctions.find(field.fieldType);
        it != parseSTypeFunctions.end())
    {{
        WSF wrappedField = WSF{{(void *)&field}};
        WSF wrappedName = WSF{{(void *)name}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        try {{
            py::module_::import("sys").attr("path").attr("append")("{python_folder}");
            py::object parseFn = py::module_::import("{module_name}").attr(it->second.c_str());
            py::object returnObj = parseFn(
                py::cast(wrappedField, py::return_value_policy::reference),
                json_name,
                fieldName,
                wrappedName,
                value);
            py::tuple tup = returnObj.cast<py::tuple>();
            if (!tup[1].is_none())
            {{
                error = tup[1].cast<Json::Value>();
                Buffer ret;
                return ret;
            }}
            auto const stVar = tup[0].cast<Buffer>();
            return tup[0].cast<Buffer>();
        }} catch (py::error_already_set& e) {{
            // Print the error message
            const char* errorMessage = e.what();
            std::cout << "Python Error: " << errorMessage << std::endl;
            throw;
        }}
    }}

    Buffer ret;
    error = unknown_type(json_name, fieldName, field.fieldType);
    return ret;
}}


extern "C"
std::vector<STypeExport>
getSTypes()
{{
    static std::vector<STypeExport> const sTypes = []{{
        std::vector<STypeExport> temp = {{}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "new_stypes")) {{
            return temp;
        }}
        auto new_stypes = pluginImport.attr("new_stypes").cast<std::vector<py::object>>();
        for (py::object variable: new_stypes)
        {{
            py::tuple tup = variable.cast<py::tuple>();
            int typeId = tup[0].cast<int>();
            py::function parseFn = tup[1].cast<py::function>();
            parseSTypeFunctions.insert({{typeId, parseFn.attr("__name__").cast<std::string>()}});
            temp.emplace_back(STypeExport{{typeId, parseLeafTypePython}});
        }}
        return temp;
    }}();
    return sTypes;
}}


extern "C"
std::vector<SFieldExport>
getSFields()
{{
    static std::vector<SFieldExport> const sFields = []{{
        std::vector<SFieldExport> temp = {{}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "new_sfields")) {{
            return temp;
        }}
        auto new_sfields = pluginImport.attr("new_sfields").cast<std::vector<py::object>>();
        for (py::object variable: new_sfields)
        {{
            WSF wrappedSField = variable.cast<WSF>();
            SField const& sfield = static_cast<SField const&>(wrappedSField);
            temp.emplace_back(SFieldExport{{
                sfield.fieldType,
                sfield.fieldValue,
                sfield.jsonName}});
        }}
        return temp;
    }}();
    return sFields;
}}

typedef std::int64_t (*visitEntryXRPChangePtr)(
    bool isDelete,
    std::shared_ptr<SLE const> const& entry,
    bool isBefore);

static std::map<int, std::string> visitEntryXRPChangeFunctions;

std::int64_t visitEntryXRPChange(
    bool isDelete,
    std::shared_ptr<SLE const> const& entry,
    bool isBefore)
{{
    if (auto it = visitEntryXRPChangeFunctions.find(entry->getType());
        it != visitEntryXRPChangeFunctions.end())
    {{
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        try {{
            py::module_::import("sys").attr("path").attr("append")("{python_folder}");
            py::object visitFn = py::module_::import("{module_name}").attr(it->second.c_str());
            py::object returnObj = visitFn(
                isDelete,
                py::cast(entry, py::return_value_policy::reference),
                isBefore);
            return returnObj.cast<std::int64_t>();
        }} catch (py::error_already_set& e) {{
            // Print the error message
            const char* errorMessage = e.what();
            std::cout << "Python Error: " << errorMessage << std::endl;
            throw;
        }}
    }}
}}

struct LedgerObjectExportInternal {{
    std::uint16_t type;
    std::string name; // CamelCase
    std::string rpcName; // snake_case
    std::vector<SOElementExport> format;
    bool isDeletionBlocker;
    DeleterFuncPtr deleteFn;
    visitEntryXRPChangePtr visitEntryXRPChange;
}};

LedgerObjectExport
mutate(LedgerObjectExportInternal const& obj)
{{
    return LedgerObjectExport{{
        obj.type,
        obj.name.c_str(),
        obj.rpcName.c_str(),
        {{const_cast<SOElementExport *>(obj.format.data()), static_cast<int>(obj.format.size())}},
        obj.isDeletionBlocker,
        obj.deleteFn,
        obj.visitEntryXRPChange,
    }};
}}


extern "C"
std::vector<LedgerObjectExport>
getLedgerObjects()
{{
    static std::vector<LedgerObjectExportInternal> const objects = []{{
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module_ module = py::module_::import("{module_name}");
        std::vector<LedgerObjectExportInternal> temp = {{}};
        if (!hasattr(module, "new_ledger_objects")) {{
            return temp;
        }}
        auto objects = module.attr("new_ledger_objects").cast<std::vector<py::object>>();
        for (py::object object: objects)
        {{
            std::vector<SOElementExport> objectFormat{{}};
            auto pythonObjectFormat = object.attr("object_format").cast<std::vector<py::object>>();
            for (py::object variable: pythonObjectFormat)
            {{
                py::tuple tup = variable.cast<py::tuple>();
                WSF sfield = tup[0].cast<WSF>();
                SOEStyle varType = tup[1].cast<SOEStyle>();
                objectFormat.emplace_back(SOElementExport{{
                    static_cast<SField const&>(sfield).getCode(),
                    varType}});
            }}
            auto const objectType = object.attr("object_type").cast<std::uint16_t>();
            py::function visitFn = object.attr("visit_entry_xrp_change").cast<py::function>();
            visitEntryXRPChangeFunctions.insert({{objectType, visitFn.attr("__name__").cast<std::string>()}});
            temp.emplace_back(LedgerObjectExportInternal{{
                objectType,
                object.attr("object_name").cast<std::string>(),
                object.attr("object_rpc_name").cast<std::string>(),
                std::move(objectFormat),
                object.attr("is_deletion_blocker").cast<bool>(),
                nullptr,  // TODO: fix
                visitEntryXRPChange}});
        }}
        return temp;
    }}();
    static std::vector<LedgerObjectExport> output;
    output.reserve(objects.size());
    std::transform(objects.begin(), objects.end(), std::back_inserter(output), mutate);

    return output;
}}
"""


def create_files(python_file):
    abs_python_file = os.path.abspath(python_file)
    python_folder = os.path.dirname(abs_python_file)
    sys.path.append(python_folder)
    last_slash = abs_python_file.rfind("/")
    module_name = abs_python_file[(last_slash+1):-3]
    module = importlib.import_module(module_name)
    tx_name = module.tx_name
    tx_type = module.tx_type
    # TODO: add logic to check validity of Python transactors
    # TODO: switch to Jinja
    # TODO: add logic to only generate the methods that exist in Python

    with open(f"{tx_name}.h", "w") as f:
        f.write(generate_header(tx_name))

    with open(f"{tx_name}.cpp", "w") as f:
        f.write(generate_cpp(tx_name, tx_type, module_name, python_folder))
    
    return os.path.abspath(f"{tx_name}.cpp"), module_name


def build_files(cpp_file, project_name):
    with tempfile.TemporaryDirectory() as build_temp:
        build_temp = "./build/test"
        build_source_dir = os.path.dirname(__file__)
        conan_source_dir = os.path.dirname(build_source_dir)
        conan_build_dir = os.path.join(conan_source_dir, "build", "generators")
        cmake_args = []
        build_args = []
        subprocess.run([
            "conan",
            "install",
            conan_source_dir,
            "--build",
            "missing",
            "--settings",
            "build_type=Debug"
        ], check=True, cwd=build_temp, stdout=subprocess.DEVNULL)
        conan_cmake_file = os.path.join(conan_build_dir, "conan_toolchain.cmake")
        cmake_args += [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={os.getcwd()}{os.sep}",
            f"-DPROJECT_NAME={project_name}",
            f"-DSOURCE_FILE={cpp_file}",
            f"-DCMAKE_TOOLCHAIN_FILE:FILEPATH={conan_cmake_file}",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DBUILD_SHARED_LIBS=ON",
        ]
        subprocess.run(
            ["cmake", build_source_dir] + cmake_args, cwd=build_temp, check=True, stdout=subprocess.DEVNULL
        )
        subprocess.run(
            ["cmake", "--build", "."] + build_args, cwd=build_temp, check=True, stdout=subprocess.DEVNULL
        )


if __name__ == "__main__":
    cpp_file_fullpath, module_name = create_files(sys.argv[1])
    build_files(cpp_file_fullpath, module_name)
