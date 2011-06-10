/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id$ */

#include <cisstCommon.h>
#include <cisstOSAbstraction.h>
#include <cisstMultiTask.h>

#include "appTask.h"
#include "robotLowLevel.h"

using namespace std;

//***** Example of using cisstMultiTask without command pattern. *****

int main(void)
{
    // Log configuration
    cmnLogger::SetMask(CMN_LOG_ALLOW_ERRORS);
    cmnLogger::AddChannel(cout, CMN_LOG_ALLOW_ERRORS);
    // add a log per thread
    osaThreadedLogFile threadedLog("example5-");
    cmnLogger::AddChannel(threadedLog, CMN_LOG_ALLOW_ERRORS);

    // create our tasks
    mtsComponentManager * componentManager = mtsComponentManager::GetInstance();
    robotLowLevel * robot1Task = new robotLowLevel("Robot1", 100 * cmn_ms);
    robotLowLevel * robot2Task = new robotLowLevel("Robot2", 100 * cmn_ms);
    appTask * appTaskControl1 = new appTask("ControlRobot1", "Robot1", "Robot2", 100 * cmn_ms);
    appTask * appTaskControl2 = new appTask("ControlRobot2", "Robot2", "Robot1", 150 * cmn_ms);

    // add all tasks
    componentManager->AddComponent(robot1Task);
    componentManager->AddComponent(robot2Task);
    componentManager->AddComponent(appTaskControl1);
    componentManager->AddComponent(appTaskControl2);

    componentManager->CreateAll();
    componentManager->StartAll();

    // Loop until both tasks are closed
    while (!(appTaskControl1->GetExitFlag() && appTaskControl2->GetExitFlag())) {
        osaSleep(0.5 * cmn_s); // 0.5 seconds
    }

    componentManager->KillAll();

    /*
    while (!robot1Task->IsTerminated()) osaTime::Sleep(PeriodRobot);
    while (!robot2Task->IsTerminated()) osaTime::Sleep(PeriodRobot);
    while (!appTaskControl->IsTerminated()) osaTime::Sleep(PeriodRobot);
    */
    componentManager->Cleanup();
    return 0;
}

/*
  Author(s):  Peter Kazanzides, Anton Deguet
  Created on: 2004-04-30

  (C) Copyright 2004-2008 Johns Hopkins University (JHU), All Rights
  Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---

*/