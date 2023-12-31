/*--------------------------------------------------------------------------------------------------------------------*/

#ifndef INDI_PROXY_INDI_PROXY_H
#define INDI_PROXY_INDI_PROXY_H

/*--------------------------------------------------------------------------------------------------------------------*/

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*--------------------------------------------------------------------------------------------------------------------*/

#define buff_t /* */ void *
#define BUFF_t const void *

#define str_t /* */ char *
#define STR_t const char *

/*--------------------------------------------------------------------------------------------------------------------*/
/* MEMORY                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

void indi_memory_initialize();

void indi_memory_finalize();

/*--------------------------------------------------------------------------------------------------------------------*/

size_t indi_memory_free(
    buff_t buff
);

buff_t indi_memory_alloc(
    size_t size
);

buff_t indi_memory_realloc(
    buff_t buff,
    size_t size
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* OBJECT                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

#define INDI_OBJECT_MAGIC 0x6565656565656565

/*--------------------------------------------------------------------------------------------------------------------*/

typedef enum
{
    INDI_TYPE_NULL      = 100,
    INDI_TYPE_BOOLEAN   = 101,
    INDI_TYPE_NUMBER    = 102,
    INDI_TYPE_STRING    = 103,
    INDI_TYPE_DICT      = 104,
    INDI_TYPE_LIST      = 105,

} indi_type_t;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct
{
    uint64_t magic;

    indi_type_t type;

} indi_object_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_object_t *indi_object_parse(
    STR_t text
);

void indi_object_free(
    /*-*/ indi_object_t *obj
);

str_t indi_object_to_string(
    const indi_object_t *obj
);

str_t indi_object_to_cstring(
    const indi_object_t *obj
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* XMLDOC                                                                                                             */
/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct _xmlDoc indi_xmldoc_t;

/*--------------------------------------------------------------------------------------------------------------------*/

indi_xmldoc_t *indi_xmldoc_parse(
    STR_t text
);

void indi_xmldoc_free(
    /*-*/ indi_xmldoc_t *doc
);

str_t indi_xmldoc_to_string(
    const indi_xmldoc_t *doc
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* VALIDATION                                                                                                         */
/*--------------------------------------------------------------------------------------------------------------------*/

bool indi_validation_initialize();

bool indi_validation_finalize();

bool indi_validation_check(
    indi_xmldoc_t *doc
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* TRANSFORM                                                                                                          */
/*--------------------------------------------------------------------------------------------------------------------*/

indi_object_t *indi_xmldoc_to_object(
    indi_xmldoc_t *doc,
    bool validate
);

indi_xmldoc_t *indi_object_to_xmldoc(
    indi_object_t *obj,
    bool validate
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* PROXY                                                                                                              */
/*--------------------------------------------------------------------------------------------------------------------*/

struct indi_proxy_s;

/*--------------------------------------------------------------------------------------------------------------------*/

typedef void (* indi_emit_func_t)(struct indi_proxy_s *proxy, size_t size, str_t buff);

/*--------------------------------------------------------------------------------------------------------------------*/

typedef struct indi_proxy_s
{
    indi_emit_func_t emit_func;

    size_t xml_stream_size;
    str_t xml_stream_buff;

    size_t xml_residual_size;
    str_t xml_residual_buff;

    size_t etag_size;
    BUFF_t etag_buff;

    int32_t state;
    size_t pos;
    void *py;

} indi_proxy_t;

/*--------------------------------------------------------------------------------------------------------------------*/

void indi_proxy_initialize(
    indi_proxy_t *proxy,
    size_t xml_stream_size,
    size_t xml_residual_size,
    indi_emit_func_t emit_func
);

void indi_proxy_finalize(
    indi_proxy_t *proxy
);

void indi_proxy_consume(
    indi_proxy_t *proxy,
    size_t size,
    STR_t buff
);

/*--------------------------------------------------------------------------------------------------------------------*/
/* CONTROL                                                                                                            */
/*--------------------------------------------------------------------------------------------------------------------*/

str_t indi_driver_list(
    STR_t path
);

int indi_server_start(
    STR_t path,
    STR_t json
);

int indi_server_stop();

/*--------------------------------------------------------------------------------------------------------------------*/

#endif /* INDI_PROXY_INDI_PROXY_H */

/*--------------------------------------------------------------------------------------------------------------------*/
