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
    class_<STTx>("STTx")
      .def("getTransactionID", &STTx::getTransactionID)
}