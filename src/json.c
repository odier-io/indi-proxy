/*--------------------------------------------------------------------------------------------------------------------*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

typedef enum json_token_type_e
{
    JSON_TOKEN_EOF,
    JSON_TOKEN_NULL,
    JSON_TOKEN_TRUE,
    JSON_TOKEN_FALSE,
    JSON_TOKEN_NUMBER,
    JSON_TOKEN_STRING,
    JSON_TOKEN_CURLY_OPEN,
    JSON_TOKEN_CURLY_CLOSE,
    JSON_TOKEN_SQUARE_OPEN,
    JSON_TOKEN_SQUARE_CLOSE,
    JSON_TOKEN_COLON,
    JSON_TOKEN_COMMA,
    JSON_TOKEN_ERROR,

} json_token_type_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct json_token_s
{
    str_t val;

    json_token_type_t token_type;

} json_token_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct json_parser_s
{
    STR_t pos;

    json_token_t curr_token;

} json_parser_t;

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#define NEXT() \
            tokenizer_next(parser)

#define PEEK() \
            (parser->curr_token)

#define CHECK(t) \
            (parser->curr_token.token_type == (t))

/*--------------------------------------------------------------------------------------------------------------------*/

static void tokenizer_next(json_parser_t *parser)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(; isspace(*parser->pos); parser->pos++);

    /*----------------------------------------------------------------------------------------------------------------*/

    const char *start = parser->pos;
    const char *end = parser->pos;

    json_token_type_t type;

    switch(*parser->pos)
    {
        case '\0':
            type = JSON_TOKEN_EOF;
            break;

        case '{':
            type = JSON_TOKEN_CURLY_OPEN;
            end++;
            break;

        case '}':
            type = JSON_TOKEN_CURLY_CLOSE;
            end++;
            break;

        case '[':
            type = JSON_TOKEN_SQUARE_OPEN;
            end++;
            break;

        case ']':
            type = JSON_TOKEN_SQUARE_CLOSE;
            end++;
            break;

        case ':':
            type = JSON_TOKEN_COLON;
            end++;
            break;

        case ',':
            type = JSON_TOKEN_COMMA;
            end++;
            break;

        case '"':
            type = JSON_TOKEN_STRING;

            end++;
            for(; *end != '\"'; end++)
            {
                if(*end == '\0')
                {
                    type = JSON_TOKEN_ERROR;
                    goto _err;
                }
            }
            end++;
            break;

        default:
            if(*end == '-' || *end == '+' || isdigit(*end))
            {
                type = JSON_TOKEN_NUMBER;

                for(; *end == '-' || *end == '+' || *end == '.' || *end == 'e' || *end == 'E' || isdigit(*end); end++)
                {
                    if(*end == '\0')
                    {
                        type = JSON_TOKEN_ERROR;
                        goto _err;
                    }
                }
            }
            else if(strncmp(end, "null", 4) == 0)
            {
                type = JSON_TOKEN_NULL;
                end += 4;
            }
            else if(strncmp(end, "true", 4) == 0)
            {
                type = JSON_TOKEN_TRUE;
                end += 4;
            }
            else if(strncmp(end, "false", 5) == 0)
            {
                type = JSON_TOKEN_FALSE;
                end += 5;
            }
            else
            {
                type = JSON_TOKEN_ERROR;
            }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    /**/ if(type == JSON_TOKEN_NUMBER)
    {
        size_t length = (size_t) end - (size_t) start - 0;

        parser->curr_token.val = strncpy(indi_alloc(length + 1), start + 0, length);

        parser->curr_token.val[length] = '\0';
    }
    else if(type == JSON_TOKEN_STRING)
    {
        size_t length = (size_t) end - (size_t) start - 2;

        parser->curr_token.val = strncpy(indi_alloc(length + 1), start + 1, length);

        parser->curr_token.val[length] = '\0';
    }

    /*----------------------------------------------------------------------------------------------------------------*/

_err:
    parser->curr_token.token_type = type;

    parser->pos = end;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static indi_dict_t *json_parse_dict(json_parser_t *parser);

static indi_list_t *json_parse_list(json_parser_t *parser);

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static indi_null_t *json_parse_null(json_parser_t *parser)
{
    if(CHECK(JSON_TOKEN_NULL) == false)
    {
        return NULL;
    }

    NEXT();

    return indi_null_new();
}

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_boolean_t *json_parse_true(json_parser_t *parser)
{
    if(CHECK(JSON_TOKEN_TRUE) == false)
    {
        return NULL;
    }

    NEXT();

    return indi_boolean_from(true);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_boolean_t *json_parse_false(json_parser_t *parser)
{
    if(CHECK(JSON_TOKEN_FALSE) == false)
    {
        return NULL;
    }

    NEXT();

    return indi_boolean_from(false);
}

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_number_t *json_parse_number(json_parser_t *parser)
{
    if(CHECK(JSON_TOKEN_NUMBER == false))
    {
        return NULL;
    }

    str_t val = PEEK().val;

    indi_number_t *result = indi_number_from(atof(val));

    indi_free(val);

    NEXT();

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_string_t *json_parse_string(json_parser_t *parser)
{
    if(CHECK(JSON_TOKEN_STRING) == false)
    {
        return NULL;
    }

    str_t val = PEEK().val;

    indi_string_t *result = indi_string_from(/**/(val));

    indi_free(val);

    NEXT();

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_dict_t *json_parse_dict(json_parser_t *parser) // NOLINT(misc-no-recursion)
{
    indi_dict_t *result = indi_dict_new();

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(JSON_TOKEN_CURLY_OPEN) == false)
    {
        goto _err;
    }

    NEXT();

    /*----------------------------------------------------------------------------------------------------------------*/

    while(CHECK(JSON_TOKEN_CURLY_CLOSE) == false)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        if(CHECK(JSON_TOKEN_COLON) == false)
        {
            ////_free(key);

            goto _err;
        }

        str_t key = PEEK().val;

        NEXT();

        if(CHECK(JSON_TOKEN_COLON) == false)
        {
            indi_free(key);

            goto _err;
        }

        NEXT();

        /*------------------------------------------------------------------------------------------------------------*/

        /**/ if(CHECK(JSON_TOKEN_NULL)) {
            indi_dict_put(result, key, json_parse_null(parser));
        }
        else if(CHECK(JSON_TOKEN_TRUE)) {
            indi_dict_put(result, key, json_parse_true(parser));
        }
        else if(CHECK(JSON_TOKEN_FALSE)) {
            indi_dict_put(result, key, json_parse_false(parser));
        }
        else if(CHECK(JSON_TOKEN_NUMBER)) {
            indi_dict_put(result, key, json_parse_number(parser));
        }
        else if(CHECK(JSON_TOKEN_STRING)) {
            indi_dict_put(result, key, json_parse_string(parser));
        }
        else if(CHECK(JSON_TOKEN_CURLY_OPEN)) {
            indi_dict_put(result, key, json_parse_dict(parser));
        }
        else if(CHECK(JSON_TOKEN_SQUARE_OPEN)) {
            indi_dict_put(result, key, json_parse_list(parser));
        }
        else
        {
            indi_free(key);

            goto _err;
        }

        /*------------------------------------------------------------------------------------------------------------*/

        indi_free(key);

        /*------------------------------------------------------------------------------------------------------------*/

        if(CHECK(JSON_TOKEN_COMMA) == true)
        {
            NEXT();

            if(CHECK(JSON_TOKEN_CURLY_CLOSE) == true)
            {
                goto _err;
            }
        }
        else
        {
            if(CHECK(JSON_TOKEN_CURLY_CLOSE) == false)
            {
                goto _err;
            }
        }

        /*------------------------------------------------------------------------------------------------------------*/

    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(JSON_TOKEN_CURLY_CLOSE) == false)
    {
        goto _err;
    }

    NEXT();

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;

_err:
    indi_dict_free(result);

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_list_t *json_parse_list(json_parser_t *parser) // NOLINT(misc-no-recursion)
{
    indi_list_t *result = indi_list_new();

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(JSON_TOKEN_SQUARE_OPEN) == false)
    {
        goto _err;
    }

    NEXT();

    /*----------------------------------------------------------------------------------------------------------------*/

    while(CHECK(JSON_TOKEN_SQUARE_CLOSE) == false)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        /**/ if(CHECK(JSON_TOKEN_NULL)) {
            indi_list_push(result, json_parse_null(parser));
        }
        else if(CHECK(JSON_TOKEN_TRUE)) {
            indi_list_push(result, json_parse_true(parser));
        }
        else if(CHECK(JSON_TOKEN_FALSE)) {
            indi_list_push(result, json_parse_false(parser));
        }
        else if(CHECK(JSON_TOKEN_NUMBER)) {
            indi_list_push(result, json_parse_number(parser));
        }
        else if(CHECK(JSON_TOKEN_STRING)) {
            indi_list_push(result, json_parse_string(parser));
        }
        else if(CHECK(JSON_TOKEN_CURLY_OPEN)) {
            indi_list_push(result, json_parse_dict(parser));
        }
        else if(CHECK(JSON_TOKEN_SQUARE_OPEN)) {
            indi_list_push(result, json_parse_list(parser));
        }
        else
        {
            goto _err;
        }

        /*------------------------------------------------------------------------------------------------------------*/

        if(CHECK(JSON_TOKEN_COMMA) == true)
        {
            NEXT();

            if(CHECK(JSON_TOKEN_SQUARE_CLOSE) == true)
            {
                goto _err;
            }
        }
        else
        {
            if(CHECK(JSON_TOKEN_SQUARE_CLOSE) == false)
            {
                goto _err;
            }
        }

        /*------------------------------------------------------------------------------------------------------------*/
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(JSON_TOKEN_SQUARE_CLOSE) == false)
    {
        goto _err;
    }

    NEXT();

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;

_err:
    indi_list_free(result);

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_object_t *indi_json_parse(STR_t json)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    json_parser_t _parser = {};

    json_parser_t *parser = &_parser;

    /*----------------------------------------------------------------------------------------------------------------*/

    parser->pos = json;

    parser->curr_token.val = NULL;

    parser->curr_token.token_type = JSON_TOKEN_ERROR;

    /*----------------------------------------------------------------------------------------------------------------*/

    tokenizer_next(parser);

    /*----------------------------------------------------------------------------------------------------------------*/

    /**/ if(CHECK(JSON_TOKEN_NULL)) {
        return (indi_object_t *) json_parse_null(parser);
    }
    else if(CHECK(JSON_TOKEN_TRUE)) {
        return (indi_object_t *) json_parse_true(parser);
    }
    else if(CHECK(JSON_TOKEN_FALSE)) {
        return (indi_object_t *) json_parse_false(parser);
    }
    else if(CHECK(JSON_TOKEN_NUMBER)) {
        return (indi_object_t *) json_parse_number(parser);
    }
    else if(CHECK(JSON_TOKEN_STRING)) {
        return (indi_object_t *) json_parse_string(parser);
    }
    else if(CHECK(JSON_TOKEN_CURLY_OPEN)) {
        return (indi_object_t *) json_parse_dict(parser);
    }
    else if(CHECK(JSON_TOKEN_SQUARE_OPEN)) {
        return (indi_object_t *) json_parse_list(parser);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/
