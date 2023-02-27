// #include <Python.h>
// #include <boost/python.hpp>
// #include <ripple/app/tx/impl/Transactor.h>
// #include <ripple/protocol/STTx.h>

/*
Application& app;
STTx const& tx;
Rules const rules;
ApplyFlags flags;
beast::Journal const j;
*/


// using namespace boost::python;

// BOOST_PYTHON_MODULE(pylib)
// {
//     class_<ripple::STTx>("STTx", no_init)
//       .def("getTransactionID", &ripple::STTx::getTransactionID)
//     ;
// }

#include <boost/python.hpp>
#include <boost/python/list.hpp>
#include <boost/python/extract.hpp>
#include <string>
#include <sstream>
#include <vector>

struct World
{
    void set(std::string msg) { mMsg = msg; }
    void many(boost::python::list msgs) {
        long l = len(msgs);
        std::stringstream ss;
        for (long i = 0; i<l; ++i) {
            if (i>0) ss << ", ";
            std::string s = boost::python::extract<std::string>(msgs[i]);
            ss << s;
        }
        mMsg = ss.str();
    }
    std::string greet() { return mMsg; }
    std::string mMsg;
};

using namespace boost::python;

BOOST_PYTHON_MODULE(classes)
{
    class_<World>("World")
        .def("greet", &World::greet)
        .def("set", &World::set)
        .def("many", &World::many)
    ;
};