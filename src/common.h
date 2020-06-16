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

#define bg_black   "\033[40m"
#define bg_red     "\033[41m"
#define bg_green   "\033[42m"
#define bg_yellow  "\033[43m"
#define bg_blue    "\033[44m"
#define bg_magenta "\033[45m"
#define bg_cyan    "\033[46m"
#define bg_white   "\033[47m"

#define fg_black   "\033[30m"
#define fg_red     "\033[31m"
#define fg_green   "\033[32m"
#define fg_yellow  "\033[33m"
#define fg_blue    "\033[34m"
#define fg_magenta "\033[35m"
#define fg_cyan    "\033[36m"
#define fg_white   "\033[37m"

#define color_reset "\033[0m"

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

char* ErrorLevel_CString[3] = { bg_red "ERROR", bg_yellow "WARN ", bg_blue "NOTE " };
void printError(Error err) {
	char buf[10000];
	char* p = buf;

	if(err.hideInfo) {
		p = p + sprintf(p, fg_black "%s" color_reset,
			ErrorLevel_CString[err.lvl]
		);
	} else {
		p = p + sprintf(p, fg_black "%s" color_reset "[%ju:%ju][%ju:%ju]",
			ErrorLevel_CString[err.lvl],
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
