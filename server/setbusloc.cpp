#include "dbthread.h"
#include <libxml/xpath.h>
#include <curl/curl.h>
#include <cstdlib>
#include <cstring>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpathInternals.h>

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
    static bool firstrun = true;
    if ( firstrun ) {
        xmlInitParser();
        firstrun = false;
    }

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

    printf("Website data (Res: %d, size: %d):\n", res, curldata.size);

    
    // printf("%s", curldata.data);

    doc = xmlParseMemory(curldata.data, curldata.size);

    if ( doc == NULL ) {
        printf("Unable to parse new data\n");
        free(curldata.data);
        return;
    }

    xpathCtx = xmlXPathNewContext(doc);
    if(xpathCtx == NULL) {
        fprintf(stderr,"Error: unable to create new XPath context\n");
        xmlFreeDoc(doc); 
        free(curldata.data);
        return;
    }

    /* Evaluate xpath expression */
    xpathObj = xmlXPathEvalExpression((xmlChar*)"/data/bus", xpathCtx);
    if(xpathObj == NULL) {
        printf("Error: unable to evaluate xpath expression\n");
        xmlXPathFreeContext(xpathCtx); 
        xmlFreeDoc(doc); 
        free(curldata.data);
        return;
    }

    xmlNodePtr cur;
    int size;
    int i;

    xmlNodeSetPtr nodes = xpathObj->nodesetval;
    size = (nodes) ? nodes->nodeNr : 0;

    printf("Result (%d nodes):\n", size);
    for(i = 0; i < size; ++i) {
        cur = nodes->nodeTab[i];    
        printf("= node \"%s\":\n", cur->name); //  content: %s\n", cur->name, (char*)xmlNodeGetContent(cur));

        xmlNodePtr child = cur->children;
        while ( true ) {
            printf("== node \"%s\": content: %s\n", child->name, (char*)xmlNodeGetContent(child));
            if ( strcmp((char*)child->name, "route_id") == 0 ) {
                atoi((char*)xmlNodeGetContent(child));
            }
            if ( child == cur->last )
                break;
            else
                child = child->next;
        }
    }

    xmlXPathFreeContext(xpathCtx); 
    xmlFreeDoc(doc); 
    free(curldata.data);
    curl_easy_cleanup(curl);
}

