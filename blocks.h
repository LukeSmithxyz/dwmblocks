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

	/* {"🔊 ", "x=\"$(amixer get Master)\"; echo \"$x\" | grep -o \"\\[off\\]\" || echo \"$x\" | grep -o [0-9]*%", 0, 10}, */
	/* {"", "sed /📦0/d ~/.pacupdate",					0,		9}, */

	/* {"🧠 ", "free -h | awk '/^Mem/ { print $3\"/\"$2 }' | sed s/i//g",	30,		0}, */

	/* {"💻 ", "sed 's/000$/°C/' /sys/class/thermal/thermal_zone0/temp", 30, 21}, */

	//{"🎼 ", "mpc status | sed \"/^volume:/d;/\\[playing\\]/d;s/^\\[paused\\].*/⏸/\"", 0, 11},

	//{"☀ ", "xbacklight | sed 's/\\..*//'",					0,		11},

	/* {"🌡 ", "sensors | awk '/^temp1:/{print $2}'",				5,		0}, */

