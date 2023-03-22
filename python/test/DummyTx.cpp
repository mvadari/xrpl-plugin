#include <ripple/app/tx/impl/DummyTx.h>
#include <ripple/app/tx/impl/Transactor.h>
#include <iostream>

#include <pybind11/embed.h> // everything needed for embedding
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

namespace ripple {

NotTEC
DummyTx::preflight(PreflightContext const& ctx)
{
    
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("/Users/mvadari/Documents/plugin_transactor/python/test");
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
    py::module_::import("sys").attr("path").attr("append")("/Users/mvadari/Documents/plugin_transactor/python/test");
    py::object preclaim = py::module_::import("dummy_tx").attr("preclaim");
    py::object preclaimReturn = preclaim(py::cast(ctx, py::return_value_policy::reference));
    return TER::fromInt(preclaimReturn.cast<int>());
}

TER
DummyTx::doApply()
{
    py::scoped_interpreter guard{}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("/Users/mvadari/Documents/plugin_transactor/python/test");
    py::object doApplyFn = py::module_::import("dummy_tx").attr("doApply");
    py::object doApplyReturn = doApplyFn(py::cast(ctx_, py::return_value_policy::reference));
    return TER::fromInt(doApplyReturn.cast<int>());
}
}

extern "C"
ripple::NotTEC
preflight(ripple::PreflightContext const& ctx)
{
    return ripple::DummyTx::preflight(ctx);
}

extern "C"
ripple::TER
preclaim(ripple::PreclaimContext const& ctx)
{
    return ripple::DummyTx::preclaim(ctx);
}

extern "C"
ripple::XRPAmount
calculateBaseFee(ripple::ReadView const& view, ripple::STTx const& tx)
{
    std::cout << "AAAAAAAAAAAAAAAAAAAAcalculateBaseFee" << std::endl;
    return ripple::DummyTx::calculateBaseFee(view, tx);
}

extern "C"
std::pair<ripple::TER, bool>
apply(ripple::ApplyContext& ctx)
{
    ripple::DummyTx p(ctx);
    return p();
}