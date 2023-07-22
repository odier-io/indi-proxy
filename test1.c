/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#include "src/indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static STR_t XML = "<defSwitchVector device=\"CCD Simulator\" name=\"CONNECTION\" label=\"Connection\" group=\"Main Control\" state=\"Ok\" perm=\"rw\" rule=\"OneOfMany\" timeout=\"60\" timestamp=\"2023-07-17T06:46:07\"><defSwitch name=\"CONNECT\" label=\"Connect\">On</defSwitch><defSwitch name=\"DISCONNECT\" label=\"Disconnect\">Off</defSwitch></defSwitchVector>\n";

/*--------------------------------------------------------------------------------------------------------------------*/

int main()
{
    /*----------------------------------------------------------------------------------------------------------------*/

    indi_memory_initialize();

    /*----------------------------------------------------------------------------------------------------------------*/

    if(indi_validation_initialize() == false)
    {
        printf("Error initializing validation\n");

        goto _err;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_xmldoc_t *doc1 = indi_xml_parse(XML);
    indi_object_t *obj1 = indi_xmldoc_to_object(doc1, true);
    str_t json1 = indi_object_to_string(obj1);
    indi_object_free(obj1);
    indi_xmldoc_free(doc1);

    indi_object_t *test = indi_json_parse(json1);
    str_t json2 = indi_object_to_string(test);
    indi_object_free(test);

    indi_object_t *obj2 = indi_json_parse(json2);
    indi_xmldoc_t *doc2 = indi_object_to_xmldoc(obj2, false);
    str_t xml = indi_xmldoc_to_string(doc2);
    indi_xmldoc_free(doc2);
    indi_object_free(obj2);

    /*----------------------------------------------------------------------------------------------------------------*/

    int ret = (xml != NULL) && strcmp(XML, xml) == 0 ? 0 : 1;

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_free(json2);
    indi_free(xml);
    indi_free(json1);

    /*----------------------------------------------------------------------------------------------------------------*/

    if(indi_validation_finalize() == false)
    {
        printf("Error finalizing validation\n");

        goto _err;
    }

    /*----------------------------------------------------------------------------------------------------------------*/

    indi_memory_finalize();

    printf("[SUCCESS]\n");

    return ret;

_err:
    indi_memory_finalize();

    printf("[ERROR]\n");

    return 0x1;
}

/*--------------------------------------------------------------------------------------------------------------------*/
