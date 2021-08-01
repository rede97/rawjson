#include "rawjson_deserialize.h"

rawjson_der_status_t rawjson_fast_der_init(rawjson_der_t *der, rawjson_der_err_cb on_err, char *json_str, size_t len_str)
{
    rawjson_der_status_t ret = RAWJSON_DER_OK;
    rawjson_der_err_t err = {0};
    der->on_err = on_err;
    der->doc = yyjson_read_opts(json_str, len_str, YYJSON_READ_INSITU, NULL, &err.yyjson);
    if (der->doc)
    {
        der->curr = yyjson_doc_get_root(der->doc);
    }
    else
    {
        ret = RAWJSON_DER_YYJSON_ERR;
        if (on_err)
        {
            on_err(der, ret, &err);
        }
    }
    return ret;
}

rawjson_der_status_t rawjson_der_init(rawjson_der_t *der, rawjson_der_err_cb on_err, const char *json_str, size_t len_str)
{
    rawjson_der_status_t ret = RAWJSON_DER_OK;
    rawjson_der_err_t err = {0};
    der->on_err = on_err;
    der->doc = yyjson_read_opts((char *)json_str, len_str, 0, NULL, &err.yyjson);
    if (der->doc)
    {
        der->curr = yyjson_doc_get_root(der->doc);
    }
    else
    {
        ret = RAWJSON_DER_YYJSON_ERR;
        if (on_err)
        {
            on_err(der, ret, &err);
        }
    }
    return ret;
}

rawjson_der_status_t rawjson_der_rfc3399(rawjson_der_t *der, struct tm *time, __useconds_t *usec)
{
    rawjson_der_status_t ret = RAWJSON_DER_OK;
}
