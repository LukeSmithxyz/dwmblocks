//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	{"", "cat /tmp/recordingicon",	0,	9},
	{"📬 ", "find ~/.local/share/mail/*/INBOX/new -type f | wc -l", 0, 13},
	{"🔊 ", "amixer get Master | grep -o \"\\(\\[off\\]\\|[0-9]*%\\)\"", 0, 10},
	{"🔋 ", "sed \"s/$/%/\" /sys/class/power_supply/BAT?/capacity", 5, 12},
	{"🕗 ", "date '+%Y %b %d (%a) %I:%M%p'",	60,	0},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim = ' ';
