/*--------------------------------------------------------------------------------------------------------------------*/

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

#define NEXT() \
            tokenizer_next(perser)

#define PEEK() \
            (perser->curr_token)

#define CHECK(t) \
            (perser->curr_token.token_type == (t))

/*--------------------------------------------------------------------------------------------------------------------*/

static void tokenizer_next(indi_json_parser_t *perser)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(; isspace(*perser->pos); perser->pos++);

    /*----------------------------------------------------------------------------------------------------------------*/

    const char *start = perser->pos;
    const char *end = perser->pos;

    indi_json_token_type_t type;

    switch(*perser->pos)
    {
        case '\0':
            type = TOKEN_EOF;
            break;

        case '{':
            type = TOKEN_CURLY_OPEN;
            end++;
            break;

        case '}':
            type = TOKEN_CURLY_CLOSE;
            end++;
            break;

        case '[':
            type = TOKEN_SQUARE_OPEN;
            end++;
            break;

        case ']':
            type = TOKEN_SQUARE_CLOSE;
            end++;
            break;

        case ':':
            type = TOKEN_COLON;
            end++;
            break;

        case ',':
            type = TOKEN_COMMA;
            end++;
            break;

        case '"':
            type = TOKEN_STRING;

            end++;
            for(; *end != '\"'; end++)
            {
                if(*end == '\0')
                {
                    type = TOKEN_ERROR;
                    goto _err;
                }
            }
            end++;
            break;

        default:
            if(*end == '-' || *end == '+' || isdigit(*end))
            {
                type = TOKEN_NUMBER;

                for(; *end == '-' || *end == '+' || *end == '.' || *end == 'e' || *end == 'E' || isdigit(*end); end++)
                {
                    if(*end == '\0')
                    {
                        type = TOKEN_ERROR;
                        goto _err;
                    }
                }
            }
            else if(strncmp(end, "null", 4) == 0)
            {
                type = TOKEN_NULL;
                end += 4;
            }
            else if(strncmp(end, "true", 4) == 0)
            {
                type = TOKEN_TRUE;
                end += 4;
            }
            else if(strncmp(end, "false", 5) == 0)
            {
                type = TOKEN_FALSE;
                end += 5;
            }
            else
            {
                type = TOKEN_ERROR;
            }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    /**/ if(type == TOKEN_NUMBER)
    {
        size_t length = (size_t) end - (size_t) start - 0;

        perser->curr_token.val = strncpy(indi_alloc(length + 1), start + 0, length);

        perser->curr_token.val[length] = '\0';
    }
    else if(type == TOKEN_STRING)
    {
        size_t length = (size_t) end - (size_t) start - 2;

        perser->curr_token.val = strncpy(indi_alloc(length + 1), start + 1, length);

        perser->curr_token.val[length] = '\0';
    }

    /*----------------------------------------------------------------------------------------------------------------*/

_err:
    perser->curr_token.token_type = type;

    perser->pos = end;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void indi_json_parser_init(struct indi_json_parser_s *parser, STR_t json)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    parser->pos = json;

    parser->curr_token.val = NULL;

    parser->curr_token.token_type = TOKEN_ERROR;

    /*----------------------------------------------------------------------------------------------------------------*/

    tokenizer_next(parser);

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

indi_null_t *indi_json_parse_null(indi_json_parser_t *perser)
{
    if(CHECK(TOKEN_NULL) == false)
    {
        return NULL;
    }

    NEXT();

    return indi_null_new();
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_boolean_t *indi_json_parse_true(indi_json_parser_t *perser)
{
    if(CHECK(TOKEN_TRUE) == false)
    {
        return NULL;
    }

    NEXT();

    return indi_boolean_from(true);
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_boolean_t *indi_json_parse_false(indi_json_parser_t *perser)
{
    if(CHECK(TOKEN_FALSE) == false)
    {
        return NULL;
    }

    NEXT();

    return indi_boolean_from(false);
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_number_t *indi_json_parse_number(indi_json_parser_t *perser)
{
    if(CHECK(TOKEN_NUMBER == false))
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

indi_string_t *indi_json_parse_string(indi_json_parser_t *perser)
{
    if(CHECK(TOKEN_STRING) == false)
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

indi_dict_t *indi_json_parse_dict(indi_json_parser_t *perser)
{
    indi_dict_t *result = indi_dict_new();

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(TOKEN_CURLY_OPEN) == false)
    {
        goto _err;
    }

    NEXT();

    /*----------------------------------------------------------------------------------------------------------------*/

    while(CHECK(TOKEN_CURLY_CLOSE) == false)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        if(CHECK(TOKEN_COLON) == false)
        {
            ////_free(key);

            goto _err;
        }

        str_t key = PEEK().val;

        NEXT();

        if(CHECK(TOKEN_COLON) == false)
        {
            indi_free(key);

            goto _err;
        }

        NEXT();

        /*------------------------------------------------------------------------------------------------------------*/

        /**/ if(CHECK(TOKEN_NULL)) {
            indi_dict_put(result, key, indi_json_parse_null(perser));
        }
        else if(CHECK(TOKEN_TRUE)) {
            indi_dict_put(result, key, indi_json_parse_true(perser));
        }
        else if(CHECK(TOKEN_FALSE)) {
            indi_dict_put(result, key, indi_json_parse_false(perser));
        }
        else if(CHECK(TOKEN_NUMBER)) {
            indi_dict_put(result, key, indi_json_parse_number(perser));
        }
        else if(CHECK(TOKEN_STRING)) {
            indi_dict_put(result, key, indi_json_parse_string(perser));
        }
        else if(CHECK(TOKEN_CURLY_OPEN)) {
            indi_dict_put(result, key, indi_json_parse_dict(perser));
        }
        else if(CHECK(TOKEN_SQUARE_OPEN)) {
            indi_dict_put(result, key, indi_json_parse_list(perser));
        }
        else
        {
            indi_free(key);

            goto _err;
        }

        /*------------------------------------------------------------------------------------------------------------*/

        indi_free(key);

        /*------------------------------------------------------------------------------------------------------------*/

        if(CHECK(TOKEN_COMMA) == true)
        {
            NEXT();

            if(CHECK(TOKEN_CURLY_CLOSE) == true)
            {
                goto _err;
            }
        }
        else
        {
            if(CHECK(TOKEN_CURLY_CLOSE) == false)
            {
                goto _err;
            }
        }

        /*------------------------------------------------------------------------------------------------------------*/

    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(TOKEN_CURLY_CLOSE) == false)
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

indi_list_t *indi_json_parse_list(indi_json_parser_t *perser)
{
    indi_list_t *result = indi_list_new();

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(TOKEN_SQUARE_OPEN) == false)
    {
        goto _err;
    }

    NEXT();

    /*----------------------------------------------------------------------------------------------------------------*/

    while(CHECK(TOKEN_SQUARE_CLOSE) == false)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        /**/ if(CHECK(TOKEN_NULL)) {
            indi_list_push(result, indi_json_parse_null(perser));
        }
        else if(CHECK(TOKEN_TRUE)) {
            indi_list_push(result, indi_json_parse_true(perser));
        }
        else if(CHECK(TOKEN_FALSE)) {
            indi_list_push(result, indi_json_parse_false(perser));
        }
        else if(CHECK(TOKEN_NUMBER)) {
            indi_list_push(result, indi_json_parse_number(perser));
        }
        else if(CHECK(TOKEN_STRING)) {
            indi_list_push(result, indi_json_parse_string(perser));
        }
        else if(CHECK(TOKEN_CURLY_OPEN)) {
            indi_list_push(result, indi_json_parse_dict(perser));
        }
        else if(CHECK(TOKEN_SQUARE_OPEN)) {
            indi_list_push(result, indi_json_parse_list(perser));
        }
        else
        {
            goto _err;
        }

        /*------------------------------------------------------------------------------------------------------------*/

        if(CHECK(TOKEN_COMMA) == true)
        {
            NEXT();

            if(CHECK(TOKEN_SQUARE_CLOSE) == true)
            {
                goto _err;
            }
        }
        else
        {
            if(CHECK(TOKEN_SQUARE_CLOSE) == false)
            {
                goto _err;
            }
        }

        /*------------------------------------------------------------------------------------------------------------*/
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(CHECK(TOKEN_SQUARE_CLOSE) == false)
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
