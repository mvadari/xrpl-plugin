#include <boost/python.hpp>
#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/protocol/STTx.h>

using namespace boost::python;
using namespace ripple;

BOOST_PYTHON_MODULE(pylib)
{
    // class_<STTx>("STTx")
    //   .def("getTransactionID", &STTx::getTransactionID);
}
