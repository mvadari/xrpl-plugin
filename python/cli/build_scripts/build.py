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

    TER
    doApply() override;
}};

//------------------------------------------------------------------------------

}}  // namespace ripple

#endif"""


def generate_cpp(tx_name, module_name, python_folder): 
    return f"""#include <ripple/app/tx/impl/{tx_name}.h>
#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/basics/Log.h>
#include <ripple/basics/XRPAmount.h>
#include <ripple/ledger/ApplyView.h>
#include <ripple/ledger/View.h>
#include <ripple/protocol/Feature.h>
#include <ripple/protocol/TER.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/st.h>

#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

namespace ripple {{

NotTEC
{tx_name}::preflight(PreflightContext const& ctx)
{{
    
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::object preflight = py::module_::import("{module_name}").attr("preflight");
    py::object preflightReturn = preflight(py::cast(ctx, py::return_value_policy::reference));
    try {{
        return NotTEC::fromInt(preflightReturn.cast<int>());
    }} catch (const py::cast_error &) {{ // TODO: figure out the exact error that is thrown
        return preflightReturn.cast<NotTEC>();
    }}
}}

TER
{tx_name}::preclaim(PreclaimContext const& ctx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::object preclaim = py::module_::import("{module_name}").attr("preclaim");
    py::object preclaimReturn = preclaim(py::cast(ctx, py::return_value_policy::reference));
    return TER::fromInt(preclaimReturn.cast<int>());
}}

TER
{tx_name}::doApply()
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::object doApplyFn = py::module_::import("{module_name}").attr("doApply");
    py::object doApplyReturn = doApplyFn(py::cast(ctx_, py::return_value_policy::reference));
    return TER::fromInt(doApplyReturn.cast<int>());
}}
}}"""


def create_files(python_file):
    abs_python_file = os.path.abspath(python_file)
    python_folder = os.path.dirname(abs_python_file)
    sys.path.append(python_folder)
    last_slash = abs_python_file.rfind("/")
    module_name = abs_python_file[(last_slash+1):-3]
    module = importlib.import_module(module_name)
    tx_name = module.tx_name
    # TODO: add logic to check validity of Python transactors
    # TODO: switch to Jinja
    # TODO: add logic to only generate the methods that exist in Python


    with open(f"{tx_name}.h", "w") as f:
        f.write(generate_header(tx_name))

    with open(f"{tx_name}.cpp", "w") as f:
        f.write(generate_cpp(tx_name, module_name, python_folder))
    
    return os.path.abspath(f"{tx_name}.cpp"), module_name


def build_files(cpp_file, project_name):
    with tempfile.TemporaryDirectory() as build_temp:
        build_source_dir = os.path.dirname(__file__)
        cmake_args = []
        build_args = []
        subprocess.run([
            "conan",
            "install",
            build_source_dir,
            "--output-folder",
            str(build_temp),
            "--build",
            "missing",
            "--settings",
            "build_type=Debug"
        ], check=True, stdout=subprocess.DEVNULL)
        conan_cmake_file = os.path.join(build_temp, "build/generators/conan_toolchain.cmake")
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
