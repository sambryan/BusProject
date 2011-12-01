#include "dbthread.h"
#include <libxml/xpath.h>
#include <curl/curl.h>
#include <cstdlib>
#include <cstring>

#define URL "http://transport.tamu.edu/busroutes/busPosition.aspx"

typedef struct {
    char *data;
    size_t size;
} sk_str;

int sk_curlwrite(void *ptr, size_t size, size_t nmemb, sk_str *userdata)
{
    if(userdata->size + size*nmemb > 10240) return 0;
    size_t newlen = size + nmemb*size;
    memcpy(userdata->data+userdata->size, ptr, size*nmemb);
    userdata->data[newlen] = '\0';
    userdata->size = newlen;
    return size*nmemb;
}


void SetBusLocations(std::vector<busCoord>** busloc)
{
    xmlInitParser();
    // DO WORK
    xmlDocPtr doc;
    xmlXPathContextPtr xpathCtx; 
    xmlXPathObjectPtr xpathObj; 


    /* Fetch data */
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    sk_str curldata;
    curldata.data = (char*)malloc(10240);
    curldata.size = 0;
    curldata.data[0] = '\0';

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, sk_curlwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curldata);
        res = curl_easy_perform(curl);
    }
    else {
        printf("Failed to get website\n");
        return;
    }

    printf("Website data (Res: %d):\n", res);

    
    printf("%s", curldata.data);
    //xmlParseMemory(ptr, size);
}

