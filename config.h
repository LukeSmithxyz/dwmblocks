//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
	/*Icon*/	/*Command*/								/*Update Interval*/	/*Update Signal*/
	{"",	"/home/akio/.config/dwmblocks/spotify.sh",			10,				7},
	{"",	"/home/akio/.config/dwmblocks/network.sh",			10,				1},
	{"",	"/home/akio/.config/dwmblocks/power.sh",			60,				2},
	{"",	"/home/akio/.config/dwmblocks/backlight.sh",		10,				3},
	{"",	"/home/akio/.config/dwmblocks/volume.sh",			5,				4},
	{"",	"/home/akio/.config/dwmblocks/date.sh",				1200,			5},
	{"",	"/home/akio/.config/dwmblocks/time.sh",				60,				6},
	// {"",	"/home/akio/.config/dwmblocks/weather.sh",			3600,			2},
};

//Sets delimiter between status commands. NULL character ('\0') means no delimiter.
static char *delim = " ";

// Have dwmblocks automatically recompile and run when you edit this file in
// vim with the following line in your vimrc/init.vim:

// autocmd BufWritePost ~/.local/src/dwmblocks/config.h !cd ~/.local/src/dwmblocks/; sudo make install && { killall -q dwmblocks;setsid dwmblocks & }
