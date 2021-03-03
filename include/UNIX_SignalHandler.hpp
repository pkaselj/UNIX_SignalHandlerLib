#ifndef UNIX_SIGNAL_HANDLER_HPP
#define UNIX_SIGNAL_HANDLER_HPP


#include<signal.h>

#include<unordered_map>

#include"Kernel.hpp"

class SignalHandlerFunctor;

class UNIX_SignalHandler
{
public:
    typedef enum : int
    {
        enuSIGABRT = SIGABRT,
        enuSIGALRM = SIGALRM,
        enuSIGBUS = SIGBUS,
        enuSIGCHLD = SIGCHLD,
        enuSIGCLD = SIGCLD,
        enuSIGCONT = SIGCONT,
        // enuSIGEMT = SIGEMT,
        enuSIGFPE = SIGFPE,
        enuSIGHUP = SIGHUP,
        enuSIGILL = SIGILL,
        // enuSIGINFO = SIGINFO,
        enuSIGPWR = SIGPWR,
        enuSIGINT = SIGINT,
        enuSIGIO = SIGIO,
        enuSIGIOT = SIGIOT,
        enuSIGKILL = SIGKILL,
        // enuSIGLOST = SIGLOST,
        enuSIGPIPE = SIGPIPE,
        enuSIGPOLL = SIGPOLL,
        enuSIGPROF = SIGPROF,
        enuSIGQUIT = SIGQUIT,
        enuSIGSEGV = SIGSEGV,
        enuSIGSTKFLT = SIGSTKFLT,
        enuSIGSTOP = SIGSTOP,
        enuSIGTSTP = SIGTSTP,
        enuSIGSYS = SIGSYS,
        enuSIGTERM = SIGTERM,
        enuSIGTRAP = SIGTRAP,
        enuSIGTTIN = SIGTTIN,
        enuSIGTTOU = SIGTTOU,
        // enuSIGUNUSED = SIGUNUSED,
        enuSIGURG = SIGURG,
        enuSIGUSR1 = SIGUSR1,
        enuSIGUSR2 = SIGUSR2,
        enuSIGVTALRM = SIGVTALRM,
        enuSIGXCPU = SIGXCPU,
        enuSIGXFSZ = SIGXFSZ,
        enuSIGWINCH = SIGWINCH
    } enuSignal;

    typedef void (*signal_handler_function)(int, siginfo_t*, void*);
    typedef struct sigaction signal_handler_properties;


    private:
    UNIX_SignalHandler();
    UNIX_SignalHandler& operator= (UNIX_SignalHandler&);
    UNIX_SignalHandler(UNIX_SignalHandler&);

    static sigset_t* m_pSignalMask;
    static std::unordered_map<enuSignal, SignalHandlerFunctor*> m_SignalToFunctorMap;

    public:
    static void SetSignalHandle(enuSignal signal, signal_handler_function handler); // Unblock signal first TODO?
    static void SetSignalHandle(enuSignal signal, SignalHandlerFunctor* pFunctor);
    static void ResetSignalHandlerToDefault(enuSignal signal);
    static void IgnoreSignal(enuSignal signal);

    static SignalHandlerFunctor* getFunctorForSignal(enuSignal signal);
    static std::string getSignalName(enuSignal signal);
    static void sendSignal(unsigned int PID, enuSignal signal);

    static void BlockAll();
    static void UnblockAll();
    static void BlockSignal(enuSignal signal);
    static void UnblockSignal(enuSignal signal);
    static void RestoreLastSignalMask();

    // add block and unblock for multiple signals

    //static bool isSignalBlocked(int signal);
};

class SignalHandlerFunctor
{
public:
    virtual void operator()(void* params) = 0;
};

#endif


