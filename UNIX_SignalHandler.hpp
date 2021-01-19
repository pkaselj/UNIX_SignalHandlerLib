#ifndef UNIX_SIGNAL_HANDLER_HPP
#define UNIX_SIGNAL_HANDLER_HPP


#include<signal.h>

#include"/home/pi/Shared/KernelLib/0.0.0/Kernel.hpp"

class UNIX_SignalHandler
{
    private:
    UNIX_SignalHandler();
    UNIX_SignalHandler& operator= (UNIX_SignalHandler&);
    UNIX_SignalHandler(UNIX_SignalHandler&);

    static sigset_t* m_pSignalMask;

    public:
    typedef void (*signal_handler_function)(int, siginfo_t*, void*);
    typedef struct sigaction signal_handler_properties;

    static void SetSignalHandle(int signal, signal_handler_function handler); // Unblock signal first TODO?
    static void ResetSignalHandlerToDefault(int signal);
    static void IgnoreSignal(int signal);

    static void BlockAll();
    static void UnblockAll();
    static void BlockSignal(int signal);
    static void UnblockSignal(int signal);
    static void RestoreLastSignalMask();

    // add block and unblock for multiple signals

    //static bool isSignalBlocked(int signal);
};

sigset_t* UNIX_SignalHandler::m_pSignalMask = nullptr;

#endif


