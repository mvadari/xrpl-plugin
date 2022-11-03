#include <Python.h>
#include <boost/python.hpp>
#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/protocol/STTx.h>

/*
Application& app;
STTx const& tx;
Rules const rules;
ApplyFlags flags;
beast::Journal const j;
*/


using namespace boost::python;

BOOST_PYTHON_MODULE(pylib)
{
    class_<ripple::STTx>("STTx", no_init)
      .def("getTransactionID", &ripple::STTx::getTransactionID)
    ;
}