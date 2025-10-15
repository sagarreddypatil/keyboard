import json

layout = json.load(open("../keyboard-layout.json"))

symbol_name = {
    "`": "GRAVE",
    "-": "MINUS",
    "=": "EQUAL",
    "[": "BRACKET_LEFT",
    "]": "BRACKET_RIGHT",
    "\\": "BACKSLASH",
    ";": "SEMICOLON",
    "'": "APOSTROPHE",
    ",": "COMMA",
    ".": "PERIOD",
    "/": "SLASH",
    "": "SPACE",
}


out = "static constexpr u8 kKeymap[kRows][kCols] = {"

for row in layout:
    out += "{"
    for key in row:
        if not isinstance(key, str):
            continue
        if "\n" in key:
            key = key.strip().splitlines()[-1]
        key = key.upper()
        if key in symbol_name:
            key = symbol_name[key]
        out += "HID_KEY_" + key.replace(" ", "_") + ", "
    out = out[:-2]
    out += "},\n"
out = out[:-2]
out += "\n};"

print(out)
