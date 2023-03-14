import sys
import importlib

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
    if (auto const ret = preflight1(ctx); !isTesSuccess(ret))
        return ret;  // LCOV_EXCL_LINE

    if (ctx.tx.getFlags() & tfUniversalMask)
        return temINVALID_FLAG;
    
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive

    py::print("Hello, World!"); // use the Python API
    py::object pythonPlugin = py::module_::import("{module_name}");
    py::print(pythonPlugin);
    py::object preflight = pythonPlugin.attr("preflight");
    py::print(preflight);
    py::print(ctx);
    preflight(ctx);

    return preflight2(ctx);
}}

TER
{tx_name}::preclaim(PreclaimContext const& ctx)
{{
    return tesSUCCESS;
}}

TER
{tx_name}::doApply()
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::print("Hello, World!"); // use the Python API
    py::object pythonPlugin = py::module_::import("{module_name}");
    py::print(ctx_.tx);
    py::print(py::cast(ctx_, py::return_value_policy::reference));
    py::print(pythonPlugin);
    py::object pythonDoApply = pythonPlugin.attr("doApply");
    py::print(pythonDoApply);
    pythonDoApply(py::cast(ctx_, py::return_value_policy::reference));

    return tesSUCCESS;
}}
}}"""


def create_files(python_file):
    sys.path.append(python_file)
    last_slash = python_file.rfind("/")
    module_name = python_file[(last_slash+1):-3]
    module = importlib.import_module(module_name)
    tx_name = module.tx_name
    print(tx_name)
    with open(f"{tx_name}.h", "w") as f:
        f.write(generate_header(tx_name))
    
    with open(f"{tx_name}.cpp", "w") as f:
        f.write(generate_cpp(tx_name, module_name))


if __name__ == "__main__":
    create_files(sys.argv[1])
