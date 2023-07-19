/*--------------------------------------------------------------------------------------------------------------------*/

#include <stdio.h>

#include <libxml/xmlschemas.h>

#include "indi_proxy_internal.h"

#include "indi_proxy_schema.h"

/*--------------------------------------------------------------------------------------------------------------------*/

static xmlSchema *INDI_SCHEMA = NULL;

/*--------------------------------------------------------------------------------------------------------------------*/

bool indi_validation_init()
{
    xmlSchemaParserCtxt *context = xmlSchemaNewMemParserCtxt(indi_proxy_xsd_buff, INDI_PROXY_XSD_SIZE);

    if(context == NULL)
    {
        return false;
    }

    /**/    /*--------------------------------------------------------------------------------------------------------*/
    /**/
    /**/    xmlSchemaSetParserErrors(
    /**/        context,
    /**/        (xmlSchemaValidityErrorFunc) fprintf,
    /**/        (xmlSchemaValidityWarningFunc) fprintf,
    /**/        stderr
    /**/    );
    /**/
    /**/    /*--------------------------------------------------------------------------------------------------------*/
    /**/
    /**/    INDI_SCHEMA = xmlSchemaParse(context);
    /**/
    /**/    /*--------------------------------------------------------------------------------------------------------*/

    xmlSchemaFreeParserCtxt(context);

    return INDI_SCHEMA != NULL;
}

/*--------------------------------------------------------------------------------------------------------------------*/

bool indi_validation_free()
{
    if(INDI_SCHEMA != NULL)
    {
        xmlSchemaFree(INDI_SCHEMA);

        return true;
    }

    return false;
}

/*--------------------------------------------------------------------------------------------------------------------*/

bool indi_validation_check(struct _xmlDoc *doc)
{
    xmlSchemaValidCtxt *context = xmlSchemaNewValidCtxt(INDI_SCHEMA);

    if(context == NULL)
    {
        return false;
    }

    /**/    /*--------------------------------------------------------------------------------------------------------*/
    /**/
    /**/    xmlSchemaSetValidErrors(
    /**/        context,
    /**/        (xmlSchemaValidityErrorFunc) fprintf,
    /**/        (xmlSchemaValidityWarningFunc) fprintf,
    /**/        stderr
    /**/    );
    /**/
    /**/    /*--------------------------------------------------------------------------------------------------------*/
    /**/
    /**/    int result = xmlSchemaValidateDoc(context, (xmlDoc *) doc) == 0;
    /**/
    /**/    /*--------------------------------------------------------------------------------------------------------*/

    xmlSchemaFreeValidCtxt(context);

    return result;
}

/*--------------------------------------------------------------------------------------------------------------------*/
