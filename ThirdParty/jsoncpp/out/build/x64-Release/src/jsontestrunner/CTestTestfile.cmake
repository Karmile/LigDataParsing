# CMake generated Testfile for 
# Source directory: D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner
# Build directory: D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/out/build/x64-Release/src/jsontestrunner
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(jsoncpp_readerwriter "D:/msys64/ucrt64/bin/python3.11.exe" "-B" "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/../../test/runjsontests.py" "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/out/build/x64-Release/bin/jsontestrunner_exe.exe" "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/../../test/data")
set_tests_properties(jsoncpp_readerwriter PROPERTIES  WORKING_DIRECTORY "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/../../test/data" _BACKTRACE_TRIPLES "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/CMakeLists.txt;43;add_test;D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/CMakeLists.txt;0;")
add_test(jsoncpp_readerwriter_json_checker "D:/msys64/ucrt64/bin/python3.11.exe" "-B" "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/../../test/runjsontests.py" "--with-json-checker" "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/out/build/x64-Release/bin/jsontestrunner_exe.exe" "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/../../test/data")
set_tests_properties(jsoncpp_readerwriter_json_checker PROPERTIES  WORKING_DIRECTORY "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/../../test/data" _BACKTRACE_TRIPLES "D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/CMakeLists.txt;47;add_test;D:/WorkSpace/ParsingJsonData/ThirdParty/jsoncpp/src/jsontestrunner/CMakeLists.txt;0;")
