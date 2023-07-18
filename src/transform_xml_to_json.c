/*--------------------------------------------------------------------------------------------------------------------*/

#include <string.h>

#include <libxml/tree.h>

#include "indi_proxy_internal.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static indi_dict_t *xml_to_json(indi_dict_t *dict, xmlNode *curr_node) // NOLINT(misc-no-recursion)
{
    /*----------------------------------------------------------------------------------------------------------------*/

    for(xmlNode *new_node = curr_node->children; new_node != NULL; new_node = new_node->next)
    {
        if(new_node->type == XML_TEXT_NODE && strlen((str_t) new_node->content) > 0)
        {
            indi_dict_put(dict, "$", indi_string_from((str_t) new_node->content));

            break;
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    for(xmlAttr *attribute = curr_node->properties; attribute != NULL; attribute = attribute->next)
    {
        indi_string_t *o = indi_string_new();

        /**/    indi_string_append(o, /*-----*/ "@" /*-----*/);
        /**/    indi_string_append(o, (str_t) attribute->name);
        /**/
        /**/    str_t attribute_name = indi_string_to_cstring(o);
        /**/
        /**/    /**/    xmlChar *attribute_val = xmlNodeListGetString(curr_node->doc, attribute->children, 1);
        /**/    /**/
        /**/    /**/    /**/    indi_dict_put(dict, (str_t) attribute_name, indi_string_from((str_t) attribute_val));
        /**/    /**/
        /**/    /**/    xmlFree(attribute_val);
        /**/
        /**/    indi_free(attribute_name);

        indi_string_free(o);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    for(xmlNode *new_node = curr_node->children; new_node != NULL; new_node = new_node->next)
    {
        if(new_node->type == XML_ELEMENT_NODE)
        {
            /*--------------------------------------------------------------------------------------------------------*/

            indi_list_t *list = (indi_list_t *) indi_dict_get(dict, (str_t) new_node->name);

            if(list == NULL)
            {
                list = indi_list_new();

                indi_dict_put(dict, (str_t) new_node->name, list);
            }

            /*--------------------------------------------------------------------------------------------------------*/

            indi_list_push(list, xml_to_json(indi_dict_new(), new_node));

            /*--------------------------------------------------------------------------------------------------------*/
        }
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    return dict;
}

/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_xml_to_json(STR_t xml, bool validate)
{
    str_t result = NULL;

    /*----------------------------------------------------------------------------------------------------------------*/

    xmlDoc *doc = xmlReadMemory(xml, (int) strlen(xml), "message.xml", "utf-8", 0);

    if(doc != NULL)
    {
        if(validate && indi_validation_check(doc) == false)
        {
            goto _err1;
        }
    }
    else
    {
        goto _err0;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    xmlNode *root = xmlDocGetRootElement(doc);

    if(root != NULL)
    {
        indi_dict_t *dict = indi_dict_new();

        result = indi_dict_to_string(
            xml_to_json(dict, root)
        );

        indi_dict_free(dict);
    }

    /*----------------------------------------------------------------------------------------------------------------*/

_err1:
    xmlFreeDoc(doc);

_err0:
    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/