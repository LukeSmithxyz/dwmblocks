//Modify this file to change what commands output to your statusbar, and recompile using the make command.
static const Block blocks[] = {
    /*Icon*/ /*Command*/   /*Update Interval*/ /*Update Signal*/
    {"",      "music",          0,              11},
    {"",      "moonphase",  18000,               4},
    {"",      "weather",    18000,               5},
    {"",      "volume",         0,              10},
    {"",      "nettraf",       10,              16},
    {"",      "clock",         10,               6},
    {"",      "killwin",        0,              15},
};

//sets delimeter between status commands. NULL character ('\0') means no delimeter.
static char *delim = "|";
