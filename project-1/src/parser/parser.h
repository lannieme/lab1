#ifndef CS5450_LAB1_PARSER_H
#define CS5450_LAB1_PARSER_H

#define SUCCESS 0

typedef struct Request Request;

Request *parse_req_str(char *req_str);

#endif //CS5450_LAB1_PARSER_H

