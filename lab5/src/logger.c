#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// colour codes
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

// coloured tags
#define DEBUG YEL "DEBUG" RESET
#define ERROR RED "ERROR" RESET
#define INFO  CYN "INFO"  RESET

/**
 * @brief Get the verbosity from LOG_LEVEL environment variable
 * 
 * @return int 0 for info, 1 debug, 
 */
int get_verbosity() {
    const char* verbosity = getenv("LOG_LEVEL");

    // default info logging
    if (verbosity == NULL) {
        return 0;
    }
    
    if (strcmp(verbosity, "debug") == 0) {
        return 1;
    }

    // fallback to info
    return 0;
}

/**
 * @brief Logs messages based on given format
 * 
 * @param tag     log message type (error, info, debug)
 * @param message content to show
 * @param args    formatting arguments
 */
void log_format(const char* tag, const char* message, va_list args) {
    int verbosity = get_verbosity();

    // skip if verbosity set to info and currently trying to log_debug
    if (verbosity == 0 && strcmp(tag, DEBUG) == 0) {
        return;
    }

    // get current time
    char buff[16];
    time_t now = time(0);
    strftime(buff, 40, "%H:%M:%S", localtime(&now));

    pthread_t id = pthread_self();

    // print to stderr if on error duh
    FILE* target = stdout;
    if (strcmp(tag, ERROR)) {
        target = stderr;
    }
    
    // [thread id] [time] [type] message
    fprintf(target, "[TID %zx] [%s] [%s] ",
            id, buff, tag);
    vprintf(message, args);
    printf("\n");
}

/**
 * @brief Logs an error message
 * 
 * @param message error message
 * @param ...     formatting arguments
 */
void log_error(const char* message, ...) {
    va_list args;
    va_start(args, message);
    log_format(ERROR, message, args);
    va_end(args);
}

/**
 * @brief Logs an info message
 * 
 * @param message info message
 * @param ...     formatting arguments
 */
void log_info(const char* message, ...) {
    va_list args;
    va_start(args, message);
    log_format(INFO, message, args);
    va_end(args);
}

/**
 * @brief Logs a debug message
 * 
 * @param message debug message
 * @param ...     formatting arguments
 */
void log_debug(const char* message, ...) {
    va_list args;
    va_start(args, message);
    log_format(DEBUG, message, args);
    va_end(args);
}
