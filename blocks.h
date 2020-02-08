//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	/* {"", "cat /tmp/recordingicon",	0,	9}, */
	/* {"",	"music",	0,	11}, */
	{"",	"pacpackages",	0,	8},
	{"",	"torrent",	20,	7},
	{"",	"news",		0,	6},
	{"",	"weather",	18000,	5},
	{"",	"mailbox",	180,	12},
	{"",	"volume",	0,	10},
	{"",	"battery",	5,	0},
	{"",	"clock",	60,	0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim = '|';
