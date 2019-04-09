
/*
	Module: segfault.cpp
    Author: Gary Conway <gary.conway@fireking.com>
    Created: 6-8-2012
    Revised:
    Compiler: C++
    Platform: Linux (Ubuntu)
    Notice: Copyright 2018 FireKing Security
    Version: 1.0

	this module handles all segfault issues, sets up a handler and receives signals
*/

using namespace std;

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>	// has EXIT_FAILURE etc. codes


// define this for production
#define EXTENDED_SEGFAULT


void TrapSegFault(void);
void SegFault(int sig);
void SegFaultExt(int sig,siginfo_t *si, void *arg);
void generate_segfault(void);
void TrapBrokenPipe(void);

int program_state=0;

extern void exit_cleanup(int status);
extern char gen_buffer[];
extern void error_exit(void);



//   cat /proc/PID/maps - shows some debugging information
//http://www.rt-embedded.com/blog/archives/resolving-crashes-and-segmentation-faults/
//http://www.makelinux.net/alp/095

void TrapSegFault(void)
{
#ifdef EXTENDED_SEGFAULT
	struct sigaction sa;
	memset(&sa,0,sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction=SegFaultExt;
	sa.sa_flags=SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);
#else
	signal(SIGSEGV,SegFault);
#endif
}

// extended version of segfault handler
// note that the address printed is the address the CPU was accessing when
// the error occured

void SegFaultExt(int sig, siginfo_t *si, void *arg)
{
    signal(SIGSEGV,SIG_DFL);    // revert back to prevent uglies
    printf("SEGFAULT: at address: %p, errno:%d, sigcode:%d, program_state:%d",si->si_addr,si->si_errno,si->si_code,program_state);
//    Logit(gen_buffer);
//    exit_cleanup(EXIT_FAILURE);
	error_exit();
}


// simple segfault handler
void SegFault(int sig)
{
	signal(SIGSEGV,SIG_DFL);	// revert back to prevent uglies
	printf("SEGFAULT: program state: %d",program_state);
//	Logit(gen_buffer);
//	exit_cleanup(EXIT_FAILURE);
	error_exit();
}


// for testing
void generate_segfault(void)
{
	//SEGFAULT generator
	char *p;
	strcpy(p,"test");
}


void TrapBrokenPipe(void)
{
	signal(SIGPIPE, SIG_IGN);
}





// EOM


