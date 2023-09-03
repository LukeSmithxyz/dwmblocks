// Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/* Icon */	/* Command */				/* Update Interval */	/* Update Signal */
	{"⌨",		"sb-kbselect",				0,			30},
	{"",		"cat /tmp/recordingicon 2>/dev/null",	0,			9},
	{"",		"sb-tasks",				10,			26},
	{"",		"sb-music",				,			11},
	{"",		"sb-pacpackages",			,			8},
	{"",		"sb-news",				,			6},
	{"",		"sb-price xmr Monero 🔒 24",		9000,			24},
	{"",		"sb-price eth Ethereum 🍸 23",		9000,			23},
	{"",		"sb-price btc Bitcoin 💰 21",		9000,			21},
	{"",		"sb-torrent",				20,			7},
	{"",		"sb-memory",				10,			14},
	{"",		"sb-cpu",				10,			18},
	{"",		"sb-moonphase",				18000,			17},
	{"",		"sb-doppler",				0,			13},
	{"",		"sb-forecast",				18000,			5},
	{"",		"sb-mailbox",				180,			12},
	{"",		"sb-nettraf",				1,			16},
	{"",		"sb-volume",				0,			10},
	{"",		"sb-battery",				5,			3},
	{"",		"sb-clock",				60,			1},
	{"",		"sb-internet",				5,			4},
	{"",		"sb-iplocate",				0,			27},
	{"",		"sb-help-icon",				0,			15},
};

// Sets delimiter between status commands.
// Use NULL character ('\0') to remove delimiter.
static char *delim = " ";
