//
// Created by Huai-Che Lu on 2/25/17.
//

#include "logger.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define BUF_SIZE 1000

#define TAG_ERROR "Error"
#define TAG_WARN "Warn"
#define TAG_INFO "Info"

struct logger_t {
    FILE *fp;
};

static bool has_log_file(logger_t *logger);
static void log_msg(logger_t *logger, unsigned long addr, char msg[]);

logger_t *create_logger(char log_fp[]) {
    logger_t *logger;

    logger = (logger_t *) malloc(sizeof(logger_t));

    return logger;
}

void bind_log_file(logger_t *logger, char file_path[]) {
    logger->fp = fopen(file_path, "a");
}

void close_log_file(logger_t *logger) {
    if (has_log_file(logger)) {
        fclose(logger->fp);
        logger->fp = NULL;
    }
}

void terminate_logger(logger_t *logger) {
    if (logger == NULL) {
        return;
    }
    close_log_file(logger);
    free(logger);

}

void log_info(logger_t *logger, unsigned long addr, char access_req[],
              int status, unsigned long bytes) {
    char msg[BUF_SIZE];
    sprintf(msg, "%s %d %lu", access_req, status, bytes);
    log_msg(logger, addr, msg);
}

void log_warn(logger_t *logger, char msg[]) {
//    log_msg(logger, TAG_WARN, msg);
}

void log_error(logger_t *logger, char msg[]) {
//    log_msg(logger, TAG_ERROR, msg);
}

static bool has_log_file(logger_t *logger) {
    return logger->fp;
}

static void log_msg(logger_t *logger, unsigned long addr, char msg[]) {
    char ts[BUF_SIZE];
    time_t t;
    struct tm *tp;
    char ip_str[16];

    sprintf(ip_str, "%d.%d.%d.%d",
            (int) (addr&0xFF),
            (int) ((addr&0xFF00) >> 8),
            (int) ((addr&0xFF0000) >> 16),
            (int) ((addr&0xFF000000) >> 24)
    );

    t = time(NULL);
    tp = localtime(&t);
    strftime(ts, BUF_SIZE, "%b %d, %Y %H:%M:%S %z", tp);

    if (has_log_file(logger)) {
        fprintf(logger->fp, "%s [%s] %s\n", ip_str, ts, msg);
        fflush(logger->fp);
    }
}