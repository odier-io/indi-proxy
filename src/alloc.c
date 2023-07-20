/*--------------------------------------------------------------------------------------------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libxml/parser.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    size_t size;

    uint64_t magic;

} block_t;

/*--------------------------------------------------------------------------------------------------------------------*/

static size_t used_mem = 0;

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

void indi_memory_initialize()
{
    used_mem = 0;

    xmlMemSetup(
        (buff_t) indi_free,
        (buff_t) indi_alloc,
        (buff_t) indi_realloc,
        (buff_t) indi_string_dup
    );
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_memory_finalize()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    xmlCleanupParser();

    /*----------------------------------------------------------------------------------------------------------------*/

    if(used_mem > 0) fprintf(stderr, "Memory leak: %ld bytes!\n", used_mem);

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

size_t indi_free(buff_t buff)
{
    if(buff == NULL)
    {
        return 0x00;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    block_t *block = (block_t *) (((str_t) buff) - sizeof(block_t));

    if(block->magic != 0x7575757575757575)
    {
        fprintf(stderr, "Memory corruption!\n");
        fflush(stderr);
        exit(1);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    size_t result = block->size;

    used_mem -= result;

    free(block);

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t indi_alloc(size_t size)
{
    if(size == 0x00)
    {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    block_t *block = malloc(sizeof(block_t) + size);

    if(block == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        fflush(stderr);
        exit(1);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    used_mem += size;

    block->size = size;
    block->magic = 0x7575757575757575;

    /*----------------------------------------------------------------------------------------------------------------*/

    return (block_t *) (((str_t) block) + sizeof(block_t));

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

buff_t indi_realloc(buff_t buff, size_t size)
{
    if(buff == NULL) {
        return indi_alloc(size);
    }

    if(size == 0x00) {
        return /*------*/(NULL);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    block_t *old_block = (block_t *) (((str_t) buff) - sizeof(block_t));

    if(old_block->magic != 0x7575757575757575)
    {
        fprintf(stderr, "Memory corruption!\n");
        fflush(stderr);
        exit(1);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    used_mem -= old_block->size;

    /*----------------------------------------------------------------------------------------------------------------*/

    block_t *new_block = realloc(old_block, sizeof(block_t) + size);

    if(new_block == NULL)
    {
        fprintf(stderr, "Out of memory!\n");
        fflush(stderr);
        exit(1);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    used_mem += size;

    new_block->size = size;
    new_block->magic = 0x7575757575757575;

    /*----------------------------------------------------------------------------------------------------------------*/

    return (block_t *) (((str_t) new_block) + sizeof(block_t));

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_boolean_dup(bool b)
{
    str_t str;

    if(b) {
        str = strcpy(indi_alloc(4 + 1), "true");
    }
    else {
        str = strcpy(indi_alloc(5 + 1), "false");
    }

    return str;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_double_dup(double d)
{
    if(!isnan(d))
    {
        str_t str = indi_alloc(32 + 1);

        snprintf(str, 32, "%lf", d);

        return str;
    }

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_string_dup(STR_t s)
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
/*--------------------------------------------------------------------------------------------------------------------*/

void indi_object_free(indi_object_t *obj)
{
    if(obj == NULL)
    {
        return;
    }

    if(obj->magic != 0x6565656565656565)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(obj->type)
    {
        case INDI_TYPE_NULL:
            indi_null_free((indi_null_t *) obj);
            break;

        case INDI_TYPE_NUMBER:
            indi_number_free((indi_number_t *) obj);
            break;

        case INDI_TYPE_BOOLEAN:
            indi_boolean_free((indi_boolean_t *) obj);
            break;

        case INDI_TYPE_STRING:
            indi_string_free((indi_string_t *) obj);
            break;

        case INDI_TYPE_LIST:
            indi_list_free((indi_list_t *) obj);
            break;

        case INDI_TYPE_DICT:
            indi_dict_free((indi_dict_t *) obj);
            break;

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_object_to_string(const indi_object_t *obj)
{
    if(obj == NULL)
    {
        return NULL;
    }

    if(obj->magic != 0x6565656565656565)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(obj->type)
    {
        case INDI_TYPE_NULL:
            return indi_null_to_string((indi_null_t *) obj);

        case INDI_TYPE_NUMBER:
            return indi_number_to_string((indi_number_t *) obj);

        case INDI_TYPE_BOOLEAN:
            return indi_boolean_to_string((indi_boolean_t *) obj);

        case INDI_TYPE_STRING:
            return indi_string_to_string((indi_string_t *) obj);

        case INDI_TYPE_LIST:
            return indi_list_to_string((indi_list_t *) obj);

        case INDI_TYPE_DICT:
            return indi_dict_to_string((indi_dict_t *) obj);

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_object_to_cstring(const indi_object_t *obj)
{
    if(obj == NULL)
    {
        return NULL;
    }

    if(obj->magic != 0x6565656565656565)
    {
        fprintf(stderr, "Invalid object!\n");
        fflush(stderr);
        exit(1);
    }

    switch(obj->type)
    {
        case INDI_TYPE_NULL:
            return indi_null_to_string((indi_null_t *) obj);

        case INDI_TYPE_NUMBER:
            return indi_number_to_string((indi_number_t *) obj);

        case INDI_TYPE_BOOLEAN:
            return indi_boolean_to_string((indi_boolean_t *) obj);

        case INDI_TYPE_STRING:
            return indi_string_to_cstring((indi_string_t *) obj);

        case INDI_TYPE_LIST:
            return indi_list_to_string((indi_list_t *) obj);

        case INDI_TYPE_DICT:
            return indi_dict_to_string((indi_dict_t *) obj);

        default:
            fprintf(stderr, "Internal error!\n");
            fflush(stderr);
            exit(1);
    }
}

/*--------------------------------------------------------------------------------------------------------------------*/
