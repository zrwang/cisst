#
#

# (C) Copyright 2005-2012 Johns Hopkins University (JHU), All Rights
# Reserved.

# --- begin cisst license - do not edit ---
#
# This software is provided "as is" under an open source license, with
# no warranty.  The complete license can be found in license.txt and
# http://www.cisst.org/cisst/license.txt.
#
# --- end cisst license ---

# CMakeLists.txt for cisstCommon wrapping tests

# Keep a list all tests for ctest
set (CTEST_TEST_FILE ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile-cisstCommonPython.txt)
set (CTEST_TEST_FILE_AUTO ${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile-cisstCommonPython-auto.txt)
file (WRITE ${CTEST_TEST_FILE} "# File generated by CMake, do not modify\n")

# Create the scripts from .in
set (LIB cisstCommon)

if (CMAKE_CONFIGURATION_TYPES)
  foreach (config ${CMAKE_CONFIGURATION_TYPES})
    configure_file (${cisst_SOURCE_DIR}/cisstTestsDriver/cisstTestsDriver.py.in
                    ${EXECUTABLE_OUTPUT_PATH}/${config}/cisstCommonTests.py
                    IMMEDIATE @ONLY@)
  endforeach (config)
else (CMAKE_CONFIGURATION_TYPES)
  configure_file (${cisst_SOURCE_DIR}/cisstTestsDriver/cisstTestsDriver.py.in
                  ${EXECUTABLE_OUTPUT_PATH}/cisstCommonTests.py
                  IMMEDIATE @ONLY@)
endif (CMAKE_CONFIGURATION_TYPES)


# Run the script to generate the list of tests automatically
add_custom_target (cisstCommon_CREATE_PYTHON_TESTS_LIST ALL
                   ${PYTHON_EXECUTABLE} ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_CFG_INTDIR}/cisstCommonTests.py
                   -d ${PYTHON_EXECUTABLE} > ${CTEST_TEST_FILE_AUTO})
set_property (TARGET cisstCommon_CREATE_PYTHON_TESTS_LIST PROPERTY FOLDER "cisstCommon/tests")

add_dependencies (cisstCommon_CREATE_PYTHON_TESTS_LIST
                  cisstCommon
                  _cisstCommonPython)

file (APPEND ${CTEST_TEST_FILE} "include (\"${CTEST_TEST_FILE_AUTO}\")\n")


# Add all the test lists
set_directory_properties (PROPERTIES TEST_INCLUDE_FILE
                          ${CTEST_TEST_FILE})
