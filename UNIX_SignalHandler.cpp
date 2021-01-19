#include "UNIX_SignalHandler.hpp"

void UNIX_SignalHandler::SetSignalHandle(int signal, signal_handler_function handler)
{
    //if(signal) // WRONG SIGNAL ? TODO
    if(handler == nullptr)
    {
        Kernel::Fatal_Error("Signal handler function cannot be NULL for signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    signal_handler_properties properties;
    properties.sa_flags = SA_SIGINFO;
    properties.sa_sigaction = handler;

    if(sigaction(signal, &properties, nullptr) < 0)
    {
        Kernel::Fatal_Error("Sigaction (signal handler assignment) error for signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Signal handler successfully assigned to signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) ); // Add signal handler identifier (address?)

}

void UNIX_SignalHandler::ResetSignalHandlerToDefault(int signal)
{
    signal_handler_properties properties;
    properties.sa_handler = SIG_DFL;

    if(sigaction(signal, &properties, nullptr) < 0)
    {
        Kernel::Fatal_Error("Sigaction (signal handler assignment) RESET TO DEFAULT error for signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Signal handler successfully RESET the signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) ); // Add signal handler identifier (address?)
}

void UNIX_SignalHandler::IgnoreSignal(int signal)
{
    signal_handler_properties properties;
    properties.sa_handler = SIG_IGN;

    if(sigaction(signal, &properties, nullptr) < 0)
    {
        Kernel::Fatal_Error("Sigaction (signal handler assignment) IGNORE error for signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Signal handler successfully SET TO IGNORE the signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) ); // Add signal handler identifier (address?)
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

void UNIX_SignalHandler::BlockSignal(int signal)
{
    sigset_t newSignalMask;

    if( sigemptyset(&newSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Sigempty error - can't block signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    if( sigaddset(&newSignalMask, signal) < 0 )
    {
        Kernel::Fatal_Error("Sigaddset error - can't block signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }


    /* Do not change 'Previous Signal Mask'
    , so that multiple single-blocks can be reset with RestoreLastSignalMask(),
     except if there is no Previous Signal Mask */

    sigset_t* pOldSignalMaskContainer = (m_pSignalMask == nullptr) ? m_pSignalMask : nullptr;

    if( pthread_sigmask(SIG_BLOCK, &newSignalMask, pOldSignalMaskContainer) < 0 ) 
    {
        Kernel::Fatal_Error("Pthread_sigmask error - can't block signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Blocked signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
}

void UNIX_SignalHandler::UnblockSignal(int signal)
{
    sigset_t newSignalMask ;

    if( sigemptyset(&newSignalMask) < 0 )
    {
        Kernel::Fatal_Error("Sigempty error - can't unblock signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    if( sigaddset(&newSignalMask, signal) < 0 )
    {
        Kernel::Fatal_Error("Sigaddset error - can't unblock signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }


    /* Do not change 'Previous Signal Mask'
    , so that multiple single-blocks can be reset with RestoreLastSignalMask(),
     except if there is no Previous Signal Mask */

    sigset_t** pOldSignalMaskContainer = (m_pSignalMask == nullptr) ? &m_pSignalMask : nullptr;

    if( pthread_sigmask(SIG_UNBLOCK, &newSignalMask, *pOldSignalMaskContainer) < 0 ) 
    {
        Kernel::Fatal_Error("Pthread_sigmask error - can't unblock signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
    }

    Kernel::Trace("Unblocked signal: " + std::to_string(signal) + " PID: " + std::to_string(getpid()) );
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