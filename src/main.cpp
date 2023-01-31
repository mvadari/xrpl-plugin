#include <vector>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
// #include <ripple/app/tx/impl/Transactor.h>
#include <ripple/protocol/STTx.h>

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

PYBIND11_MODULE(plugin_transactor, m) {
    py::class_<STTx> STTx(m, "STTx");
        .def("getTransactionID", &STTx::getTransactionID);
}