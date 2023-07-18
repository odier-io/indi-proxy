/*--------------------------------------------------------------------------------------------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

static size_t indi_mem = 0;

/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_block_s
{
    size_t size;

    uint32_t magic;
};

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t indi_alloc(size_t size)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    struct indi_block_s *block = malloc(sizeof(struct indi_block_s) + size);

    if(block == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        fflush(stderr);
        exit(1);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_mem += size;

    block->size = size;
    block->magic = 0x75757575;

    return (struct indi_block_s *) (((str_t) block) + sizeof(struct indi_block_s));

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_free(buff_t buff)
{
    if(buff != NULL)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        struct indi_block_s *block = (struct indi_block_s *) (((str_t) buff) - sizeof(struct indi_block_s));

        if(block->magic != 0x75757575)
        {
            fprintf(stderr, "Memory corruption!\n");
            fflush(stderr);
            exit(1);
        }

        /*------------------------------------------------------------------------------------------------------------*/

        indi_mem -= block->size;

        free(block);

        /*------------------------------------------------------------------------------------------------------------*/
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_memory_report()
{
    if(indi_mem > 0)
    {
        fprintf(stderr, "Memory leak: %ld bytes!\n", indi_mem);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_strdup(STR_t s)
{
    if(s != NULL)
    {
        str_t str = indi_alloc(strlen(s));

        strcpy(str, s);

        return str;
    }

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_dbldup(double d)
{
    if(!isnan(d))
    {
        str_t str = indi_alloc(32 + 1);

        sprintf(str, "%lf", d);

        return str;
    }

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void indi_object_free(buff_t buff)
{
    indi_object_t *o = buff;

    if(o == NULL || o->magic != 0x65656565)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(o->type)
    {
        case INDI_TYPE_NULL:
            indi_null_free((indi_null_t *) o);
            break;

        case INDI_TYPE_NUMBER:
            indi_number_free((indi_number_t *) o);
            break;

        case INDI_TYPE_BOOLEAN:
            indi_boolean_free((indi_boolean_t *) o);
            break;

        case INDI_TYPE_STRING:
            indi_string_free((indi_string_t *) o);
            break;

        case INDI_TYPE_LIST:
            indi_list_free((indi_list_t *) o);
            break;

        case INDI_TYPE_DICT:
            indi_dict_free((indi_dict_t *) o);
            break;

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_object_to_string(buff_t buff)
{
    indi_object_t *o = (indi_object_t *) buff;

    if(o == NULL || o->magic != 0x65656565)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(o->type)
    {
        case INDI_TYPE_NULL:
            return indi_null_to_string((indi_null_t *) o);

        case INDI_TYPE_NUMBER:
            return indi_number_to_string((indi_number_t *) o);

        case INDI_TYPE_BOOLEAN:
            return indi_boolean_to_string((indi_boolean_t *) o);

        case INDI_TYPE_STRING:
            return indi_string_to_string((indi_string_t *) o);

        case INDI_TYPE_LIST:
            return indi_list_to_string((indi_list_t *) o);

        case INDI_TYPE_DICT:
            return indi_dict_to_string((indi_dict_t *) o);

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_object_to_cstring(buff_t buff)
{
    indi_object_t *o = (indi_object_t *) buff;

    if(o == NULL || o->magic != 0x65656565)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(o->type)
    {
        case INDI_TYPE_NULL:
            return indi_null_to_string((indi_null_t *) o);

        case INDI_TYPE_NUMBER:
            return indi_number_to_string((indi_number_t *) o);

        case INDI_TYPE_BOOLEAN:
            return indi_boolean_to_string((indi_boolean_t *) o);

        case INDI_TYPE_STRING:
            return indi_string_to_cstring((indi_string_t *) o);

        case INDI_TYPE_LIST:
            return indi_list_to_string((indi_list_t *) o);

        case INDI_TYPE_DICT:
            return indi_dict_to_string((indi_dict_t *) o);

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
