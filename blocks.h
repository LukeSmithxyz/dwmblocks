//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/		/*Update Interval*/	/*Update Signal*/
	/* {"", "cat /tmp/recordingicon",	0,	9}, */
	/* {"", "mpc status | grep -v ^volume | head -n 1", 0, 11}, */
	{"mail:", "find ~/.local/share/mail/*/INBOX/new -type f | wc -l", 0, 12},
	{"vol:", "amixer get Master | tail -n1 | sed -r 's/.*\\[(.*)%\\].*/\\1/'", 0, 10},
	{"bat:", "sed \"s/$/%/\" /sys/class/power_supply/BAT?/capacity", 5, 0},
	{"", "date '+%b %d (%a) %I:%M%p'",	60,	0},
	/* weather: 5 */
	/* news: 6 */
	/* torrent: 7 */
	/* packages: 8 */
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char delim = '|';
