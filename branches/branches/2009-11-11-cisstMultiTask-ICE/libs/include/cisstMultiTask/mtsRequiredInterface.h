/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*
  $Id$

  Author(s):  Peter Kazanzides
  Created on: 2008-11-13

  (C) Copyright 2008 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/


#ifndef _mtsRequiredInterface_h
#define _mtsRequiredInterface_h

#include <cisstCommon/cmnGenericObject.h>
#include <cisstCommon/cmnClassRegisterMacros.h>
#include <cisstCommon/cmnNamedMap.h>

#include <cisstMultiTask/mtsCommandBase.h>
#include <cisstMultiTask/mtsForwardDeclarations.h>
#include <cisstMultiTask/mtsCommandQueuedVoid.h>
#include <cisstMultiTask/mtsCommandQueuedWrite.h>

#include <cisstMultiTask/mtsInterfaceCommon.h>

// Always include last
#include <cisstMultiTask/mtsExport.h>

/*!
  \file
  \brief Declaration of mtsRequiredInterface
 */


/*!
  \ingroup cisstMultiTask

  This class implements the required interface for a device (mtsDevice)
  or task(mtsTask).  The required interface gets populated with pointers
  to command objects, which have four signatures:

     Void:           no parameters
     Read:           one non-const parameter
     Write:          one const parameter
     QualifiedRead:  one non-const (read) and one const (write) parameter

  The required interface may also have command object pointers for the
  following types of event handlers:

     Void:           no parameters
     Write:          one const parameter

  When the required interface of this task/device is connected to the
  provided interface of another task/device, the command object pointers
  are "bound" to command objects provided by the other task/device.
  Similarly, the event handlers are added as observers of events that
  are generated by the provided interface of the other task/device.

  This implementation is simpler than the provided interface because
  we assume that a required interface is never connected to more than
  one provided interface, whereas a provided interface can be used
  by multiple required interfaces.  While one can conceive of cases
  where it may be useful to have a required interface connect to
  multiple provided interfaces (e.g., running a robot simulation in
  parallel with a real robot), at this time it is not worth the trouble.
 */

class CISST_EXPORT mtsRequiredInterface: public cmnGenericObject
{
    CMN_DECLARE_SERVICES(CMN_NO_DYNAMIC_CREATION, CMN_LOG_LOD_RUN_ERROR);

    friend class mtsComponentProxy;
    friend class mtsComponentInterfaceProxyClient;

protected:

    // PK: TEMP (copied from mtsTaskInterface.h)    
    enum {DEFAULT_ARG_BUFFER_LEN = 16 };

    /*! A string identifying the 'Name' of the required interface. */
    std::string Name;

    /*! Mailbox (if supported). */
    mtsMailBox * MailBox;

    /*! Pointer to provided interface that we are connected to. */
    mtsDeviceInterface * OtherInterface;

    /*! Default constructor. Does nothing, should not be used. */
    mtsRequiredInterface(void) {}

 public:
    /*! Constructor. Sets the name, device pointer, and mailbox for queued events.
        \param interfaceName Name of required interface
        \param mbox Mailbox to use for queued events (for tasks); set to 0 for devices
        Could use a boolean (useMbox) for last parameter or delete it completely, and decide
        whether or not to allocate a mailbox based on dynamic type of device.
    */
    mtsRequiredInterface(const std::string & interfaceName, mtsMailBox * mailBox = 0);

    /*! Default destructor. */
    virtual ~mtsRequiredInterface();
    
    /*! Returns the name of the interface. */
    virtual std::string GetName(void) const {
        return Name;
    }

    virtual mtsDeviceInterface *GetConnectedInterface(void) const {
        return OtherInterface;
    }
    
    /*! Get the names of commands required by this interface. */
    //@{
    virtual std::vector<std::string> GetNamesOfCommandPointers(void) const;
    virtual std::vector<std::string> GetNamesOfCommandPointersVoid(void) const;
    virtual std::vector<std::string> GetNamesOfCommandPointersRead(void) const;
    virtual std::vector<std::string> GetNamesOfCommandPointersWrite(void) const;
    virtual std::vector<std::string> GetNamesOfCommandPointersQualifiedRead(void) const;
    //@}

    /*! Get the names of event handlers that exist in this interface */
    //@{
    virtual std::vector<std::string> GetNamesOfEventHandlersVoid(void) const;
    virtual std::vector<std::string> GetNamesOfEventHandlersWrite(void) const;
    //@}

    /*! Find an event handler based on its name. */
    //@{
    virtual mtsCommandVoidBase * GetEventHandlerVoid(const std::string & eventName) const;
    virtual mtsCommandWriteBase * GetEventHandlerWrite(const std::string & eventName) const;
    //@}
    
    /*! Extract all the information on a required interface (function objects 
        and event handlers with arguments serialized (if any)) */
    bool GetRequiredInterfaceDescription(RequiredInterfaceDescription & requiredInterfaceDescription) const;

    void ConnectTo(mtsDeviceInterface * other) {
        OtherInterface = other;
    }
    void Disconnect(void);  // this could work if we use function objects rather than ptrs, or have special NOP command object

    /*! Bind command and events.  This method needs to provide a user
      Id so that GetCommandVoid and GetCommandWrite (queued
      commands) know which mailbox to use.  The user Id is provided
      by the provided interface when calling AllocateResources. */ 
    bool BindCommandsAndEvents(unsigned int userId);

    inline void DisableAllEvents(void) {
        EventHandlersVoid.ForEachVoid(&mtsCommandBase::Disable);
        EventHandlersWrite.ForEachVoid(&mtsCommandBase::Disable);
    }

    inline void EnableAllEvents(void) {
        EventHandlersVoid.ForEachVoid(&mtsCommandBase::Enable);
        EventHandlersWrite.ForEachVoid(&mtsCommandBase::Enable);
    }

    /*! Process any queued events. */
    unsigned int ProcessMailBoxes(void);

    /*! Send a human readable description of the interface. */
    void ToStream(std::ostream & outputStream) const;
    
protected:
#ifndef SWIG  // SWIG cannot deal with this
    template <class _commandType>
    class CommandInfo {
        _commandType **CommandPointer;
        bool IsRequired;
    public:
        CommandInfo(_commandType *&commandPointer, bool isReq):
            CommandPointer(&commandPointer),
            IsRequired(isReq)
        {}

        ~CommandInfo() {}
        
        inline void Clear(void) {
            *CommandPointer = 0;
        }
        
        bool Bind(_commandType *cmd)
        {  *CommandPointer = cmd;
           return cmd || !IsRequired;
        }
        void ToStream(std::ostream & outputStream) const
        {
            outputStream << *CommandPointer;
            if (!IsRequired) {
                outputStream << " (optional)";
            } else {
                outputStream << " (required)";
            }
        }
    };
    
#endif // !SWIG
        
    /*! Typedef for a map of name of zero argument command and name of command. */
    typedef cmnNamedMap<CommandInfo<mtsCommandVoidBase> > CommandPointerVoidMapType;
    CommandPointerVoidMapType CommandPointersVoid; // Void (command)
    
    /*! Typedef for a map of name of one argument command and name of command. */
    typedef cmnNamedMap<CommandInfo<mtsCommandReadBase> > CommandPointerReadMapType;
    CommandPointerReadMapType CommandPointersRead; // Read (state read)
    
    /*! Typedef for a map of name of one argument command and name of command. */
    typedef cmnNamedMap<CommandInfo<mtsCommandWriteBase> > CommandPointerWriteMapType;
    CommandPointerWriteMapType CommandPointersWrite; // Write (command)
    
    /*! Typedef for a map of name of two argument command and name of command. */
    typedef cmnNamedMap<CommandInfo<mtsCommandQualifiedReadBase> > CommandPointerQualifiedReadMapType;
    CommandPointerQualifiedReadMapType CommandPointersQualifiedRead; // Qualified Read (conversion, read at time index, ...)
    
    /*! Typedef for a map of event name and event handler (command object). */
    typedef cmnNamedMap<mtsCommandVoidBase> EventHandlerVoidMapType;
    typedef cmnNamedMap<mtsCommandWriteBase> EventHandlerWriteMapType;
    EventHandlerVoidMapType EventHandlersVoid;
    EventHandlerWriteMapType EventHandlersWrite;

public:

    bool AddCommandPointer(const std::string & commandName, mtsCommandVoidBase *& commandPointer, bool required = true)
    {
        return CommandPointersVoid.AddItem(commandName, new CommandInfo<mtsCommandVoidBase>(commandPointer, required));
    }

    bool AddCommandPointer(const std::string & commandName, mtsCommandReadBase *& commandPointer, bool required = true)
    {
        return CommandPointersRead.AddItem(commandName, new CommandInfo<mtsCommandReadBase>(commandPointer, required));
    }

    bool AddCommandPointer(const std::string & commandName, mtsCommandWriteBase *& commandPointer, bool required = true)
    {
        return CommandPointersWrite.AddItem(commandName, new CommandInfo<mtsCommandWriteBase>(commandPointer, required));
    }
    
    bool AddCommandPointer(const std::string & commandName, mtsCommandQualifiedReadBase *& commandPointer, bool required = true)
    {
        return CommandPointersQualifiedRead.AddItem(commandName, new CommandInfo<mtsCommandQualifiedReadBase>(commandPointer, required));
    }

    // Maybe make this a templated function?
    template <class _FunctionType>
    bool AddFunction(const std::string & commandName, _FunctionType & func, bool required = true)
    {
        return func.AddToRequiredInterface(*this, commandName, required);
    }

    template <class __classType>
    inline mtsCommandVoidBase * AddEventHandlerVoid(void (__classType::*method)(void),
                                                    __classType * classInstantiation,
                                                    const std::string & eventName,
                                                    bool queued = true);
    
    inline mtsCommandVoidBase * AddEventHandlerVoid(void (*function)(void),
                                                    const std::string & eventName,
                                                    bool queued = true);
    
    template <class __classType, class __argumentType>
    inline mtsCommandWriteBase * AddEventHandlerWrite(void (__classType::*method)(const __argumentType &),
                                                      __classType * classInstantiation,
                                                      const std::string & eventName,
                                                      bool queued = true);

};


#ifndef SWIG
template <class __classType>
inline mtsCommandVoidBase * mtsRequiredInterface::AddEventHandlerVoid(void (__classType::*method)(void),
                                                                      __classType * classInstantiation,
                                                                      const std::string & eventName,
                                                                      bool queued) {
    mtsCommandVoidBase * actualCommand = new mtsCommandVoidMethod<__classType>(method, classInstantiation, eventName);
    if (queued) {
        if (MailBox)
            EventHandlersVoid.AddItem(eventName, new mtsCommandQueuedVoid(MailBox, actualCommand));
        else
            CMN_LOG_CLASS_INIT_ERROR << "No mailbox for queued event handler void " << eventName << std::endl;
    } else {
        EventHandlersVoid.AddItem(eventName, actualCommand);
    }
    return EventHandlersVoid.GetItem(eventName);
}


inline mtsCommandVoidBase * mtsRequiredInterface::AddEventHandlerVoid(void (*function)(void),
                                                                      const std::string & eventName,
                                                                      bool queued) {
    mtsCommandVoidBase * actualCommand = new mtsCommandVoidFunction(function, eventName);
    if (queued) {
        if (MailBox)
            EventHandlersVoid.AddItem(eventName, new mtsCommandQueuedVoid(MailBox, actualCommand));
        else
            CMN_LOG_CLASS_INIT_ERROR << "No mailbox for queued event handler void(func) " << eventName << std::endl;
    } else {
        EventHandlersVoid.AddItem(eventName, actualCommand);
    }
    return EventHandlersVoid.GetItem(eventName);
}


template <class __classType, class __argumentType>
inline mtsCommandWriteBase * mtsRequiredInterface::AddEventHandlerWrite(void (__classType::*method)(const __argumentType &),
                                                                        __classType * classInstantiation,
                                                                        const std::string & eventName,
                                                                        bool queued) {
    mtsCommandWriteBase * actualCommand =
        new mtsCommandWrite<__classType, __argumentType>(method, classInstantiation, eventName, __argumentType());
    if (queued) {
        if (MailBox)
            EventHandlersWrite.AddItem(eventName,  new mtsCommandQueuedWrite<__argumentType>(MailBox, actualCommand, DEFAULT_ARG_BUFFER_LEN));
        else
            CMN_LOG_CLASS_INIT_ERROR << "No mailbox for queued event handler write " << eventName << std::endl;
    } else {
        EventHandlersWrite.AddItem(eventName, actualCommand);
    }
    return EventHandlersWrite.GetItem(eventName);
}
#endif  // !SWIG


/*! Stream out operator. */
template <class _commandType>
inline std::ostream & operator << (std::ostream & output,
                                   const mtsRequiredInterface::CommandInfo<_commandType> & req) {
    req.ToStream(output);
    return output;
}

CMN_DECLARE_SERVICES_INSTANTIATION(mtsRequiredInterface)


#endif // _mtsRequiredInterface_h

