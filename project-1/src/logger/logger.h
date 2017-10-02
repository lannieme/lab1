//
// Created by Huai-Che Lu on 2/25/17.
//

#ifndef CS5450_LAB1_LOGGER_H
#define CS5450_LAB1_LOGGER_H

#include <stdio.h>

typedef struct logger_t logger_t;

// Life cycle
logger_t *create_logger();
void terminate_logger(logger_t *logger);

// Log file
void bind_log_file(logger_t *logger, char log_fp[]);
void close_log(logger_t *logger);

// Logging methods
void log_info(logger_t *logger, unsigned long addr, char access_req[],
              int status_code, unsigned long bytes);
void log_warn(logger_t *logger, char msg[]);
void log_error(logger_t *logger, char msg[]);

#endif //CS5450_LAB1_LOGGER_H
