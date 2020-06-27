#ifndef DASH_COMMON_HEADER
#define DASH_COMMON_HEADER

#include "defs.h"

typedef struct {
	usize line, column;
} LineColumn;

typedef struct {
	LineColumn begin, end;
} Location;

#include <stdio.h>

#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

#define FG_BLACK   "\033[30m"
#define FG_RED     "\033[31m"
#define FG_GREEN   "\033[32m"
#define FG_YELLOW  "\033[33m"
#define FG_BLUE    "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN    "\033[36m"
#define FG_WHITE   "\033[37m"

#define COLOR_RESET "\033[0m"

typedef u8 ErrorLevel;
enum {
	ErrorLevelError,
	ErrorLevelWarn,
	ErrorLevelNote,
};

typedef struct {
	ErrorLevel lvl;
	string     msg;
	bool       hideInfo;
	Location   loc;
	string     file;
} Error;

char* ErrorLevel_cstring[3] = { BG_RED "ERROR", BG_YELLOW "WARN ", BG_BLUE "NOTE " };
void print_error(Error err) {
	char buf[10000];
	char* p = buf;

	if(err.hideInfo) {
		p = p + sprintf(p, FG_BLACK "%s" COLOR_RESET,
			ErrorLevel_cstring[err.lvl]
		);
	} else {
		p = p + sprintf(p, FG_BLACK "%s" COLOR_RESET "[%ju:%ju][%ju:%ju]",
			ErrorLevel_cstring[err.lvl],
			err.loc.begin.line,
			err.loc.begin.column,
			err.loc.end.line,
			err.loc.end.column
		);
		p = p + snprintf(p, err.file.len + 1, "%s",   err.file.ptr);
	}
	
	p = p + snprintf(p, err.msg.len  + 3, ": %s", err.msg.ptr);
	printf("%s\n", buf);
}

#endif
