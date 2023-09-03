#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <X11/Xlib.h>
#define LENGTH(X) (sizeof(X) / sizeof (X[0]))
#define MAX_CMD_OUTPUT_LENGTH 50
#define MAX_STATUS_LENGTH 256

typedef struct {
	char* icon;
	char* command;
	unsigned int interval;
	unsigned int signal;
} Block;

#include "config.h"

void sighandler(int num);
void buttonhandler(int sig, siginfo_t *si, void *ucontext);
void replace(char *str, char old, char new);
void remove_all(char *str, char to_remove);
void updateStatusMessages(int time);
#ifndef __OpenBSD__
void updateStatusMessagesBySignal(int signal);
void setupSignals();
void sighandler(int signum);
#endif
void updateWindowName();
void statusloop();
int calculateUpdateInterval();
void termhandler(int signum);

static char statusMessages[LENGTH(blocks)][MAX_CMD_OUTPUT_LENGTH] = { 0 };
static char statusBarOutput[MAX_STATUS_LENGTH];
static char oldStatusBarOutput[MAX_STATUS_LENGTH];
static int statusContinue = 1;
static void (*writeStatus) () = updateWindowName;

void replace(char *str, char old, char new) {
	for (char * c = str; *c; c++)
		if (*c == old)
			*c = new;
}

// The previous function looked nice but unfortunately it didnt work if to_remove was in any position other than the last character
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

int gcd(int a, int b)
{
	int temp;
	while (b > 0) {
		temp = a % b;

		a = b;
		b = temp;
	}
	return a;
}

void setStatusBlock(const Block *block, char *output, char* cmdOutput) {
	int statusLength = strlen(block->icon);
	strcpy(output, block->icon);
	strcpy(output + statusLength, cmdOutput);
	remove_all(output, '\n');
	statusLength = strlen(output);

	int isNotLastBlock = block != &blocks[LENGTH(blocks) - 1];
	int shouldAddDelim = statusLength > 0 && isNotLastBlock;
	if (shouldAddDelim) {
		strcat(output, delim);
		// XXX: This line was outside the if block. Please test it.
		statusLength += strlen(delim);
	}
	output[statusLength++] = '\0';
}

char* runCommand(const Block *block, char *cmdOutput) {
	char *cmd = block->command;
	FILE *cmdFd = popen(cmd, "r");

	if (!cmdFd) {
		return NULL;
	}

	// TODO: Decide whether its better to use the last value till next time or just keep trying while the error was the interrupt
	// this keeps trying to read if it got nothing and the error was an interrupt
	// could also just read to a separate buffer and not move the data over if interrupted
	// this way will take longer trying to complete 1 thing but will get it done
	// the other way will move on to keep going with everything and the part that failed to read will be wrong till its updated again
	// either way you have to save the data to a temp buffer because when it fails it writes nothing and then then it gets displayed before this finishes
	char * s;
	int e;

	do {
		errno = 0;
		s = fgets(cmdOutput, MAX_CMD_OUTPUT_LENGTH - (strlen(delim) + 1), cmdFd);
		e = errno;
	} while (!s && e == EINTR);
	pclose(cmdFd);

	return cmdOutput;
}

void updateStatusMessage(const Block *block, char *output)
{
	if (block->signal != 0) {
		output[0] = block->signal;
		output++;
	}

	char cmdOutput[MAX_CMD_OUTPUT_LENGTH] = "";
	runCommand(block, cmdOutput);

	setStatusBlock(block, output, cmdOutput);
}

void updateStatusMessages(int time)
{
	const Block* block;

	for (int i = 0; i < LENGTH(blocks); i++) {
		block = blocks + i;

		int forceUpdate = time == -1;
		int shouldUpdate = block->interval != 0 && time % block->interval == 0;
		if (forceUpdate || shouldUpdate) {
			updateStatusMessage(block, statusMessages[i]);
		}
	}
}

#ifndef __OpenBSD__
void updateStatusMessagesBySignal(int signal)
{
	const Block *current;
	for (int i = 0; i < LENGTH(blocks); i++) {
		current = blocks + i;
		if (current->signal == signal) {
			updateStatusMessage(current, statusMessages[i]);
		}
	}
}

void setupSignals()
{
	struct sigaction sa;

	for (int i = SIGRTMIN; i <= SIGRTMAX; i++)
		signal(i, SIG_IGN);

	for (int i = 0; i < LENGTH(blocks); i++) {
		if (blocks[i].signal > 0) {
			signal(SIGRTMIN+blocks[i].signal, sighandler);
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

// This function can be splitted into multiple functions, but
// I've added comments for the sake of simplicity.
int updateStatusBarOutput()
{
	// Cache previous output
	strcpy(oldStatusBarOutput, statusBarOutput);

	// Update status bar output
	statusBarOutput[0] = '\0';
	for (int i = 0; i < LENGTH(blocks); i++) {
		strcat(statusBarOutput, statusMessages[i]);
		if (i == LENGTH(blocks) - 1)
			strcat(statusBarOutput, " ");
	}
	statusBarOutput[strlen(statusBarOutput)-1] = '\0';

	// Check if it's been updated
	return strcmp(statusBarOutput, oldStatusBarOutput);
}

void updateWindowName()
{
	if (!updateStatusBarOutput())
		return;

	Display *display = XOpenDisplay(NULL);
	int screen = DefaultScreen(display);
	Window root = RootWindow(display, screen);

	XStoreName(display, root, statusBarOutput);

	XCloseDisplay(display);
}

void pstdout()
{
	if (!updateStatusBarOutput())
		return;
	printf("%s\n", statusBarOutput);
	fflush(stdout);
}

void statusloop()
{
#ifndef __OpenBSD__
	setupSignals();
#endif

	unsigned int interval = calculateUpdateInterval();
	const struct timespec sleeptime = { interval, 0 };
	struct timespec tosleep = sleeptime;

	// Initial update
	updateStatusMessages(-1);

	int interrupted = 0;
	unsigned int timeElapsed = 0;
	while (statusContinue) {
		interrupted = nanosleep(&tosleep, &tosleep);
		if (interrupted == -1) {
			continue;
		}

		updateStatusMessages(timeElapsed);
		writeStatus();

		timeElapsed += interval;
		tosleep = sleeptime;
	}
}

int calculateUpdateInterval() {
	unsigned int interval = -1;

	for (int i = 0; i < LENGTH(blocks); i++) {
		if (blocks[i].interval != 0) {
			interval = gcd(blocks[i].interval, interval);
		}
	}

	return interval;
}

#ifndef __OpenBSD__
void sighandler(int signum)
{
	updateStatusMessagesBySignal(signum - SIGRTMIN);
	writeStatus();
}

void buttonhandler(int sig, siginfo_t *si, void *ucontext)
{
	char button[2] = {'0' + si->si_value.sival_int & 0xff, '\0'};
	pid_t process_id = getpid();
	sig = si->si_value.sival_int >> 8;
	if (fork() == 0) {
		const Block *current;
		for (int i = 0; i < LENGTH(blocks); i++) {
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
	for (int i = 0; i < argc; i++) {
		if (!strcmp("-d", argv[i]))
			delim = argv[++i];
		else if (!strcmp("-p",argv[i]))
			writeStatus = pstdout;
	}

	signal(SIGTERM, termhandler);
	signal(SIGINT, termhandler);

	statusloop();
}
