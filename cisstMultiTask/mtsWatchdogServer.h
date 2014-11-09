/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
$Id: $

Author(s):  Marcin Balicki
Created on: 2014

(C) Copyright 2006-2014 Johns Hopkins University (JHU), All Rights
Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/
#ifndef _mtsWatchdogServer_h
#define _mtsWatchdogServer_h

#include <cisstCommon.h>
#include <cisstMultiTask/mtsFunctionWrite.h>
#include <cisstMultiTask/mtsFunctionRead.h>
#include <cisstMultiTask/mtsFunctionVoid.h>
#include <cisstMultiTask/mtsInterfaceProvided.h>
#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsStateTable.h>
#include <cisstOSAbstraction/osaThread.h>
#include <cisstOSAbstraction/osaStopwatch.h>

#include <queue>

/// A tool for observing a connection between two components for a set amount of time.
/// \todo Should be easy to merge server and client into a single object. Could integrate into standard component interfaces.

class mtsWatchdogServer: public cmnGenericObject {
    // used to control the log level, "Run Error" by default
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);
protected:
    osaStopwatch StopWatch;
    double Timeout;

    mtsFunctionWrite *WatchdogReadBool;
    mtsFunctionWrite *WatchdogWriteBool;

    bool Is_OK;

    //if false then it was remotely updated
    //set to true locally as an acknowledgment
    mtsBool  WatchdogState;

public:
    mtsWatchdogServer():
        Timeout(0),
        Is_OK(false),
        WatchdogState(true)
        {};
  
    ~mtsWatchdogServer(){};

    void AddToProvidedInterface(mtsInterfaceProvided &provInt, mtsStateTable &stateTable);

    void SetTimeoutPeriod(const double &seconds) {
        Timeout = seconds;
    }

    void Start();

    //true if everything is ok;
    bool CheckAndUpdate();
    void Reset();
};


CMN_DECLARE_SERVICES_INSTANTIATION(mtsWatchdogServer);

#endif
