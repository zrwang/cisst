/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Anton Deguet
  Created on: 2011-05-26

  (C) Copyright 2011 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>


class cmnXMLPathTest: public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(cmnXMLPathTest);
    {
        CPPUNIT_TEST(TestReadExistingFile);
        CPPUNIT_TEST(TestCopyReadExistingFile);
    }
    CPPUNIT_TEST_SUITE_END();

 public:
    void setUp(void);

    void tearDown(void);

    /*! Test read existing file */
    void TestReadExistingFile(void);

    /*! Test read existing file copy */
    void TestCopyReadExistingFile(void);

protected:
    void TestExistingFile1(const std::string & testFile, const std::string & schemaFile);
};


CPPUNIT_TEST_SUITE_REGISTRATION(cmnXMLPathTest);
