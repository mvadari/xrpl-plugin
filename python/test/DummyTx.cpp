#include <ripple/app/tx/impl/DummyTx.h>
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

namespace ripple {

NotTEC
DummyTx::preflight(PreflightContext const& ctx)
{
    
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::object preflight = py::module_::import("dummy_tx").attr("preflight");
    py::object preflightReturn = preflight(py::cast(ctx, py::return_value_policy::reference));
    try {
        return NotTEC::fromInt(preflightReturn.cast<int>());
    } catch (const py::cast_error &) { // TODO: figure out the exact error that is thrown
        return preflightReturn.cast<NotTEC>();
    }
}

TER
DummyTx::preclaim(PreclaimContext const& ctx)
{
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::object preclaim = py::module_::import("dummy_tx").attr("preclaim");
    py::object preclaimReturn = preclaim(py::cast(ctx, py::return_value_policy::reference));
    return TER::fromInt(preclaimReturn.cast<int>());
}

TER
DummyTx::doApply()
{
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::object doApplyFn = py::module_::import("dummy_tx").attr("doApply");
    py::object doApplyReturn = doApplyFn(py::cast(ctx_, py::return_value_policy::reference));
    return TER::fromInt(doApplyReturn.cast<int>());
}
}