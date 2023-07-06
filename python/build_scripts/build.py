import os
import sys
import subprocess
import tempfile


def generate_cpp(tx_name, module_name, python_folder):
    return f"""#include <ripple/app/tx/impl/Transactor.h>
#include <ripple/basics/Log.h>
#include <ripple/basics/XRPAmount.h>
#include <ripple/ledger/ApplyView.h>
#include <ripple/ledger/View.h>
#include <ripple/plugin/exports.h>
#include <ripple/protocol/Feature.h>
#include <ripple/protocol/TER.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/st.h>
#include <map>

#include <pybind11/embed.h> // everything needed for embedding
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

using namespace ripple;

struct WSF {{
  void *f_;
  operator SField const &() const {{
    return *static_cast<SField *>(f_);
  }};
}};

// ----------------------------------------------------------------------------
// Transactors
// ----------------------------------------------------------------------------

static std::map<std::uint16_t, int> txTypeToIndexMap;

py::object
getTransactorFromPython(std::uint16_t txType)
{{
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module module = py::module_::import("{module_name}");
    if (auto it = txTypeToIndexMap.find(txType);
        it != txTypeToIndexMap.end())
    {{
        int index = it->second;
        return module.attr("transactors")[py::cast(index)];
    }}
    std::string errorMessage = "Cannot find tx type value " + std::to_string(txType);
    std::cout << "Python Processing Error: " << errorMessage << std::endl;
    throw std::runtime_error(errorMessage);
}}

py::object
getTransactorFromPython(std::uint16_t txType, beast::Journal const j)
{{
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module module = py::module_::import("{module_name}");
    if (auto it = txTypeToIndexMap.find(txType);
        it != txTypeToIndexMap.end())
    {{
        int index = it->second;
        return module.attr("transactors")[py::cast(index)];
    }}
    std::string errorMessage = "Cannot find tx type value " + std::to_string(txType);
    JLOG(j.error()) << "Python Processing Error: " << errorMessage;
    throw std::runtime_error(errorMessage);
}}

TxConsequences
makeTxConsequences(PreflightContext const& ctx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(ctx.tx.getTxnType(), ctx.j);
    py::object function = transactor.attr("makeTxConsequences");
    try {{
        py::object ret = function(py::cast(ctx, py::return_value_policy::reference));
        return ret.cast<TxConsequences>();
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(ctx.j.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

XRPAmount
calculateBaseFee(ReadView const& view, STTx const& tx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(tx.getTxnType());
    py::object function = transactor.attr("calculateBaseFee");
    try {{
        py::object ret = function(
            py::cast(view, py::return_value_policy::reference),
            py::cast(tx, py::return_value_policy::reference)
        );
        return ret.cast<XRPAmount>();
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

NotTEC
preflight(PreflightContext const& ctx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(ctx.tx.getTxnType(), ctx.j);
    py::object function = transactor.attr("preflight");
    try {{
        py::object ret = function(py::cast(ctx, py::return_value_policy::reference));
        try {{
            return ret.cast<NotTEC>();
        }} catch (const py::cast_error &) {{ // TODO: figure out the exact error that is thrown
            return NotTEC::fromInt(ret.cast<int>());
        }}
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(ctx.j.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

TER
preclaim(PreclaimContext const& ctx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(ctx.tx.getTxnType(), ctx.j);
    py::object function = transactor.attr("preclaim");
    try {{
        py::object ret = function(py::cast(ctx, py::return_value_policy::reference));
        return TER::fromInt(ret.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(ctx.j.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

TER
doApply(ApplyContext& ctx, XRPAmount mPriorBalance, XRPAmount mSourceBalance)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(ctx.tx.getTxnType(), ctx.journal);
    py::object function = transactor.attr("do_apply");
    try {{
        py::object ret = function(
            py::cast(ctx, py::return_value_policy::reference),
            mPriorBalance,
            mSourceBalance);
        return TER::fromInt(ret.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(ctx.journal.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

NotTEC
checkSeqProxy(ReadView const& view, STTx const& tx, beast::Journal j)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(tx.getTxnType(), j);
    py::object function = transactor.attr("check_seq_proxy");
    try {{
        py::object ret = function(
            py::cast(view, py::return_value_policy::reference),
            py::cast(tx, py::return_value_policy::reference),
            py::cast(j, py::return_value_policy::reference)
        );
        try {{
            return ret.cast<NotTEC>();
        }} catch (const py::cast_error &) {{ // TODO: figure out the exact error that is thrown
            return NotTEC::fromInt(ret.cast<int>());
        }}
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(j.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

NotTEC
checkPriorTxAndLastLedger(PreclaimContext const& ctx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(ctx.tx.getTxnType(), ctx.j);
    py::object function = transactor.attr("check_prior_tx_and_last_ledger_ptr");
    try {{
        py::object functionReturn = function(py::cast(ctx, py::return_value_policy::reference));
        return NotTEC::fromInt(functionReturn.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(ctx.j.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

TER
checkFee(PreclaimContext const& ctx, XRPAmount baseFee)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(ctx.tx.getTxnType(), ctx.j);
    py::object function = transactor.attr("check_fee");
    try {{
        py::object ret = function(
            py::cast(ctx, py::return_value_policy::reference),
            baseFee
        );
        return TER::fromInt(ret.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(ctx.j.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

NotTEC
checkSign(PreclaimContext const& ctx)
{{
    py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
    py::object transactor = getTransactorFromPython(ctx.tx.getTxnType(), ctx.j);
    py::object function = transactor.attr("check_sign");
    try {{
        py::object ret = function(py::cast(ctx, py::return_value_policy::reference));
        return NotTEC::fromInt(ret.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        JLOG(ctx.j.error()) << "Python Error: " << errorMessage;
        throw;
    }}
}}

struct TransactorExportInternal {{
    std::string txName;
    std::uint16_t txType;
    std::vector<SOElementExport> txFormat;
    Transactor::ConsequencesFactoryType consequencesFactoryType;
    makeTxConsequencesPtr makeTxConsequences = nullptr;
    calculateBaseFeePtr calculateBaseFee = nullptr;
    preflightPtr preflight = nullptr;
    preclaimPtr preclaim = nullptr;
    doApplyPtr doApply = nullptr;
    checkSeqProxyPtr checkSeqProxy = nullptr;
    checkPriorTxAndLastLedgerPtr checkPriorTxAndLastLedger = nullptr;
    checkFeePtr checkFee = nullptr;
    checkSignPtr checkSign = nullptr;
}};

TransactorExport
mutate(TransactorExportInternal const& tx)
{{
    return TransactorExport{{
        tx.txName.c_str(),
        tx.txType,
        {{const_cast<SOElementExport *>(tx.txFormat.data()), static_cast<int>(tx.txFormat.size())}},
        tx.consequencesFactoryType,
        tx.makeTxConsequences,
        tx.calculateBaseFee,
        tx.preflight,
        tx.preclaim,
        tx.doApply,
        tx.checkSeqProxy,
        tx.checkPriorTxAndLastLedger,
        tx.checkFee,
        tx.checkSign,
    }};
}}

extern "C"
Container<TransactorExport>
getTransactors()
{{
    static std::vector<TransactorExportInternal> const transactors = []{{
        std::vector<TransactorExportInternal> temp = {{}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "transactors")) {{
            return temp;
        }}
        auto transactors = pluginImport.attr("transactors").cast<std::vector<py::object>>();
        for (int i = 0; i < transactors.size(); i++)
        {{
            py::object transactor = transactors[i];
            auto txType = transactor.attr("tx_type").cast<std::uint16_t>();
            txTypeToIndexMap.insert({{txType, i}});

            std::vector<SOElementExport> format{{}};
            auto pythonFormat = transactor.attr("tx_format").cast<std::vector<py::object>>();
            for (py::object transactor: pythonFormat)
            {{
                py::tuple tup = transactor.cast<py::tuple>();
                WSF sfield = tup[0].cast<WSF>();
                SOEStyle varType = tup[1].cast<SOEStyle>();
                format.emplace_back(SOElementExport{{
                    static_cast<SField const&>(sfield).getCode(),
                    varType}});
            }}

            auto txName = transactor.attr("name").cast<std::string>();
            auto consequencesFactoryType = transactor.attr("consequences_factory_type").cast<Transactor::ConsequencesFactoryType>();
            auto exportedTx = TransactorExportInternal{{
                txName,
                txType,
                format,
                consequencesFactoryType,
                transactor.attr("make_tx_consequences").is_none() ? nullptr : makeTxConsequences,
                transactor.attr("calculate_base_fee").is_none() ? nullptr : calculateBaseFee,
                transactor.attr("preflight").is_none() ? nullptr : preflight,
                transactor.attr("preclaim").is_none() ? nullptr : preclaim,
                transactor.attr("do_apply").is_none() ? nullptr : doApply,
                transactor.attr("check_seq_proxy").is_none() ? nullptr : checkSeqProxy,
                transactor.attr("check_prior_tx_and_last_ledger").is_none() ? nullptr : checkPriorTxAndLastLedger,
                transactor.attr("check_fee").is_none() ? nullptr : checkFee,
                transactor.attr("check_sign").is_none() ? nullptr : checkSign,
            }};

            temp.emplace_back(exportedTx);
        }}
        return temp;
    }}();
    static std::vector<TransactorExport> output;
    output.reserve(transactors.size());
    std::transform(transactors.begin(), transactors.end(), std::back_inserter(output), mutate);
    return {{const_cast<TransactorExport *>(output.data()), static_cast<int>(output.size())}};
}}

static std::map<int, std::string> parseSTypeFunctions;

Buffer
parseLeafTypePython(
    SField const& field,
    std::string const& json_name,
    std::string const& fieldName,
    SField const* name,
    Json::Value const& value,
    Json::Value& error)
{{
    if (auto it = parseSTypeFunctions.find(field.fieldType);
        it != parseSTypeFunctions.end())
    {{
        WSF wrappedField = WSF{{(void *)&field}};
        WSF wrappedName = WSF{{(void *)name}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        try {{
            py::module_::import("sys").attr("path").attr("append")("{python_folder}");
            py::object parseFn = py::module_::import("{module_name}").attr(it->second.c_str());
            py::object returnObj = parseFn(
                py::cast(wrappedField, py::return_value_policy::reference),
                json_name,
                fieldName,
                wrappedName,
                value);
            py::tuple tup = returnObj.cast<py::tuple>();
            if (!tup[1].is_none())
            {{
                error = tup[1].cast<Json::Value>();
                Buffer ret;
                return ret;
            }}
            auto const stVar = tup[0].cast<Buffer>();
            return tup[0].cast<Buffer>();
        }} catch (py::error_already_set& e) {{
            // Print the error message
            const char* errorMessage = e.what();
            std::cout << "Python Error: " << errorMessage << std::endl;
            throw;
        }}
    }}

    Buffer ret;
    error = unknown_type(json_name, fieldName, field.fieldType);
    return ret;
}}


extern "C"
Container<STypeExport>
getSTypes()
{{
    static std::vector<STypeExport> const sTypes = []{{
        std::vector<STypeExport> temp = {{}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "new_stypes")) {{
            return temp;
        }}
        auto new_stypes = pluginImport.attr("new_stypes").cast<std::vector<py::object>>();
        for (py::object variable: new_stypes)
        {{
            py::tuple tup = variable.cast<py::tuple>();
            int typeId = tup[0].cast<int>();
            py::function parseFn = tup[1].cast<py::function>();
            parseSTypeFunctions.insert({{typeId, parseFn.attr("__name__").cast<std::string>()}});
            temp.emplace_back(STypeExport{{typeId, parseLeafTypePython}});
        }}
        return temp;
    }}();
    return {{const_cast<STypeExport *>(sTypes.data()), static_cast<int>(sTypes.size())}};
}}


extern "C"
Container<SFieldExport>
getSFields()
{{
    static std::vector<SFieldExport> const sFields = []{{
        std::vector<SFieldExport> temp = {{}};
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "new_sfields")) {{
            return temp;
        }}
        auto new_sfields = pluginImport.attr("new_sfields").cast<std::vector<py::object>>();
        for (py::object variable: new_sfields)
        {{
            WSF wrappedSField = variable.cast<WSF>();
            SField const& sfield = static_cast<SField const&>(wrappedSField);
            temp.emplace_back(SFieldExport{{
                sfield.fieldType,
                sfield.fieldValue,
                sfield.jsonName}});
        }}
        return temp;
    }}();
    return {{const_cast<SFieldExport *>(sFields.data()), static_cast<int>(sFields.size())}};
}}

typedef std::int64_t (*visitEntryXRPChangePtr)(
    bool isDelete,
    std::shared_ptr<SLE const> const& entry,
    bool isBefore);

static std::map<int, std::string> visitEntryXRPChangeFunctions;

std::int64_t visitEntryXRPChange(
    bool isDelete,
    std::shared_ptr<SLE const> const& entry,
    bool isBefore)
{{
    if (auto it = visitEntryXRPChangeFunctions.find(entry->getType());
        it != visitEntryXRPChangeFunctions.end())
    {{
        py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
        try {{
            py::module_::import("sys").attr("path").attr("append")("{python_folder}");
            py::object visitFn = py::module_::import("{module_name}").attr(it->second.c_str());
            py::object returnObj = visitFn(
                isDelete,
                py::cast(entry, py::return_value_policy::reference),
                isBefore);
            return returnObj.cast<std::int64_t>();
        }} catch (py::error_already_set& e) {{
            // Print the error message
            const char* errorMessage = e.what();
            std::cout << "Python Error: " << errorMessage << std::endl;
            throw;
        }}
    }}
}}

struct LedgerObjectExportInternal {{
    std::uint16_t type;
    std::string name; // CamelCase
    std::string rpcName; // snake_case
    std::vector<SOElementExport> format;
    bool isDeletionBlocker;
    DeleterFuncPtr deleteFn;
    visitEntryXRPChangePtr visitEntryXRPChange;
}};

LedgerObjectExport
mutate(LedgerObjectExportInternal const& obj)
{{
    return LedgerObjectExport{{
        obj.type,
        obj.name.c_str(),
        obj.rpcName.c_str(),
        {{const_cast<SOElementExport *>(obj.format.data()), static_cast<int>(obj.format.size())}},
        obj.isDeletionBlocker,
        obj.deleteFn,
        obj.visitEntryXRPChange,
    }};
}}


// extern "C"
// std::vector<LedgerObjectExport>
// getLedgerObjects()
// {{
//     static std::vector<LedgerObjectExportInternal> const objects = []{{
//         py::scoped_interpreter guard{{}}; // start the interpreter and keep it alive
//         py::module_::import("sys").attr("path").attr("append")("{python_folder}");
//         py::module_ module = py::module_::import("{module_name}");
//         std::vector<LedgerObjectExportInternal> temp = {{}};
//         if (!hasattr(module, "new_ledger_objects")) {{
//             return temp;
//         }}
//         auto objects = module.attr("new_ledger_objects").cast<std::vector<py::object>>();
//         for (py::object object: objects)
//         {{
//             std::vector<SOElementExport> objectFormat{{}};
//             auto pythonObjectFormat = object.attr("object_format").cast<std::vector<py::object>>();
//             for (py::object variable: pythonObjectFormat)
//             {{
//                 py::tuple tup = variable.cast<py::tuple>();
//                 WSF sfield = tup[0].cast<WSF>();
//                 SOEStyle varType = tup[1].cast<SOEStyle>();
//                 objectFormat.emplace_back(SOElementExport{{
//                     static_cast<SField const&>(sfield).getCode(),
//                     varType}});
//             }}
//             auto const objectType = object.attr("object_type").cast<std::uint16_t>();
//             py::function visitFn = object.attr("visit_entry_xrp_change").cast<py::function>();
//             visitEntryXRPChangeFunctions.insert({{objectType, visitFn.attr("__name__").cast<std::string>()}});
//             temp.emplace_back(LedgerObjectExportInternal{{
//                 objectType,
//                 object.attr("object_name").cast<std::string>(),
//                 object.attr("object_rpc_name").cast<std::string>(),
//                 std::move(objectFormat),
//                 object.attr("is_deletion_blocker").cast<bool>(),
//                 nullptr,  // TODO: fix
//                 visitEntryXRPChange}});
//         }}
//         return temp;
//     }}();
//     static std::vector<LedgerObjectExport> output;
//     output.reserve(objects.size());
//     std::transform(objects.begin(), objects.end(), std::back_inserter(output), mutate);

//     return output;
// }}
"""


def create_files(python_file):
    abs_python_file = os.path.abspath(python_file)
    python_folder = os.path.dirname(abs_python_file)
    sys.path.append(python_folder)
    last_slash = abs_python_file.rfind("/")
    module_name = abs_python_file[(last_slash+1):-3]
    # module = importlib.import_module(module_name)
    tx_name = "DummyTx"  # module.tx_name
    # TODO: add logic to check validity of Python transactors
    # TODO: switch to Jinja
    # TODO: add logic to only generate the methods that exist in Python

    with open(f"{tx_name}.cpp", "w") as f:
        f.write(generate_cpp(tx_name, module_name, python_folder))
    
    return os.path.abspath(f"{tx_name}.cpp"), module_name


def build_files(cpp_file, project_name):
    with tempfile.TemporaryDirectory() as build_temp:
        build_temp = "./build/test"
        build_source_dir = os.path.dirname(__file__)
        conan_source_dir = os.path.dirname(build_source_dir)
        conan_build_dir = os.path.join(conan_source_dir, "build", "generators")
        cmake_args = []
        build_args = []
        if not os.path.exists(conan_build_dir):
            subprocess.run([
                "conan",
                "install",
                conan_source_dir,
                "--build",
                "missing",
                "--settings",
                "build_type=Debug"
            ], check=True, cwd=build_temp, stdout=subprocess.DEVNULL)
        conan_cmake_file = os.path.join(conan_build_dir, "conan_toolchain.cmake")
        cmake_args += [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={os.getcwd()}{os.sep}",
            f"-DPROJECT_NAME={project_name}",
            f"-DSOURCE_FILE={cpp_file}",
            f"-DCMAKE_TOOLCHAIN_FILE:FILEPATH={conan_cmake_file}",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DBUILD_SHARED_LIBS=ON",
        ]
        subprocess.run(
            ["cmake", build_source_dir] + cmake_args, cwd=build_temp, check=True, stdout=subprocess.DEVNULL
        )
        subprocess.run(
            ["cmake", "--build", "."] + build_args, cwd=build_temp, check=True, stdout=subprocess.DEVNULL
        )


if __name__ == "__main__":
    cpp_file_fullpath, module_name = create_files(sys.argv[1])
    build_files(cpp_file_fullpath, module_name)
