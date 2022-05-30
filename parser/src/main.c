#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGParser.h"
#include "SVGHelpers.h"
#include "LinkedListAPI.h"

//gcc -Wall -I/usr/include/libxml2/ -lm -I../include/ main.c SVGParser.c SVGHelpers.c LinkedListAPI.c -lxml2 -ggdb3//

int main (int argc, char **argv) {
    // if (argc != 2){
    //     printf("usage: xmlExample <someXMLfile>\n");

    //     return(1);
    // }

    //SVG *Object = malloc(sizeof(SVG));
    printf("%s\n", validImageToJSON("sample.svg", "../bin/files/svg.xsd"));

    //Object = createSVG(argv[1]);
    /*Free the object*/
    //deleteSVG(Object);
    
    return 0;
}
