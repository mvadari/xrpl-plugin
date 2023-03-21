import os
import sys
import importlib
# import tempfile

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


def generate_cpp(tx_name, module_name): 
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
    py::object preclaim = py::module_::import("{module_name}").attr("preclaim");
    py::object preclaimReturn = preclaim(py::cast(ctx, py::return_value_policy::reference));
    return TER::fromInt(preclaimReturn.cast<int>());
}}

TER
{tx_name}::doApply()
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object doApplyFn = py::module_::import("{module_name}").attr("doApply");
    py::object doApplyReturn = doApplyFn(py::cast(ctx_, py::return_value_policy::reference));
    return TER::fromInt(doApplyReturn.cast<int>());
}}
}}"""


def create_files(python_file):
    abs_python_file = os.path.abspath(python_file)
    sys.path.append(os.path.dirname(abs_python_file))
    last_slash = abs_python_file.rfind("/")
    module_name = abs_python_file[(last_slash+1):-3]
    module = importlib.import_module(module_name)
    tx_name = module.tx_name
    # TODO: add logic to check validity of Python transactors
    # TODO: switch to Jinja
    # TODO: add logic to only generate the methods that exist in Python

    # temp_dir = tempfile.TemporaryDirectory()

    with open(f"{tx_name}.h", "w") as f:
        f.write(generate_header(tx_name))

    with open(f"{tx_name}.cpp", "w") as f:
        f.write(generate_cpp(tx_name, module_name))
    
    return os.path.abspath(f"{tx_name}.cpp", module_name)

def build_files(cpp_file, project_name):
    pass


if __name__ == "__main__":
    create_files(sys.argv[1])
