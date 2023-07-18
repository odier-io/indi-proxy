#include <stdio.h>

#include <libxml/parser.h>

#include "indi_proxy_internal.h"

int main()
{
    if(indi_validation_init() != 0)
    {
        printf("Error initializing validation\n");
    }

    STR_t xml1 = "<defSwitchVector device=\"CCD Simulator\" name=\"CONNECTION\" label=\"Connection\" group=\"Main Control\" state=\"Ok\" perm=\"rw\" rule=\"OneOfMany\" timeout=\"60\" timestamp=\"2023-07-17T06:46:07\"><defSwitch name=\"CONNECT\" label=\"Connect\">On</defSwitch><defSwitch name=\"DISCONNECT\" label=\"Disconnect\">Off</defSwitch></defSwitchVector>";

    printf("%s\n\n", xml1);

    str_t json1 = indi_xml_to_json(xml1, true);

    printf("%s\n\n", json1);

    indi_object_t *test = indi_json_parse(json1);
    str_t json2 = indi_object_to_string(test);
    indi_object_free(test);

    printf("%s\n\n", json2);

    str_t xml2 = indi_json_to_xml(json2, false);

    printf("%s\n\n", xml2);

    indi_free(xml2);
    indi_free(json2);
    indi_free(json1);

    if(indi_validation_free() != 0)
    {
        printf("Error finalizing validation\n");
    }

    indi_memory_report();

    xmlCleanupParser();
    xmlMemoryDump();

    return 0;
}
