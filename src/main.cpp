#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

#include "main.h"

#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)

namespace py = pybind11;

Class::Class() {

}

Class2::Class2() {

}

Class* c1 = new Class();
const Class* Class2::var = c1;
Class2* c = new Class2();


PYBIND11_MODULE(plugin_transactor, m) {
    py::class_<Class> Class(m, "Class");
    py::class_<Class2> Class2(m, "Class2");
    Class2
        .def_readonly_static("var", &Class2::var, py::return_value_policy::reference);
}
