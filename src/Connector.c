/* $CORTO_GENERATED
 *
 * Connector.c
 *
 * Only code written between the begin and end tags will be preserved
 * when the file is regenerated.
 */

#include "corto/net/weather/weather.h"

/* $header() */
typedef struct weather_onRequest_data {
    corto_result result;
    JSON_Value *value;
} weather_onRequest_data;

int weather_iterHasNext(corto_iter *iter) {
    weather_onRequest_data *data = iter->udata;
    return data->value != NULL;
}

void* weather_iterNext(corto_iter *iter) {
    weather_onRequest_data *data = iter->udata;
    corto_string json;

    JSON_Object *o = json_value_get_object(data->value);
    JSON_Object *main = json_object_dotget_object(o, "main");
    JSON_Object *weather = json_array_get_object(json_object_dotget_array(o, "weather"), 0);
    JSON_Object *wind = json_object_dotget_object(o, "wind");

    corto_asprintf(&json,
      "{\"main\":\"%s\",\"description\":\"%s\",\"temp_min\":%f,\"temp_max\":%f"
      ",\"pressure\":%f,\"humidity\":%f,\"wind\":{\"speed\":%f,\"degree\":%f}}",
      json_object_dotget_string(weather, "main"),
      json_object_dotget_string(weather, "description"),
      json_object_dotget_number(main, "temp_min"),
      json_object_dotget_number(main, "temp_max"),
      json_object_dotget_number(main, "pressure"),
      json_object_dotget_number(main, "humidity"),
      json_object_dotget_number(wind, "speed"),
      json_object_dotget_number(wind, "deg")
    );

    data->result.value = (corto_word)json;

    return &data->result;
}

void weather_iterRelease(corto_iter *iter) {
    weather_onRequest_data *data = iter->udata;
    corto_dealloc(data);
}

/* $end */

corto_int16 _weather_Connector_construct(
    weather_Connector this)
{
/* $begin(corto/net/weather/Connector/construct) */
    corto_replicator_setContentType(this, "text/json");

    /* Augment all cities with weather data */
    corto_type cityType = corto_resolve(NULL, "corto/net/cities/City");
    if (!cityType) {
        corto_seterr("failed to resolve corto/net/cities/City");
        goto error;
    }

    corto_augment(cityType, "weather", this);
    corto_release(cityType);

    return corto_replicator_construct(this);
error:
    return -1;
/* $end */
}

corto_resultIter _weather_Connector_onRequest(
    weather_Connector this,
    corto_request *request)
{
/* $begin(corto/net/weather/Connector/onRequest) */
    corto_resultIter it;
    weather_onRequest_data *data = corto_calloc(sizeof(weather_onRequest_data));
    client_Url url; memset(&url, 0, sizeof(url));

    corto_asprintf(
      &url.address,
      "http://api.openweathermap.org/data/2.5/weather?id=%s&appid=%s&units=imperial",
      request->expr,
      "3705771a7e43d8682c57a020e0992518");

    client_Url_request(&url);
    corto_dealloc(url.address);

    data->value = json_parse_string(url.response);
    it.udata = data;
    it.next = weather_iterNext;
    it.hasNext = weather_iterHasNext;
    it.release = weather_iterRelease;

    return it;
/* $end */
}
