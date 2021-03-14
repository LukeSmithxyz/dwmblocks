//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	/* {"",	"music",	0,	11},*/
	/* {"",	"pacpackages",	0,	8}, */
	/* {"",	"memory",	10,	14}, */
	/* {"",	"weather",	18000,	5}, */
	/* {"",	"mailbox",	180,	12}, */
	/* {"",	"nettraf",	1,	16}, */
	/* {"",	"volume",	0,	10}, */
    /* {"",	"internet",	5,	4}, */
    {"",    "stalonetray.sh", 0, 27},
    /* {"?",   "sleep 2s && date", 1, 8}, */
    {"",	"cpupercent.sh",	1,	19},
	{"",	"battery",	5,	3},
	{"",	"clock",	1,	1},
};

//Sets delimiter between status commands. NULL character ('\0') means no delimiter.
static char *delim = "|";

// Have dwmblocks automatically recompile and run when you edit this file in
// vim with the following line in your vimrc/init.vim:

// autocmd BufWritePost ~/.local/src/dwmblocks/config.h !cd ~/.local/src/dwmblocks/; sudo make install && { killall -q dwmblocks;setsid dwmblocks & }
