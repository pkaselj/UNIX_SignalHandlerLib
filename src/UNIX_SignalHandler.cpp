#include "UNIX_SignalHandler.hpp"

#include <algorithm>
#include<csignal>

std::unordered_map<UNIX_SignalHandler::enuSignal, SignalHandlerFunctor*> UNIX_SignalHandler::m_SignalToFunctorMap = {
                                        {enuSIGALRM, nullptr},
                                        {enuSIGBUS, nullptr},
                                        {enuSIGCLD, nullptr},
                                        {enuSIGCHLD, nullptr},
                                        {enuSIGCONT, nullptr},
                                        {enuSIGFPE, nullptr},
                                        {enuSIGHUP, nullptr},
                                        {enuSIGILL, nullptr},
                                        {enuSIGINT, nullptr},
                                        {enuSIGIOT, nullptr},
                                        {enuSIGABRT, nullptr},
                                        {enuSIGKILL, nullptr},
                                        {enuSIGPIPE, nullptr},
                                        {enuSIGPOLL, nullptr},
                                        {enuSIGIO, nullptr},
                                        {enuSIGPROF, nullptr},
                                        {enuSIGPWR, nullptr},
                                        {enuSIGQUIT, nullptr},
                                        {enuSIGSEGV, nullptr},
                                        {enuSIGSTKFLT, nullptr},
                                        {enuSIGSTOP, nullptr},
                                        {enuSIGTSTP, nullptr},
                                        {enuSIGTERM, nullptr},
                                        {enuSIGTRAP, nullptr},
                                        {enuSIGTTIN, nullptr},
                                        {enuSIGTTOU, nullptr},
                                        {enuSIGSYS, nullptr},
                                        {enuSIGURG, nullptr},
                                        {enuSIGUSR1, nullptr},
                                        {enuSIGUSR2, nullptr},
                                        {enuSIGVTALRM, nullptr},
                                        {enuSIGXCPU, nullptr},
                                        {enuSIGXFSZ, nullptr},
                                        {enuSIGWINCH, nullptr},
};

std::string UNIX_SignalHandler::getSignalName(enuSignal signal)
{
    switch (signal)
    {
    case enuSIGABRT:
        return "SIGABRT";

    case enuSIGALRM:
        return "SIGALRM";

    case enuSIGBUS:
        return "SIGBUS";

    case enuSIGCHLD:
        return "SIGCHLD";

    case enuSIGCONT:
        return "SIGCONT";

    case enuSIGFPE:
        return "SIGFPE";

    case enuSIGHUP:
        return "SIGHUP";

    case enuSIGILL:
        return "SIGILL";

    case enuSIGPWR:
        return "SIGPWR";

    case enuSIGINT:
        return "SIGINT";

    case enuSIGIO:
        return "SIGIO";

    case enuSIGKILL:
        return "SIGKILL";

    case enuSIGPIPE:
        return "SIGPIPE";

    case enuSIGPROF:
        return "SIGPROF";

    case enuSIGQUIT:
        return "SIGQUIT";

    case enuSIGSEGV:
        return "SIGSEGV";

    case enuSIGSTKFLT:
        return "SIGSTKFLT";

    case enuSIGSTOP:
        return "SIGSTOP";

    case enuSIGTSTP:
        return "SIGTSTP";

    case enuSIGSYS:
        return "SIGSYS";

    case enuSIGTERM:
        return "SIGTERM";

    case enuSIGTRAP:
        return "SIGTRAP";

    case enuSIGTTIN:
        return "SIGTTIN";

    case enuSIGTTOU:
        return "SIGTTOU";

    case enuSIGURG:
        return "SIGURG";

    case enuSIGUSR1:
        return "SIGUSR1";

    case enuSIGUSR2:
        return "SIGUSR2";

    case enuSIGVTALRM:
        return "SIGVTALRM";

    case enuSIGXCPU:
        return "SIGXCPU";

    case enuSIGXFSZ:
        return "SIGXFSZ";

    case enuSIGWINCH:
        return "SIGWINCH";

    default:
        return "INVALID SIGNAL!";
    }
}

void UNIX_SignalHandler::SetSignalHandle(enuSignal signal, signal_handler_function handler)
{
    if(handler == nullptr)
    {
        Kernel::Fatal_Error("Signal handler function cannot be NULL for signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    signal_handler_properties properties;
    properties.sa_flags = SA_SIGINFO;
    properties.sa_sigaction = handler;

    if(sigaction(signal, &properties, nullptr) < 0)
    {
        Kernel::Fatal_Error("Sigaction (signal handler assignment) error for signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Signal handler successfully assigned to signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) ); // Add signal handler identifier (address?)

}

SignalHandlerFunctor* UNIX_SignalHandler::getFunctorForSignal(enuSignal signal)
{
    return m_SignalToFunctorMap[signal];
}

void defaultSignalHandlerForFunctors(int signalVal, siginfo_t* pInfo, void* uu)
{
    UNIX_SignalHandler::enuSignal signal = static_cast<UNIX_SignalHandler::enuSignal>(signalVal);
    SignalHandlerFunctor* pSpecificHandler = UNIX_SignalHandler::getFunctorForSignal(signal);
    
    if (pSpecificHandler == nullptr)
    {
        Kernel::Fatal_Error("Signal: " + UNIX_SignalHandler::getSignalName(signal) + " - no callback!");
    }


    std::cout << "Generated signal! " << std::endl;
    // std::raise(SIGINT);

    void* pExpiredTimer_voidptr = pInfo->_sifields._rt.si_sigval.sival_ptr;

    // TODO EXPLAIN
    (*pSpecificHandler)(pExpiredTimer_voidptr);
    // (*pSpecificHandler)(nullptr);
}

void UNIX_SignalHandler::sendSignal(unsigned int PID, enuSignal signal)
{
    kill(PID, signal);
}

void UNIX_SignalHandler::SetSignalHandle(enuSignal signal, SignalHandlerFunctor* pFunctor)
{
    if (pFunctor == nullptr)
    {
        Kernel::Fatal_Error("Signal handler function cannot be NULL for signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()));
    }

    signal_handler_properties properties;
    properties.sa_flags = SA_SIGINFO;
    properties.sa_sigaction = defaultSignalHandlerForFunctors;

    if (sigaction(signal, &properties, nullptr) < 0)
    {
        Kernel::Fatal_Error("Sigaction (signal handler assignment) error for signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()));
    }

    m_SignalToFunctorMap[signal] = pFunctor;

    Kernel::Trace("Signal handler successfully assigned to signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid())); // Add signal handler identifier (address?)
}

void UNIX_SignalHandler::ResetSignalHandlerToDefault(enuSignal signal)
{
    signal_handler_properties properties;
    properties.sa_handler = SIG_DFL;

    if(sigaction(signal, &properties, nullptr) < 0)
    {
        Kernel::Fatal_Error("Sigaction (signal handler assignment) RESET TO DEFAULT error for signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    for (auto& pair : m_SignalToFunctorMap)
    {
        if (pair.first == signal)
        {
            pair.second = nullptr;
        }
    }


    Kernel::Trace("Signal handler successfully RESET the signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) ); // Add signal handler identifier (address?)
}

void UNIX_SignalHandler::IgnoreSignal(enuSignal signal)
{
    signal_handler_properties properties;
    properties.sa_handler = SIG_IGN;

    if(sigaction(signal, &properties, nullptr) < 0)
    {
        Kernel::Fatal_Error("Sigaction (signal handler assignment) IGNORE error for signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Signal handler successfully SET TO IGNORE the signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) ); // Add signal handler identifier (address?)
}

void UNIX_SignalHandler::BlockAll()
{
    sigset_t newSignalMask;

    if( sigfillset(&newSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Sigfillset error - can't block all signals PID: " + std::to_string(getpid()) );
    }

    if( pthread_sigmask(SIG_SETMASK, &newSignalMask, m_pSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Pthread_sigmask error - can't block all signals PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Blocked all signals PID: " + std::to_string(getpid()) );
}

void UNIX_SignalHandler::UnblockAll()
{
    sigset_t newSignalMask ;

    if( sigemptyset(&newSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Sigempty error - can't unblock all signals PID: " + std::to_string(getpid()) );
    }

    if( pthread_sigmask(SIG_SETMASK, &newSignalMask, m_pSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Pthread_sigmask error - can't unblock all signals PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("unblocked all signals PID: " + std::to_string(getpid()) );
}

void UNIX_SignalHandler::BlockSignal(enuSignal signal)
{
    sigset_t newSignalMask;

    if( sigemptyset(&newSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Sigempty error - can't block signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    if( sigaddset(&newSignalMask, signal) < 0 )
    {
        Kernel::Fatal_Error("Sigaddset error - can't block signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }


    /* Do not change 'Previous Signal Mask'
    , so that multiple single-blocks can be reset with RestoreLastSignalMask(),
     except if there is no Previous Signal Mask */

    sigset_t* pOldSignalMaskContainer = (m_pSignalMask == nullptr) ? m_pSignalMask : nullptr;

    if( pthread_sigmask(SIG_BLOCK, &newSignalMask, pOldSignalMaskContainer) < 0 ) 
    {
        Kernel::Fatal_Error("Pthread_sigmask error - can't block signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Blocked signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
}

void UNIX_SignalHandler::UnblockSignal(enuSignal signal)
{
    sigset_t newSignalMask ;

    if( sigemptyset(&newSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Sigempty error - can't unblock signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    if( sigaddset(&newSignalMask, signal) < 0 )
    {
        Kernel::Fatal_Error("Sigaddset error - can't unblock signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }


    /* Do not change 'Previous Signal Mask'
    , so that multiple single-blocks can be reset with RestoreLastSignalMask(),
     except if there is no Previous Signal Mask */

    sigset_t** pOldSignalMaskContainer = (m_pSignalMask == nullptr) ? &m_pSignalMask : nullptr;

    if( pthread_sigmask(SIG_UNBLOCK, &newSignalMask, *pOldSignalMaskContainer) < 0 ) 
    {
        Kernel::Fatal_Error("Pthread_sigmask error - can't unblock signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Unblocked signal: " + getSignalName(signal) + " PID: " + std::to_string(getpid()) );
}

void UNIX_SignalHandler::RestoreLastSignalMask()
{

    sigset_t allowAllSignals;
    sigemptyset(&allowAllSignals);

    if(m_pSignalMask == nullptr)
    {
        Kernel::Warning("Requested previous signal mask unknown - nullptr! PID: " + std::to_string(getpid()) );
        m_pSignalMask = &allowAllSignals;
    }

    sigset_t previousSignalMask;
    sigemptyset(&previousSignalMask);

    if( pthread_sigmask(SIG_SETMASK, m_pSignalMask, &previousSignalMask) < 0 ) 
    {
        Kernel::Fatal_Error("Pthread_sigmask error - can't reset signal mask PID: " + std::to_string(getpid()) );
    }

    *m_pSignalMask = previousSignalMask;

    Kernel::Trace("Reset signal mask PID: " + std::to_string(getpid()) );
}

/*bool UNIX_SignalHandler::isSignalBlocked(int signal)
{
    return sigismember()
}*/

sigset_t* UNIX_SignalHandler::m_pSignalMask = nullptr;