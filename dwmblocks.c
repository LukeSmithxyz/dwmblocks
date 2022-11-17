#define _POSIX_C_SOURCE 200112L

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <X11/Xlib.h>
#include <pthread.h>
#include <err.h>
#define LENGTH(X) (sizeof(X) / sizeof (X[0]))
#define CMDLENGTH		50

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	unsigned int signal;
} Block;
typedef struct Thread_ev {
    const Block *block;
    size_t idx;
} Thread_ev;
void sighandler(int num);
void buttonhandler(int sig, siginfo_t *si, void *ucontext);
void replace(char *str, char old, char new);
void remove_all(char *str, char to_remove);
void getcmds(void);
#ifndef __OpenBSD__
void getsigcmds(int signal);
void setupsignals();
void sighandler(int signum);
#endif
int getstatus(char *str, char *last);
void setroot();
void statusloop();
void termhandler(int signum);


#include "config.h"

static Display *dpy;
static int screen;
static Window root;
static char statusbar[LENGTH(blocks)][CMDLENGTH] = {0};
static char statusstr[2][256];
static volatile int statusContinue = 1;
static pthread_mutex_t write_mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t cmd_mut = PTHREAD_MUTEX_INITIALIZER;
static void (*writestatus) () = setroot;

void replace(char *str, char old, char new)
{
	for(char * c = str; *c; c++)
		if(*c == old)
			*c = new;
}

// the previous function looked nice but unfortunately it didnt work if to_remove was in any position other than the last character
// theres probably still a better way of doing this
void remove_all(char *str, char to_remove) {
	char *read = str;
	char *write = str;
	while (*read) {
		if (*read != to_remove) {
			*write++ = *read;
		}
		++read;
	}
	*write = '\0';
}

//opens process *cmd and stores output in *output
void getcmd(const Block *block, char *output)
{
    pthread_mutex_lock(&cmd_mut);
	if (block->signal)
	{
		output[0] = block->signal;
		output++;
	}
	char *cmd = block->command;
	FILE *cmdf = popen(cmd,"r");
	if (!cmdf){
        //printf("failed to run: %s, %d\n", block->command, errno);
        pthread_mutex_unlock(&cmd_mut);
		return;
    }
    char tmpstr[CMDLENGTH] = "";
    // TODO decide whether its better to use the last value till next time or just keep trying while the error was the interrupt
    // this keeps trying to read if it got nothing and the error was an interrupt
    //  could also just read to a separate buffer and not move the data over if interrupted
    //  this way will take longer trying to complete 1 thing but will get it done
    //  the other way will move on to keep going with everything and the part that failed to read will be wrong till its updated again
    // either way you have to save the data to a temp buffer because when it fails it writes nothing and then then it gets displayed before this finishes
	char * s;
    int e;
    do {
        errno = 0;
        s = fgets(tmpstr, CMDLENGTH-(strlen(delim)+1), cmdf);
        e = errno;
    } while (!s && e == EINTR);
	pclose(cmdf);
	int i = strlen(block->icon);
	strcpy(output, block->icon);
    strcpy(output+i, tmpstr);
	remove_all(output, '\n');
	i = strlen(output);
    if ((i > 0 && block != &blocks[LENGTH(blocks) - 1])){
        strcat(output, delim);
    }
    i+=strlen(delim);
	output[i++] = '\0';
    pthread_mutex_unlock(&cmd_mut);
}

static void syncwrite(void) {
    pthread_mutex_lock(&write_mut);
    writestatus();
    pthread_mutex_unlock(&write_mut);
}

void threadEvent(union sigval data) {
    Thread_ev *ev = (Thread_ev*) data.sival_ptr;
    getcmd(ev->block, statusbar[ev->idx]);
    syncwrite();
}

void getcmds(void)
{
	const Block* current;
	for(int i = 0; i < LENGTH(blocks); i++)
	{
		current = blocks + i;
		getcmd(current,statusbar[i]);
	}
}

#ifndef __OpenBSD__
void getsigcmds(int signal)
{
	const Block *current;
	for (int i = 0; i < LENGTH(blocks); i++)
	{
		current = blocks + i;
		if (current->signal == signal){
			getcmd(current,statusbar[i]);
        }
	}
}

void setupsignals()
{
	struct sigaction sa;

	for(int i = SIGRTMIN; i <= SIGRTMAX; i++)
		signal(i, SIG_IGN);

	for(int i = 0; i < LENGTH(blocks); i++)
	{
		if (blocks[i].signal > 0)
		{
            struct sigaction action;
            sigemptyset(&action.sa_mask);
            action.sa_flags = 0;
            action.sa_handler = sighandler;

            // Not checking if sigaction returns -1
            sigaction(SIGRTMIN+blocks[i].signal, &action, NULL);
			sigaddset(&sa.sa_mask, SIGRTMIN+blocks[i].signal);
		}
	}
	sa.sa_sigaction = buttonhandler;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa, NULL);
	struct sigaction sigchld_action = {
  		.sa_handler = SIG_DFL,
  		.sa_flags = SA_NOCLDWAIT
	};
	sigaction(SIGCHLD, &sigchld_action, NULL);

}
#endif

int getstatus(char *str, char *last)
{
	strcpy(last, str);
	str[0] = '\0';
    for(int i = 0; i < LENGTH(blocks); i++) {
		strcat(str, statusbar[i]);
        if (i == LENGTH(blocks) - 1)
            strcat(str, " ");
    }
	str[strlen(str)-1] = '\0';
	return strcmp(str, last);//0 if they are the same
}

void setroot()
{
	if (!getstatus(statusstr[0], statusstr[1]))//Only set root if text has changed.
		return;
	Display *d = XOpenDisplay(NULL);
	if (d) {
		dpy = d;
	}
	screen = DefaultScreen(dpy);
	root = RootWindow(dpy, screen);
	XStoreName(dpy, root, statusstr[0]);
	XCloseDisplay(dpy);
}

void pstdout()
{
	if (!getstatus(statusstr[0], statusstr[1]))//Only write out if text has changed.
		return;
    printf("%s\n",statusstr[0]);
	fflush(stdout);
}


void statusloop()
{
#ifndef __OpenBSD__
	setupsignals();
#endif
    // first figure out the default wait interval by finding the
    // greatest common denominator of the intervals
	setupsignals();
	getcmds();

    sigset_t blocked;
    sigemptyset(&blocked);

    timer_t timer_ids[LENGTH(blocks)];
    Thread_ev evs[LENGTH(blocks)];

    size_t timer_size = 0;
    size_t ev_size = 0;

    for(size_t i = 0; i < LENGTH(blocks); i++) {
        const Block *current = blocks + i;
        if(current->signal > 0) {
            sigaddset(&blocked, current->signal);
        }
    }

    sigprocmask(SIG_BLOCK, &blocked, NULL);

    for(size_t i = 0; i < LENGTH(blocks); i++) {
        const Block *current = blocks + i;

        timer_t t_id;
        Thread_ev ev;

        timer_ids[timer_size] = t_id;
        evs[ev_size] = ev;


        struct sigevent event;

        if(current->signal == 0) {
            evs[ev_size].block = current;
            evs[ev_size].idx = i;

            event.sigev_notify = SIGEV_THREAD;
            event.sigev_value.sival_ptr = &evs[ev_size];
            event.sigev_notify_function = threadEvent;
        } else {
            event.sigev_notify = SIGEV_SIGNAL;
            event.sigev_signo = current->signal + SIGRTMIN;
        }

        if(timer_create(CLOCK_REALTIME, &event, &timer_ids[timer_size]) == -1)
            err(1, "Failed to create timer");


        struct itimerspec inter;

        inter.it_value.tv_sec = 1;
        inter.it_value.tv_nsec = 0;

        inter.it_interval.tv_sec = current->interval;
        inter.it_interval.tv_nsec = 0;

        if(timer_settime(timer_ids[timer_size], 0, &inter, NULL) == -1) {
            err(1, "Failed to set timer");
        }

        timer_size++;
        ev_size++;
    }

    sigprocmask(SIG_UNBLOCK, &blocked, NULL);

    while(statusContinue) {
        syncwrite();
        pause();
    }

    for(size_t idx = 0; idx < timer_size; idx++) {
        timer_delete(timer_ids[idx]);
    }
}

#ifndef __OpenBSD__
void sighandler(int signum)
{
	getsigcmds(signum-SIGRTMIN);
	writestatus();
}

void buttonhandler(int sig, siginfo_t *si, void *ucontext)
{
	char button[2] = {'0' + si->si_value.sival_int & 0xff, '\0'};
	pid_t process_id = getpid();
	sig = si->si_value.sival_int >> 8;
	if (fork() == 0)
	{
		const Block *current;
		for (int i = 0; i < LENGTH(blocks); i++)
		{
			current = blocks + i;
			if (current->signal == sig)
				break;
		}
		char shcmd[1024];
		sprintf(shcmd,"%s && kill -%d %d",current->command, current->signal+34,process_id);
		char *command[] = { "/bin/sh", "-c", shcmd, NULL };
		setenv("BLOCK_BUTTON", button, 1);
		setsid();
		execvp(command[0], command);
		exit(EXIT_SUCCESS);
	}
}

#endif

void termhandler(int signum)
{
	statusContinue = 0;
	exit(0);
}

int main(int argc, char** argv)
{
	for(int i = 0; i < argc; i++)
	{
		if (!strcmp("-d",argv[i]))
			delim = argv[++i];
		else if(!strcmp("-p",argv[i]))
			writestatus = pstdout;
	}
	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);
	statusloop();
}
