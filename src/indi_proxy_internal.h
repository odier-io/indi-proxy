/*--------------------------------------------------------------------------------------------------------------------*/

#ifndef INDI_PROXY_INDI_PROXY_INTERNAL_H
#define INDI_PROXY_INDI_PROXY_INTERNAL_H

/*--------------------------------------------------------------------------------------------------------------------*/

#include "indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/* ALLOC                                                                                                              */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef enum indi_type_e
{
    INDI_TYPE_NULL      = 100,
    INDI_TYPE_BOOLEAN   = 101,
    INDI_TYPE_NUMBER    = 102,
    INDI_TYPE_STRING    = 103,
    INDI_TYPE_DICT      = 104,
    INDI_TYPE_LIST      = 105,

} indi_type_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_object_s
{
    uint32_t magic;

    enum indi_type_e type;

} indi_object_t;

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t indi_alloc(
    size_t size
);

void indi_free(
    buff_t buff
);

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_strdup(
    STR_t s
);

str_t indi_dbldup(
    double d
);

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t indi_object_new(
    enum indi_type_e type
);

void indi_object_free(
    buff_t object
);

str_t indi_object_to_string(
    buff_t object
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* NULL                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_null_s
{
    struct indi_object_s object;

} indi_null_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_null_s *indi_null_new();

void indi_null_free(
    struct indi_null_s *o
);

str_t indi_null_to_string(
    struct indi_null_s *o
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* NUMBER                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_number_s
{
    struct indi_object_s object;

    double data;

} indi_number_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_number_s *indi_number_new();

void indi_number_free(
    struct indi_number_s *o
);

double indi_number_get(
    struct indi_number_s *o
);

void indi_number_set(
    struct indi_number_s *o,
    double data
);

str_t indi_number_to_string(
    struct indi_number_s *o
);

/*--------------------------------------------------------------------------------------------------------------------*/

static inline struct indi_number_s *indi_number_from(double data)
{
    struct indi_number_s *result = indi_number_new();

    indi_number_set(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* BOOLEAN                                                                                                            */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_boolean_s
{
    struct indi_object_s object;

    bool data;

} indi_boolean_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_boolean_s *indi_boolean_new();

void indi_boolean_free(
    struct indi_boolean_s *o
);

bool indi_boolean_get(
    struct indi_boolean_s *o
);

void indi_boolean_set(
    struct indi_boolean_s *o,
    bool data
);

str_t indi_boolean_to_string(
    struct indi_boolean_s *o
);

/*--------------------------------------------------------------------------------------------------------------------*/

static inline struct indi_boolean_s *indi_boolean_from(bool data)
{
    struct indi_boolean_s *result = indi_boolean_new();

    indi_boolean_set(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* STRING                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_string_s
{
    struct indi_object_s object;

    struct indi_string_node_s *head;
    struct indi_string_node_s *tail;

} indi_string_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_string_s *indi_string_new();

void indi_string_free(
    struct indi_string_s *o
);

struct indi_string_s *indi_string_append(
    struct indi_string_s *o,
    STR_t data
);

size_t indi_string_length(
    struct indi_string_s *o
);

str_t indi_string_to_string(
    struct indi_string_s *o
);

str_t indi_string_to_cstring(
    struct indi_string_s *o
);

/*--------------------------------------------------------------------------------------------------------------------*/

static inline struct indi_string_s *indi_string_from(STR_t data)
{
    struct indi_string_s *result = indi_string_new();

    indi_string_append(result, data);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/* LIST                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_list_s
{
    struct indi_object_s object;

    struct indi_list_node_s *head;
    struct indi_list_node_s *tail;

} indi_list_t;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_list_s *indi_list_new();

void indi_list_free(
    struct indi_list_s *o
);

void indi_list_del(
    struct indi_list_s *o,
    int idx
);

struct indi_object_s *indi_list_get(
    struct indi_list_s *o,
    int idx
);

struct indi_list_s *indi_list_push(
    struct indi_list_s *o,
    buff_t val
);

str_t indi_list_to_string(
    struct indi_list_s *o
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* DICT                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_dict_s
{
    struct indi_object_s object;

    struct indi_dict_node_s *head;
    struct indi_dict_node_s *tail;

} indi_dict_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_dict_t *indi_dict_new();

void indi_dict_free(
    struct indi_dict_s *o
);

void indi_dict_del(
    struct indi_dict_s *o,
    STR_t key
);

struct indi_object_s *indi_dict_get(
    struct indi_dict_s *o,
    STR_t key
);

void indi_dict_put(
    struct indi_dict_s *o,
    STR_t key,
    buff_t val
);

str_t indi_dict_to_string(
    struct indi_dict_s *o
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* JSON                                                                                                               */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef enum indi_json_token_type_e
{
    TOKEN_ERROR,
    TOKEN_CURLY_OPEN,
    TOKEN_CURLY_CLOSE,
    TOKEN_SQUARE_OPEN,
    TOKEN_SQUARE_CLOSE,
    TOKEN_COLON,
    TOKEN_COMMA,
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_EOF

} indi_json_token_type_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_json_token_s
{
    str_t val;

    indi_json_token_type_t token_type;

} indi_json_token_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_json_parser_s
{
    STR_t pos;

    indi_json_token_t curr_token;

} indi_json_parser_t;

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_json_parser_init(
    struct indi_json_parser_s *parser,
    STR_t json
);

/*--------------------------------------------------------------------------------------------------------------------*/

indi_null_t *indi_json_parse_null(
    struct indi_json_parser_s *parser
);

indi_boolean_t *indi_json_parse_true(
    struct indi_json_parser_s *parser
);

indi_boolean_t *indi_json_parse_false(
    struct indi_json_parser_s *parser
);

indi_number_t *indi_json_parse_number(
    struct indi_json_parser_s *parser
);

indi_string_t *indi_json_parse_string(
    struct indi_json_parser_s *parser
);

indi_dict_t *indi_json_parse_dict(
    struct indi_json_parser_s *parser
);

indi_list_t *indi_json_parse_list(
    struct indi_json_parser_s *parser
);

/*--------------------------------------------------------------------------------------------------------------------*/

#endif // INDI_PROXY_INDI_PROXY_INTERNAL_H

/*--------------------------------------------------------------------------------------------------------------------*/
