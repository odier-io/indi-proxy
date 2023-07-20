/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include "src/indi_proxy.h"

/*--------------------------------------------------------------------------------------------------------------------*/

int main()
{
    indi_memory_initialize();

    if(indi_validation_init() == false)
    {
        printf("Error initializing validation\n");
    }

    STR_t xml1 = "<defSwitchVector device=\"CCD Simulator\" name=\"CONNECTION\" label=\"Connection\" group=\"Main Control\" state=\"Ok\" perm=\"rw\" rule=\"OneOfMany\" timeout=\"60\" timestamp=\"2023-07-17T06:46:07\"><defSwitch name=\"CONNECT\" label=\"Connect\">On</defSwitch><defSwitch name=\"DISCONNECT\" label=\"Disconnect\">Off</defSwitch></defSwitchVector>";

    indi_xmldoc_t *doc1 = indi_xml_parse(xml1);
    indi_object_t *obj1 = indi_xmldoc_to_object(doc1, true);
    str_t json1 = indi_object_to_string(obj1);
    indi_object_free(obj1);
    indi_xmldoc_free(doc1);

    indi_object_t *test = indi_json_parse(json1);
    str_t json2 = indi_object_to_string(test);
    indi_object_free(test);

    indi_object_t *obj2 = indi_json_parse(json2);
    indi_xmldoc_t *doc2 = indi_object_to_xmldoc(obj2, false);
    str_t xml2 = indi_xml_to_string(doc2);
    indi_xmldoc_free(doc2);
    indi_object_free(obj2);

    printf("xml1: %s\n\n", xml1);
    printf("json1: %s\n\n", json1);
    printf("json2: %s\n\n", json2);
    printf("xml2: %s\n\n", xml2);

    indi_free(xml2);
    indi_free(json2);
    indi_free(json1);
    ////_free(xml1);

    if(indi_validation_free() == false)
    {
        printf("Error finalizing validation\n");
    }

    indi_memory_finalize();

    return 0;
}

/*--------------------------------------------------------------------------------------------------------------------*/
