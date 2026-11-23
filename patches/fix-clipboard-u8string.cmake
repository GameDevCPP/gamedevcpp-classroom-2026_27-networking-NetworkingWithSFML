# Idempotent, line-ending-safe fix for imgui-sfml 2.6.x's clipboard code,
# which assumes sf::Clipboard::getString().toUtf8() converts directly to
# std::basic_string<std::uint8_t> -- broke when SFML changed
# U8StringCharTraits. A context-diff patch (git apply) is fragile here:
# a Windows checkout with core.autocrlf=true gets CRLF line endings, and
# git apply then reports "patch does not apply" even with
# --ignore-whitespace. A literal string replace on the file content
# sidesteps line-ending differences entirely.
set(TARGET_FILE "imgui-SFML.cpp")
file(READ "${TARGET_FILE}" FILE_CONTENTS)

set(OLD_LINE "std::basic_string<std::uint8_t> tmp = sf::Clipboard::getString().toUtf8();")
set(NEW_LINES "auto u8Clip = sf::Clipboard::getString().toUtf8();\n    std::basic_string<std::uint8_t> tmp(u8Clip.begin(), u8Clip.end());")

string(FIND "${FILE_CONTENTS}" "${OLD_LINE}" OLD_LINE_POS)
string(FIND "${FILE_CONTENTS}" "auto u8Clip = sf::Clipboard::getString().toUtf8();" NEW_LINE_POS)

if(NOT OLD_LINE_POS EQUAL -1)
  string(REPLACE "${OLD_LINE}" "${NEW_LINES}" FILE_CONTENTS "${FILE_CONTENTS}")
  file(WRITE "${TARGET_FILE}" "${FILE_CONTENTS}")
  message(STATUS "fix-clipboard-u8string: patched ${TARGET_FILE}")
elseif(NOT NEW_LINE_POS EQUAL -1)
  message(STATUS "fix-clipboard-u8string: already patched, skipping")
else()
  message(WARNING "fix-clipboard-u8string: expected clipboard code not found in ${TARGET_FILE} -- imgui-sfml source may have changed upstream; patch skipped, build may fail")
endif()
