/*--------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

#include "../indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_dict_node_s
{
    STR_t key;

    struct indi_object_s *val;

    struct indi_dict_node_s *next;

} node_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_dict_t *indi_dict_new()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_t *o = indi_alloc(sizeof(indi_dict_t));

    /*----------------------------------------------------------------------------------------------------------------*/

    o->object.magic = 0x65656565;
    o->object.type = INDI_TYPE_DICT;

    /*----------------------------------------------------------------------------------------------------------------*/

    o->head = NULL;
    o->tail = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    return o;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_dict_free(indi_dict_t *o)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *node = o->head; node != NULL;)
    {
        /*------------------------------------------------------------------------------------------------------------*/

        node_t *temp = node;

        node = node->next;

        /*------------------------------------------------------------------------------------------------------------*/

        indi_object_free(temp->val);

        indi_free(temp);

        /*------------------------------------------------------------------------------------------------------------*/
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_free(o);

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_dict_del(indi_dict_t *o, STR_t key)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *prev_node = NULL, *curr_node = o->head; curr_node != NULL; prev_node = curr_node, curr_node = curr_node->next)
    {
        if(strcmp(curr_node->key, key) == 0)
        {
            /*--------------------------------------------------------------------------------------------------------*/

            if(prev_node == NULL)
            {
                o->head = curr_node->next;
            }
            else
            {
                prev_node->next = curr_node->next;
            }

            /*--------------------------------------------------------------------------------------------------------*/

            indi_object_free(curr_node->val);

            indi_free(curr_node);

            /*--------------------------------------------------------------------------------------------------------*/

            break;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_object_t *indi_dict_get(indi_dict_t *o, STR_t key)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *curr_node = o->head; curr_node != NULL; curr_node = curr_node->next)
    {
        if(strcmp(curr_node->key, key) == 0)
        {
            return curr_node->val;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_dict_put(indi_dict_t *o, STR_t key, buff_t val)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(node_t *curr_node = o->head; curr_node != NULL; curr_node = curr_node->next)
    {
        if(strcmp(curr_node->key, key) == 0)
        {
            indi_object_free(curr_node->val);

            curr_node->val = val;

            return;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    node_t *node = (node_t *) indi_alloc(sizeof(node_t) + strlen(key) + 1);

    node->key = strcpy((str_t) (node + 1), key);

    node->val = val;
    node->next = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    if(o->head == NULL)
    {
        o->head = node;
        o->tail = node;
    }
    else
    {
        o->tail->next = node;
        o->tail /*-*/ = node;
    }

    /*----------------------------------------------------------------------------------------------------------------*/
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_dict_to_string(indi_dict_t *o)
{
    indi_string_t *string = indi_string_new();

    /**/    indi_string_append(string, "{");
    /**/
    /**/    for(node_t *curr_node = o->head; curr_node != NULL; curr_node = curr_node->next)
    /**/    {
    /**/        str_t curr_node_val = indi_object_to_string(curr_node->val);
    /**/
    /**/        /**/    indi_string_append(string, "\"");
    /**/        /**/    indi_string_append(string, curr_node->key);
    /**/        /**/    indi_string_append(string, "\"");
    /**/        /**/    indi_string_append(string, ":");
    /**/        /**/    indi_string_append(string, curr_node_val);
    /**/
    /**/        indi_free(curr_node_val);
    /**/
    /**/        if(curr_node->next != NULL)
    /**/        {
    /**/            indi_string_append(string, ",");
    /**/        }
    /**/    }
    /**/
    /**/    indi_string_append(string, "}");

    str_t result = indi_string_to_cstring(string);

    indi_string_free(string);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
