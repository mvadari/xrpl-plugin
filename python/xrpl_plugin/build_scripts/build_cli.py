import os
import sys
import subprocess
import tempfile
from pathlib import Path


def generate_cpp(tx_name, module_name, python_folder):
    return f"""#include <ripple/app/tx/TxConsequences.h>
#include <ripple/basics/Log.h>
#include <ripple/basics/XRPAmount.h>
#include <ripple/ledger/ApplyView.h>
#include <ripple/ledger/View.h>
#include <ripple/plugin/exports.h>
#include <ripple/protocol/Feature.h>
#include <ripple/protocol/InnerObjectFormats.h>
#include <ripple/protocol/TER.h>
#include <ripple/protocol/TxFlags.h>
#include <ripple/protocol/st.h>
#include <map>
#include <vector>

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

static int interpreterCount = 0;

class CustomScopedInterpreter
{{
public:
    explicit CustomScopedInterpreter()
    {{
        if (interpreterCount == 0) py::initialize_interpreter();
        interpreterCount += 1;
    }}

    ~CustomScopedInterpreter()
    {{
        interpreterCount -= 1;
        if (interpreterCount == 0) py::finalize_interpreter();
    }}
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
    ConsequencesFactoryType consequencesFactoryType;
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
mutateTransactor(TransactorExportInternal const& tx)
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
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
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
                format.emplace_back(SOElementExport{{
                    tup[0].attr("fieldCode").cast<int>(),
                    tup[1].cast<SOEStyle>()}});
            }}

            auto exportedTx = TransactorExportInternal{{
                transactor.attr("name").cast<std::string>(),
                txType,
                format,
                transactor.attr("consequences_factory_type").cast<ConsequencesFactoryType>(),
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
    std::transform(transactors.begin(), transactors.end(), std::back_inserter(output), mutateTransactor);
    return {{const_cast<TransactorExport *>(output.data()), static_cast<int>(output.size())}};
}}

// ----------------------------------------------------------------------------
// Ledger Objects
// ----------------------------------------------------------------------------

static std::map<std::uint16_t, int> ledgerObjectTypeToIndexMap;

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
mutateLedgerObject(LedgerObjectExportInternal const& obj)
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

py::object
getLedgerObjectFromPython(std::uint16_t object)
{{
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module module = py::module_::import("{module_name}");
    if (auto it = ledgerObjectTypeToIndexMap.find(object);
        it != ledgerObjectTypeToIndexMap.end())
    {{
        int index = it->second;
        return module.attr("ledger_objects")[py::cast(index)];
    }}
    std::string errorMessage = "Cannot find ledger object type value " + std::to_string(object);
    std::cout << "Python Processing Error: " << errorMessage << std::endl;
    throw std::runtime_error(errorMessage);
}}

TER
deleteObject(
    Application& app,
    ApplyView& view,
    AccountID const& account,
    uint256 const& delIndex,
    std::shared_ptr<SLE> const& sleDel,
    beast::Journal j)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::object object = getLedgerObjectFromPython(sleDel->getType());
    py::object function = object.attr("delete_object");
    try {{
        py::object ret = function(
            py::cast(app, py::return_value_policy::reference),
            py::cast(view, py::return_value_policy::reference),
            py::cast(account, py::return_value_policy::reference),
            delIndex,
            py::cast(sleDel, py::return_value_policy::reference),
            py::cast(j, py::return_value_policy::reference)
        );
        return TER::fromInt(ret.cast<int>());
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

std::int64_t
visitEntryXRPChange(
    bool isDelete,
    std::shared_ptr<STLedgerEntry const> const& entry,
    bool isBefore)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::object object = getLedgerObjectFromPython(entry->getType());
    py::object function = object.attr("visit_entry_xrp_change");
    try {{
        py::object ret = function(
            isDelete,
            py::cast(entry, py::return_value_policy::reference),
            isBefore
        );
        return ret.cast<std::int64_t>();
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

extern "C"
Container<LedgerObjectExport>
getLedgerObjects()
{{
    static std::vector<LedgerObjectExportInternal> const objects = []{{
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module_ module = py::module_::import("{module_name}");
        std::vector<LedgerObjectExportInternal> temp = {{}};
        if (!hasattr(module, "ledger_objects")) {{
            return temp;
        }}
        auto objects = module.attr("ledger_objects").cast<std::vector<py::object>>();
        for (int i = 0; i < objects.size(); i++)
        {{
            py::object object = objects[i];
            auto objectType = object.attr("object_type").cast<std::uint16_t>();
            ledgerObjectTypeToIndexMap.insert({{objectType, i}});

            std::vector<SOElementExport> objectFormat{{}};
            auto pythonObjectFormat = object.attr("object_format").cast<std::vector<py::object>>();
            for (py::object variable: pythonObjectFormat)
            {{
                py::tuple tup = variable.cast<py::tuple>();
                objectFormat.emplace_back(SOElementExport{{
                    tup[0].attr("fieldCode").cast<int>(),
                    tup[1].cast<SOEStyle>()}});
            }}
            temp.emplace_back(LedgerObjectExportInternal{{
                objectType,
                object.attr("name").cast<std::string>(),
                object.attr("rpc_name").cast<std::string>(),
                std::move(objectFormat),
                object.attr("is_deletion_blocker").cast<bool>(),
                object.attr("delete_object").is_none() ? nullptr : deleteObject,
                object.attr("visit_entry_xrp_change").is_none() ? nullptr : visitEntryXRPChange}});
        }}
        return temp;
    }}();
    static std::vector<LedgerObjectExport> output;
    output.reserve(objects.size());
    std::transform(objects.begin(), objects.end(), std::back_inserter(output), mutateLedgerObject);
    return {{const_cast<LedgerObjectExport *>(output.data()), static_cast<int>(output.size())}};
}}

// ----------------------------------------------------------------------------
// SFields
// ----------------------------------------------------------------------------

struct SFieldExportInternal {{
    int typeId;
    int fieldValue;
    std::string txtName;
}};

SFieldExport
mutateSField(SFieldExportInternal const& field)
{{
    return SFieldExport{{
        field.typeId,
        field.fieldValue,
        field.txtName.c_str(),
    }};
}}

extern "C"
Container<SFieldExport>
getSFields()
{{
    static std::vector<SFieldExportInternal> const sFields = []{{
        std::vector<SFieldExportInternal> temp = {{}};
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "sfields")) {{
            return temp;
        }}
        auto sfields = pluginImport.attr("sfields").cast<std::vector<py::object>>();
        for (py::object& sfield: sfields)
        {{
            temp.emplace_back(SFieldExportInternal{{
                sfield.attr("fieldType").cast<int>(),
                sfield.attr("fieldValue").cast<int>(),
                sfield.attr("fieldName").cast<std::string>()}});
        }}
        return temp;
    }}();
    static std::vector<SFieldExport> output;
    output.reserve(sFields.size());
    std::transform(sFields.begin(), sFields.end(), std::back_inserter(output), mutateSField);
    return {{const_cast<SFieldExport *>(output.data()), static_cast<int>(output.size())}};
}}

// ----------------------------------------------------------------------------
// Serialized Types
// ----------------------------------------------------------------------------

static std::map<int, int> sTypeToIndexMap;

py::object
getSTypeFromPython(std::uint16_t object)
{{
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module module = py::module_::import("{module_name}");
    if (auto it = sTypeToIndexMap.find(object);
        it != sTypeToIndexMap.end())
    {{
        int index = it->second;
        return module.attr("stypes")[py::cast(index)];
    }}
    std::string errorMessage = "Cannot find stype value " + std::to_string(object);
    std::cout << "Python Processing Error: " << errorMessage << std::endl;
    throw std::runtime_error(errorMessage);
}}

Buffer
parseValue(
    SField const& field,
    std::string const& json_name,
    std::string const& fieldName,
    SField const* name,
    Json::Value const& value,
    Json::Value& error)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::object object = getSTypeFromPython(field.fieldType);
    py::object function = object.attr("parse_value");
    try {{
        WSF wrappedField = WSF{{(void *)&field}};
        WSF wrappedName = WSF{{(void *)name}};
        py::object returnObj = function(
            py::cast(wrappedField, py::return_value_policy::reference),
            json_name,
            fieldName,
            wrappedName,
            value);
        try {{
            return returnObj.cast<Buffer>();
        }} catch (const py::cast_error &) {{ // TODO: figure out the exact error that is thrown
            error = returnObj.cast<Json::Value>();
            return Buffer();
        }}
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

std::string
toString(int typeId, Buffer const& buf)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::object object = getSTypeFromPython(typeId);
    py::object function = object.attr("to_string");
    try {{
        py::object ret = function(py::cast(buf, py::return_value_policy::reference));
        return ret.cast<std::string>();
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

Json::Value
toJson(int typeId, Buffer const& buf)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::object object = getSTypeFromPython(typeId);
    py::object function = object.attr("to_json");
    try {{
        py::object ret = function(py::cast(buf, py::return_value_policy::reference));
        return ret.cast<Json::Value>();
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

void
toSerializer(int typeId, Buffer const& buf, Serializer& s)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::object object = getSTypeFromPython(typeId);
    py::object function = object.attr("to_serializer");
    try {{
        function(
            py::cast(buf, py::return_value_policy::reference),
            py::cast(s, py::return_value_policy::reference)
        );
        return;
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

Buffer
fromSerialIter(int typeId, SerialIter& st)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::object object = getSTypeFromPython(typeId);
    py::object function = object.attr("from_serial_iter");
    try {{
        py::object ret = function(py::cast(st, py::return_value_policy::reference));
        return ret.cast<Buffer>();
    }} catch (py::error_already_set& e) {{
        // Print the error message
        const char* errorMessage = e.what();
        std::cout << "Python Error: " << errorMessage << std::endl;
        throw;
    }}
}}

struct STypeExportInternal {{
    int typeId;
    std::string typeName;
    parsePluginValuePtr parsePtr;
    toStringPtr toString;
    toJsonPtr toJson;
    toSerializerPtr toSerializer;
    fromSerialIterPtr fromSerialIter;
}};

STypeExport
mutateSType(STypeExportInternal const& stype)
{{
    return STypeExport{{
        stype.typeId,
        stype.typeName.c_str(),
        stype.parsePtr,
        stype.toString,
        stype.toJson,
        stype.toSerializer,
        stype.fromSerialIter,
    }};
}}

extern "C"
Container<STypeExport>
getSTypes()
{{
    static std::vector<STypeExportInternal> const types = []{{
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module_ module = py::module_::import("{module_name}");
        std::vector<STypeExportInternal> temp = {{}};
        if (!hasattr(module, "stypes")) {{
            return temp;
        }}
        auto types = module.attr("stypes").cast<std::vector<py::object>>();
        for (int i = 0; i < types.size(); i++)
        {{
            py::object stype = types[i];
            auto typeId = stype.attr("type_id").cast<int>();
            auto typeName = stype.attr("type_name").cast<std::string>();
            sTypeToIndexMap.insert({{typeId, i}});

            temp.emplace_back(STypeExportInternal{{
                typeId,
                typeName,
                parseValue,
                toString,
                stype.attr("to_json").is_none() ? nullptr : toJson,
                toSerializer,
                fromSerialIter}});
        }}
        return temp;
    }}();
    static std::vector<STypeExport> output;
    output.reserve(types.size());
    std::transform(types.begin(), types.end(), std::back_inserter(output), mutateSType);
    return {{const_cast<STypeExport *>(output.data()), static_cast<int>(output.size())}};
}}

// ----------------------------------------------------------------------------
// Amendments
// ----------------------------------------------------------------------------

struct AmendmentExportInternal {{
    std::string name;
    bool supported;
    VoteBehavior vote;
}};

AmendmentExport
mutateAmendment(AmendmentExportInternal const& amendment)
{{
    return AmendmentExport{{
        amendment.name.c_str(),
        amendment.supported,
        amendment.vote,
    }};
}}

extern "C"
Container<AmendmentExport>
getAmendments()
{{
    static std::vector<AmendmentExportInternal> const amendments = []{{
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
        std::vector<AmendmentExportInternal> temp = {{}};
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "amendments")) {{
            return temp;
        }}
        auto amendments = pluginImport.attr("amendments").cast<std::vector<py::object>>();
        for (py::object variable: amendments)
        {{
            temp.emplace_back(AmendmentExportInternal{{
                variable.attr("name").cast<std::string>(),
                variable.attr("supported").cast<bool>(),
                variable.attr("vote").cast<VoteBehavior>()}});
        }}
        return temp;
    }}();
    static std::vector<AmendmentExport> output;
    output.reserve(amendments.size());
    std::transform(amendments.begin(), amendments.end(), std::back_inserter(output), mutateAmendment);
    return {{const_cast<AmendmentExport *>(output.data()), static_cast<int>(output.size())}};
}}

// ----------------------------------------------------------------------------
// TER codes
// ----------------------------------------------------------------------------

struct TERExportInternal {{
    int code;
    std::string codeStr;
    std::string description;
}};

TERExport
mutateTERcode(TERExportInternal const& TERcode)
{{
    return TERExport{{
        TERcode.code,
        TERcode.codeStr.c_str(),
        TERcode.description.c_str(),
    }};
}}

extern "C"
Container<TERExport>
getTERcodes()
{{
    static std::vector<TERExportInternal> const codes = []{{
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
        std::vector<TERExportInternal> temp = {{}};
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "ter_codes")) {{
            return temp;
        }}
        auto codes = pluginImport.attr("ter_codes").cast<std::vector<py::object>>();
        for (py::object variable: codes)
        {{
            temp.emplace_back(TERExportInternal{{
                variable.attr("code").cast<int>(),
                variable.attr("code_str").cast<std::string>(),
                variable.attr("description").cast<std::string>()}});
        }}
        return temp;
    }}();
    static std::vector<TERExport> output;
    output.reserve(codes.size());
    std::transform(codes.begin(), codes.end(), std::back_inserter(output), mutateTERcode);
    return {{const_cast<TERExport *>(output.data()), static_cast<int>(output.size())}};
}}

// ----------------------------------------------------------------------------
// Invariant Checks
// ----------------------------------------------------------------------------

static std::map<void*, std::vector<std::string>> checkData;

void
visitEntryExport(
    void* id,
    bool isDelete,
    std::shared_ptr<SLE const> const& before,
    std::shared_ptr<SLE const> const& after)
{{
    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module pluginImport = py::module_::import("{module_name}");
    if (!hasattr(pluginImport, "invariant_checks")) {{
        throw std::runtime_error("Expected `invariant_checks` variable");
    }}
    auto checks = pluginImport.attr("invariant_checks").cast<std::vector<py::object>>();
    std::vector<std::string> storedCheckData;
    if (auto it = checkData.find(id); it != checkData.end())
    {{
        storedCheckData = it->second;
    }} else
    {{
        for (int i = 0; i < checks.size(); i++)
        {{
            storedCheckData.push_back("");
        }}
        checkData.insert({{id, storedCheckData}});
    }}

    for (int i = 0; i < checks.size(); i++)
    {{
        std::string data = storedCheckData[i];
        py::function checkType = checks[i];
        py::object check = checkType();

        py::object function = check.attr("visit_entry_actual");
        try {{
            py::object ret = function(
                data.length() == 0 ? py::none() : py::cast(data),
                isDelete,
                py::cast(before, py::return_value_policy::reference),
                py::cast(after, py::return_value_policy::reference)
            );
            storedCheckData[i] = ret.cast<std::string>();
        }} catch (py::error_already_set& e) {{
            // Print the error message
            const char* errorMessage = e.what();
            std::cout << "Python Error: " << errorMessage << std::endl;
            throw;
        }}
    }}
    checkData[id] = storedCheckData;
}}

bool
finalizeExport(
    void* id,
    STTx const& tx,
    TER const result,
    XRPAmount const fee,
    ReadView const& view,
    beast::Journal const& j)
{{
    bool finalizeResult = true;

    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    py::module pluginImport = py::module_::import("{module_name}");
    if (!hasattr(pluginImport, "invariant_checks")) {{
        throw std::runtime_error("Expected `invariant_checks` variable");
    }}
    auto checks = pluginImport.attr("invariant_checks").cast<std::vector<py::object>>();
    std::vector<std::string> storedCheckData;
    if (auto it = checkData.find(id); it != checkData.end())
    {{
        storedCheckData = it->second;
    }} else
    {{
        JLOG(j.fatal()) << "Invariant failed: could not find matching ID";
        return false;
    }}
    for (int i = 0; i < checks.size(); i++)
    {{
        std::string data = storedCheckData[i];
        py::function checkType = checks[i];
        py::object check = checkType();

        py::object function = check.attr("finalize_actual");
        try {{
            py::object ret = function(
                data,
                py::cast(tx, py::return_value_policy::reference),
                py::cast(result, py::return_value_policy::reference),
                py::cast(fee, py::return_value_policy::reference),
                py::cast(view, py::return_value_policy::reference),
                py::cast(j, py::return_value_policy::reference)
            );
            finalizeResult &= ret.cast<bool>();
        }} catch (py::error_already_set& e) {{
            // Print the error message
            const char* errorMessage = e.what();
            std::cout << "Python Error: " << errorMessage << std::endl;
            throw;
        }}
    }}
    checkData.erase(id);
    return finalizeResult;
}}

extern "C"
Container<InvariantCheckExport>
getInvariantChecks()
{{
    static std::vector<InvariantCheckExport> const checks = []{{
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module_ module = py::module_::import("{module_name}");
        std::vector<InvariantCheckExport> temp = {{}};
        if (!hasattr(module, "invariant_checks")) {{
            return temp;
        }}
        temp.emplace_back(InvariantCheckExport{{
            visitEntryExport,
            finalizeExport}});
        return temp;
    }}();
    return {{const_cast<InvariantCheckExport *>(checks.data()), static_cast<int>(checks.size())}};
}}

// ----------------------------------------------------------------------------
// Inner Object Formats
// ----------------------------------------------------------------------------

struct InnerObjectExportInternal {{
    int code;
    std::string name;
    std::vector<SOElementExport> format;
}};

InnerObjectExport
mutateInnerObject(InnerObjectExportInternal const& innerObject)
{{
    return InnerObjectExport{{
        static_cast<uint16_t>(innerObject.code),
        innerObject.name.c_str(),
        {{const_cast<SOElementExport *>(innerObject.format.data()), static_cast<int>(innerObject.format.size())}},
    }};
}}

extern "C"
Container<InnerObjectExport>
getInnerObjectFormats()
{{
    static std::vector<InnerObjectExportInternal> const innerObjects = []{{
        std::vector<InnerObjectExportInternal> temp = {{}};
        CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
        py::module_::import("sys").attr("path").attr("append")("{python_folder}");
        py::module pluginImport = py::module_::import("{module_name}");
        if (!hasattr(pluginImport, "inner_objects")) {{
            return temp;
        }}
        auto innerObjects = pluginImport.attr("inner_objects").cast<std::vector<py::object>>();
        for (int i = 0; i < innerObjects.size(); i++)
        {{
            py::object innerObject = innerObjects[i];

            std::vector<SOElementExport> format{{}};
            auto pythonFormat = innerObject.attr("format").cast<std::vector<py::object>>();
            for (py::object innerObject: pythonFormat)
            {{
                py::tuple tup = innerObject.cast<py::tuple>();
                format.emplace_back(SOElementExport{{
                    tup[0].attr("fieldCode").cast<int>(),
                    tup[1].cast<SOEStyle>()}});
            }}

            auto exportedTx = InnerObjectExportInternal{{
                innerObject.attr("field").attr("fieldCode").cast<int>(),
                innerObject.attr("field").attr("fieldName").cast<std::string>(),
                format,
            }};

            temp.emplace_back(exportedTx);
        }}
        return temp;
    }}();
    static std::vector<InnerObjectExport> output;
    output.reserve(innerObjects.size());
    std::transform(innerObjects.begin(), innerObjects.end(), std::back_inserter(output), mutateInnerObject);
    return {{const_cast<InnerObjectExport *>(output.data()), static_cast<int>(output.size())}};
}}

typedef void (*initializePluginPointersPtr)(
    std::map<std::uint16_t, ripple::PluginTxFormat>*,
    std::map<std::uint16_t, ripple::PluginLedgerFormat>*,
    std::map<std::uint16_t, ripple::PluginInnerObjectFormat>*,
    std::map<int, ripple::SField const*>*,
    std::vector<int>*,
    std::map<int, ripple::STypeFunctions>*,
    std::map<int, ripple::parsePluginValuePtr>*,
    std::vector<ripple::TERExport>*);

using init_ptr = std::function<void(
    std::map<std::uint16_t, ripple::PluginTxFormat>*,
    std::map<std::uint16_t, ripple::PluginLedgerFormat>*,
    std::map<std::uint16_t, ripple::PluginInnerObjectFormat>*,
    std::map<int, ripple::SField const*>*,
    std::vector<int>*,
    std::map<int, ripple::STypeFunctions>*,
    std::map<int, ripple::parsePluginValuePtr>*,
    std::vector<ripple::TERExport>*)>;

extern "C" void setPluginPointers(
    std::map<std::uint16_t, PluginTxFormat>* pluginTxFormatPtr,
    std::map<std::uint16_t, PluginLedgerFormat>* pluginObjectsMapPtr,
    std::map<std::uint16_t, PluginInnerObjectFormat>*
        pluginInnerObjectFormatsPtr,
    std::map<int, SField const*>* knownCodeToFieldPtr,
    std::vector<int>* pluginSFieldCodesPtr,
    std::map<int, STypeFunctions>* pluginSTypesPtr,
    std::map<int, parsePluginValuePtr>* pluginLeafParserMapPtr,
    std::vector<TERExport>* pluginTERcodes)
{{
    registerTxFormats(pluginTxFormatPtr);
    registerLedgerObjects(pluginObjectsMapPtr);
    registerPluginInnerObjectFormats(pluginInnerObjectFormatsPtr);
    registerSFields(knownCodeToFieldPtr, pluginSFieldCodesPtr);
    registerSTypes(pluginSTypesPtr);
    registerLeafTypes(pluginLeafParserMapPtr);
    registerPluginTERs(pluginTERcodes);

    CustomScopedInterpreter guard{{}}; // start the interpreter and keep it alive
    py::module_::import("sys").attr("path").attr("append")("{python_folder}");
    auto initializePtr = py::module_::import("xrpl_plugin").attr("rippled_py").attr("initializePluginPointers");
    initializePtr(
        (void *)pluginTxFormatPtr,
        (void *)pluginObjectsMapPtr,
        (void *)pluginInnerObjectFormatsPtr,
        (void *)knownCodeToFieldPtr,
        (void *)pluginSFieldCodesPtr,
        (void *)pluginSTypesPtr,
        (void *)pluginLeafParserMapPtr,
        (void *)pluginTERcodes);
}}
"""


def update_progress(progress):
    bar_length = 30
    sys.stdout.write("\r")
    sys.stdout.write(
        "Building: |{:{}}| {:>3}% Completed".format(
            "█" * int(progress / (100.0 / bar_length)), bar_length, int(progress)
        )
    )
    sys.stdout.flush()


def snake_to_camel_case(string):
    temp = string.split("_")
    return temp[0].capitalize() + "".join(ele.title() for ele in temp[1:])


def create_files(python_file):
    abs_python_file = os.path.abspath(python_file)
    python_folder = os.path.dirname(abs_python_file)
    sys.path.append(python_folder)
    last_slash = abs_python_file.rfind("/")
    module_name = abs_python_file[(last_slash + 1) : -3]
    # module = importlib.import_module(module_name)
    tx_name = snake_to_camel_case(module_name)
    # TODO: add logic to check validity of Python transactors
    # TODO: switch to Jinja
    # TODO: add logic to only generate the methods that exist in Python

    with open(f"{tx_name}.cpp", "w") as f:
        f.write(generate_cpp(tx_name, module_name, python_folder))

    return os.path.abspath(f"{tx_name}.cpp"), module_name


def build_files(cpp_file, project_name):
    with tempfile.TemporaryDirectory() as build_temp:
        build_source_dir = os.path.dirname(__file__)
        conan_source_dir = build_source_dir
        # build_temp = os.getcwd()
        output_dir = os.getcwd()
        conan_build_dir = os.path.join(conan_source_dir, "build", "generators")
        cmake_args = []
        build_args = []
        if not os.path.exists(conan_build_dir):
            subprocess.run(
                [
                    "conan",
                    "install",
                    conan_source_dir,
                    "--build",
                    "missing",
                    "--settings",
                    "build_type=Debug",
                ],
                check=True,
                cwd=build_temp,
                stdout=subprocess.DEVNULL,
            )
        conan_cmake_file = os.path.join(conan_build_dir, "conan_toolchain.cmake")
        cmake_args += [
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={output_dir}",
            f"-DPROJECT_NAME={project_name}",
            f"-DSOURCE_FILE={cpp_file}",
            f"-DCMAKE_TOOLCHAIN_FILE:FILEPATH={conan_cmake_file}",
            "-DCMAKE_BUILD_TYPE=Debug",
            "-DBUILD_SHARED_LIBS=ON",
        ]
        subprocess.run(
            ["cmake", build_source_dir] + cmake_args,
            cwd=build_temp,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
        )

        p = subprocess.Popen(
            ["cmake", "--build", "."] + build_args,
            stdout=subprocess.PIPE,
            cwd=build_temp,
        )

        for line in iter(p.stdout.readline, b""):
            decoded = line.decode("utf-8")
            progress = decoded[decoded.find("[") + 1 : decoded.find("]")].replace(
                "%", ""
            )
            try:
                progress = int(progress)
            except ValueError:
                progress = 0
            update_progress(progress)
        p.stdout.close()
        p.wait()
        if p.returncode == 0:
            print(f"\nPlugin generated: {output_dir}/{project_name}.xrplugin")


def build():
    cpp_file_fullpath, module_name = create_files(sys.argv[1])
    build_files(cpp_file_fullpath, module_name)


# This is included just for dev purposes, to avoid needing to reinstall the
# whole package whenever this script is updated during library development
# It should not be used in prod
# if __name__ == "__main__":
#     build()
