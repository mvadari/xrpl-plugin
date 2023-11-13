import sys
import re

if len(sys.argv) != 2:
    print("Usage: python " + sys.argv[0] + " path/to/rippled/src/ripple/")
    sys.exit(1)


def read_file(filename):
    with open(filename, "r") as f:
        return f.read()


sfield_h_filename = sys.argv[1] + "/protocol/SField.h"
sfield_h = read_file(sfield_h_filename)

sfield_hits = re.findall(
    r"^extern (SF[A-Za-z_0-9]+) const (sf[A-Za-z0-9]+);$", sfield_h, re.MULTILINE
)

_CAMEL_CASE_LEADING_LOWER = "^[^A-Z]+"
_CAMEL_CASE_ABBREVIATION = "[A-Z]+(?![^A-Z])"
_CAMEL_CASE_TYPICAL = "[A-Z][^A-Z]*"
_CAMEL_TO_SNAKE_CASE_REGEX = re.compile(
    f"(?:{_CAMEL_CASE_LEADING_LOWER}|{_CAMEL_CASE_ABBREVIATION}|{_CAMEL_CASE_TYPICAL})"
)
ABBREVIATIONS = {
    "amm": "AMM",
    "did": "DID",
    "id": "ID",
    "lp": "LP",
    "nftoken": "NFToken",
    "unl": "UNL",
    "uri": "URI",
    "xchain": "XChain",
}


def camel_to_snake_case(field: str) -> str:
    # convert all special CamelCase substrings to capitalized strings
    for spec_str in ABBREVIATIONS.values():
        if spec_str in field:
            field = field.replace(spec_str, spec_str.capitalize())

    return "_".join(
        [word.lower() for word in _CAMEL_TO_SNAKE_CASE_REGEX.findall(field)]
    )


def get_type(sf_type):
    if "UINT" in sf_type:
        return "STUInt" + sf_type[7:]
    if "XCHAIN" in sf_type:
        return "STXChainBridge"
    if sf_type == "SF_VL":
        return "STBlob"
    return "ST" + sf_type[3].upper() + sf_type[4:].lower()


for hit in sfield_hits:
    if "SField" in hit[0]:
        print(
            f'    sfieldModule.attr("{camel_to_snake_case(hit[1])}") = WSF{{(void *)&ripple::{hit[1]}}};'
        )
    else:
        stype = get_type(hit[0])
        print(
            f'    sfieldModule.attr("{camel_to_snake_case(hit[1])}") = TWSF<ripple::{stype}>{{(void *)&ripple::{hit[1]}}};'
        )


# TODO: actually edit the file
# Regex to replace:
# (?:^    sfieldModule.attr\("sf_[a-z0-9_]+"\) = T?WSF(?:<ripple::ST[A-Za-z0-9]+>)?{\(void \*\)&ripple::sf[A-Za-z0-9]+};$)+
