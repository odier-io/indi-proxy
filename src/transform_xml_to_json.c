/*--------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

#include <libxml/tree.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_dict_t *xml_to_json(xmlNode *curr_node) // NOLINT(misc-no-recursion)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_t *result = indi_dict_new();

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_dict_put(result, "<>", indi_string_from((str_t) curr_node->name));

    /*----------------------------------------------------------------------------------------------------------------*/

    for(xmlNode *new_node = curr_node->children; new_node != NULL; new_node = new_node->next)
    {
        if(new_node->type == XML_TEXT_NODE && strlen((str_t) new_node->content) > 0)
        {
            indi_dict_put(result, "$", indi_string_from((str_t) new_node->content));

            break;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    for(xmlAttr *attribute = curr_node->properties; attribute != NULL; attribute = attribute->next)
    {
        indi_string_t *obj = indi_string_new();

        /**/    indi_string_append(obj, /*-----*/ "@" /*-----*/);
        /**/    indi_string_append(obj, (str_t) attribute->name);
        /**/
        /**/    str_t attribute_name = indi_string_to_cstring(obj);
        /**/
        /**/    /**/    xmlChar *attribute_val = xmlNodeListGetString(curr_node->doc, attribute->children, 1);
        /**/    /**/
        /**/    /**/    /**/    indi_dict_put(result, (str_t) attribute_name, indi_string_from((str_t) attribute_val));
        /**/    /**/
        /**/    /**/    xmlFree(attribute_val);
        /**/
        /**/    indi_free(attribute_name);

        indi_string_free(obj);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(curr_node->children)
    {
        indi_list_t *list = NULL;

        for(xmlNode *new_node = curr_node->children; new_node != NULL; new_node = new_node->next)
        {
            if(new_node->type == XML_ELEMENT_NODE)
            {
                if(list == NULL)
                {
                    indi_dict_put(result, "children", list = indi_list_new());
                }

                indi_list_push(list, xml_to_json(new_node));
            }
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/

indi_object_t *indi_xmldoc_to_object(indi_xmldoc_t *doc, bool validate)
{
    if(doc == NULL)
    {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    xmlNode *root = xmlDocGetRootElement(doc);

    if(root == NULL)
    {
        return NULL;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    if(validate == false || indi_validation_check(doc) == true)
    {
        return (indi_object_t *) xml_to_json(root);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/
