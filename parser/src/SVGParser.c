/**
 * @file SVGParser.c
 * @author Abdullah Khan (mkhan94@uoguelph.ca) - 1101209
 * @brief This project/code focuses on creating a set of structs representing SVG files
 * using libxml2. It then goes to using a set of accessor and search functions that will
 * accompany the parser and the structs.
 * @date 2022-01-20
 * @copyright Copyright (c) 2022
 * Compilation: gcc -Wall -I/usr/include/libxml2/ -lm -I../include/ main.c SVGParser.c SVGHelpers.c LinkedListAPI.c -lxml2 -ggdb3
 * Run: ./a.out [.xml/.svg file]
 * When running Test Harness: export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>

#include "SVGParser.h"
#include "SVGHelpers.h"
#include "LinkedListAPI.h"

#define LIBXML_SCHEMAS_ENABLED

/******************************  LinkedList Functions *******************************/

/*SVG Attributes List functions*/
char *attributeToString(void* data) {
    char *tmpStr;
    Attribute *tmpAttr;
    int length;

    if (data == NULL) {
        return NULL;
    }

    tmpAttr = (Attribute*)data;

    length = strlen(tmpAttr->name) + strlen(tmpAttr->value) + strlen("\tName: %s Value: %s");
    tmpStr = (char *)malloc(sizeof(char) * length);
    
    sprintf(tmpStr, "\tName: %s Value: %s", tmpAttr->name, tmpAttr->value);

    return tmpStr;
}

void deleteAttribute(void* data) {
    Attribute* tmpAttr;

    if (data == NULL) {
        return;
    }

    tmpAttr = (Attribute*)data;

    if(tmpAttr->name != NULL) {
        free(tmpAttr->name);
    }
    free(tmpAttr);
}

int compareAttributes(const void* first, const void* second) {
    Attribute* tmpAttr1;
	Attribute* tmpAttr2;
	
	if (first == NULL || second == NULL){
		return 0;
	}
	
	tmpAttr1 = (Attribute*)first;
	tmpAttr2 = (Attribute*)second;
	
	return strcmp((char*)tmpAttr1->name, (char*)tmpAttr2->name);
}

/*Rectangle List functions*/
char *rectangleToString(void* data) {
    char *tmpStr;
    Rectangle *tmpRect;
    int length;

    if (data == NULL) {
        return NULL;
    }

    tmpRect = (Rectangle*)data;

    char *attrString;
    attrString = toString(tmpRect->otherAttributes);
 
    length = strlen(tmpRect->units) + 40 + 40 + 40 + 40 + 26 + strlen(attrString);
    tmpStr = (char *)malloc(sizeof(char) * length);
    
    sprintf(tmpStr, "x: %0.2f y: %.2f weight: %0.2f height: %.2f", tmpRect->x, tmpRect->y, tmpRect->width, tmpRect->height);
    strcat(tmpStr, attrString);

    free(attrString);

    return tmpStr;
}

void deleteRectangle(void* data) {
    if (data == NULL) {
        return;
    }

    Rectangle *tmpRect = (Rectangle *)data;

    freeList(tmpRect->otherAttributes);
    free(tmpRect);
}

int compareRectangles(const void* first, const void* second) {
	if (first == NULL || second == NULL){
		return 0;
	}
	
	Rectangle* tmpRect1 = (Rectangle*)first;
	Rectangle* tmpRect2 = (Rectangle*)second;
	
	return strcmp((char*)tmpRect1->units, (char*)tmpRect2->units);
}

/*Circle List functions*/
char *circleToString(void* data) {
    char *tmpStr;
    int length;

    if (data == NULL) {
        return NULL;
    }

    Circle *tmpCirc = (Circle*)data;

    char *attrString;
    //char *cxString;

    attrString = toString(tmpCirc->otherAttributes);
    //sprintf(cxString, "%0.2f", tmpCirc->cx);
 
    length = strlen(tmpCirc->units) + 40 + 40 + 40 + 14 + strlen(attrString);
    tmpStr = (char *)malloc(sizeof(char) * length);

    sprintf(tmpStr, "cx: %0.2f cy: %0.2f r: %0.2f", tmpCirc->cx, tmpCirc->cy, tmpCirc->r);
    strcat(tmpStr, attrString);

    free(attrString);

    return tmpStr;
}

void deleteCircle(void* data) {
    if (data == NULL) {
        return;
    }

    Circle *tmpCirc = (Circle *)data;

    freeList(tmpCirc->otherAttributes);
    free(tmpCirc);
}

int compareCircles(const void* first, const void* second) {
    if (first == NULL || second == NULL){
		return 0;
	}
	
	Circle* tmpCirc1 = (Circle*)first;
	Circle* tmpCirc2 = (Circle*)second;
	
	return strcmp((char*)tmpCirc1->units, (char*)tmpCirc2->units);
}

/*Path List functions*/
void deletePath(void* data) {
    if (data == NULL) {
        return;
    }

    Path *tmpPath = (Path *)data;

    freeList(tmpPath->otherAttributes);
    free(tmpPath);
}

char* pathToString(void* data) {
    char *tmpStr;
    int length;

    if (data == NULL) {
        return NULL;
    }

    Path *tmpPath = (Path*)data;

    char *attrString;
    attrString = toString(tmpPath->otherAttributes);
 
    length = strlen(tmpPath->data) + 7 + strlen(attrString);
    //tmpStr = (char *)malloc(sizeof(char) * length);
    tmpStr = malloc(sizeof(char) * length);
    
    sprintf(tmpStr, "Data: %s", tmpPath->data);
    strcat(tmpStr, attrString);

    free(attrString);

    return tmpStr;
}

int comparePaths(const void *first, const void *second) {
    if (first == NULL || second == NULL) {
		return 0;
	}
	
	Path* tmpPath1 = (Path*)first;
	Path* tmpPath2 = (Path*)second;
	
	return strcmp((char*)tmpPath1->data, (char*)tmpPath2->data);
}

/*Group List functions*/
void deleteGroup(void* data) {
    if (data == NULL) {
        return;
    }

    Group *tmpGroup = (Group*)data;

    freeList(tmpGroup->circles);
    freeList(tmpGroup->rectangles);
    freeList(tmpGroup->paths);
    freeList(tmpGroup->groups);
    //freeList(tmpGroup->otherAttributes);
    free(tmpGroup);
}

char* groupToString(void* data) {
    char *mainString, *rectString, *circString, *pathString, *groupString, *otherAttrString;
    Group *group;
    
    if (data == NULL) {
        return NULL;
    }

    group = (Group*)data;

    rectString = toString(group->rectangles);
    circString = toString(group->circles);
    pathString = toString(group->paths);
    otherAttrString = toString(group->otherAttributes);
    groupString = toString(group->groups);

    mainString = malloc(sizeof(char) * (strlen(rectString) + strlen(circString) + 
    strlen(pathString) + strlen(otherAttrString) + strlen(groupString) + 
    strlen("Group Attributes:") + 3));

    strcpy(mainString, "\nGroup Attributes:");
    strcat(mainString, otherAttrString);
    strcat(mainString, rectString);
    strcat(mainString, circString);
    strcat(mainString, pathString);
    strcat(mainString, groupString);

    /*Free the strings*/
    free(rectString);
    free(circString);
    free(pathString);
    free(otherAttrString);
    free(groupString);
    
    return mainString;
}

int compareGroups(const void *first, const void *second) {
    if (first == NULL || second == NULL) {
        return 0;
    } else {
        return 1;
    }
}

/**************************** SVG Accessor Functions ********************************/

/*Function that returns a list of all rectangles in the struct*/
List* getRects(const SVG* img) {
    List* rectList = NULL;
    List* groupList;

    if (img == NULL) {
        return rectList;
    }

    ListIterator rectIterator;
    ListIterator groupIterator;
    Group *tmpGroup;
    void *elem;

    rectList = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    rectIterator = createIterator(img->rectangles);
    groupList = getGroups(img);
    groupIterator = createIterator(groupList);
    /*Finds all rectangle objects in the SVG object*/
    while ((elem = nextElement(&rectIterator)) != NULL) {
        if (elem) {
            insertBack(rectList, (void*)elem);
        }
    }
    /*Looks through the Group objects for any rectangle objects*/
    while ((tmpGroup = (Group*)nextElement(&groupIterator)) != NULL) {
        rectIterator = createIterator(tmpGroup->rectangles);
        
        while ((elem = nextElement(&rectIterator)) != NULL) {
            if (elem) {
                insertBack(rectList, (void*)elem);
            }
        }
    }
    
    return rectList;
}

/*Function that returns a list of all circles in the struct*/
List* getCircles(const SVG* img) {
    List* circleList = NULL;
    List* groupList;

    if (img == NULL) {
        return circleList;
    }

    ListIterator circleIter;
    ListIterator groupIterator;
    Group *tmpGroup;
    void *elem;

    circleList = initializeList(&circleToString, &deleteCircle, &compareCircles);
    circleIter = createIterator(img->circles);
    groupList = getGroups(img);
    groupIterator = createIterator(groupList);

    while ((elem = nextElement(&circleIter)) != NULL) {
        if (elem) {
            insertBack(circleList, (void*)elem);
        }
    }

    while ((tmpGroup = (Group*)nextElement(&groupIterator)) != NULL) {
        circleIter = createIterator(tmpGroup->circles);
        
        while ((elem = nextElement(&circleIter)) != NULL) {
            if (elem) {
                insertBack(circleList, (void*)elem);
            }
        }
    }
    
    return circleList;
}

/*Function that returns a list of all paths in the struct*/
List* getPaths(const SVG* img) {
    List* pathList = NULL;
    List* groupList;

    if (img == NULL) {
        return pathList;
    }

    ListIterator circleIter;
    ListIterator groupIterator;
    Group *tmpGroup;
    void *elem;

    pathList = initializeList(&pathToString, &deletePath, &comparePaths);
    circleIter = createIterator(img->paths);
    groupList = getGroups(img);
    groupIterator = createIterator(groupList);

    while ((elem = nextElement(&circleIter)) != NULL) {
        if (elem) {
            insertBack(pathList, (void*)elem);
        }
    }

    while ((tmpGroup = (Group*)nextElement(&groupIterator)) != NULL) {
        circleIter = createIterator(tmpGroup->paths);
        
        while ((elem = nextElement(&circleIter)) != NULL) {
            if (elem) {
                insertBack(pathList, (void*)elem);
            }
        }
    }
    
    return pathList;
}

/*Function that returns a list of all groups in the struct*/
List* getGroups(const SVG* img) {
    List* list = NULL;
    Group *tmpGroup;

    if (img == NULL) {
        return list;
    }

    void* elem;
    list = initializeList(&groupToString, &deleteGroup, &compareGroups);
    ListIterator groupIter = createIterator(img->groups);
    
    while ((elem = nextElement(&groupIter)) != NULL) {
        if (elem != NULL) {
            tmpGroup = (Group*)elem;
            insertBack(list, (void*)tmpGroup);
        } 
        parseGroupWithinGroup(list, (Group*)elem);
    }

    return list;
}

/***************************** SVG Summary Functions ********************************/

int numRectsWithArea(const SVG* img, float area) {
    /*If NULL, return NULL*/
    if (img == NULL || area < 0) {
        return 0;
    }
    /*Declare variables*/
    List* rects;
    ListIterator rectIter;
    void* elem;
    int count = 0;

    rects = getRects(img);
    rectIter = createIterator(rects);
    /*Iterate through the list and counter++ when area is equal to the rectangle area*/
    while ((elem = nextElement(&rectIter)) != NULL) {
        float rectArea = ((Rectangle*)elem)->height * ((Rectangle*)elem)->width;
        if (ceil(rectArea) == ceil(area)) {
            count++;
        }
    }

    /*Free and return the length*/
    free(rects);
    return count;
}

int numCirclesWithArea(const SVG* img, float area) {
    /*If NULL, return NULL*/
    if (img == NULL || area < 0) {
        return 0;
    }
    /*Declare variables*/
    int count = 0;
    void *elem;
    List *circleList = getCircles(img);
    ListIterator iterator = createIterator(circleList);

    /*Iterate through the list and counter++ when area is equal to the circle area*/
    while ((elem = nextElement(&iterator))) {
        float circleArea = ((Circle*)elem)->r * ((Circle*)elem)->r * PI;
        
        if (ceil(area) == ceil(circleArea)) {
            count++;
        }
    }

    /*Free and return the length*/
    free(circleList);
    return count;
}

int numPathsWithdata(const SVG* img, const char* data) {
    /*If NULL, return NULL*/
    if (img == NULL || strlen(data) < 0) {
        return 0;
    }
    /*Declare variables*/
    int count = 0;
    void *pathElement;
    List *pathList = getPaths(img);
    ListIterator iterator = createIterator(pathList);
    /*Iterate through the list and counter++ when data is equal to the path data*/
    while ((pathElement = nextElement(&iterator))) {
        if (strcmp(((Path*)pathElement)->data, data) == 0) {
            count++;
        }
    }

    /*Free and return the length*/
    free(pathList);
    return count;
}

int numGroupsWithLen(const SVG* img, int len) {
    /*If NULL, return NULL*/
    if (img == NULL || len < 0) {
        return 0;
    }
    /*Declare variables*/
    int counter = 0;
    int numOfGroups;
    void *elem;
    List *groupList = getGroups(img);
    ListIterator iterator = createIterator(groupList);
    Group *groupObj;

    while ((elem = nextElement(&iterator))) {
        groupObj = (Group*)elem;
        numOfGroups = 0;
        /*Get current length of the objects present in group object*/
        numOfGroups += groupObj->circles->length;
        numOfGroups += groupObj->rectangles->length;
        numOfGroups += groupObj->paths->length;
        numOfGroups += groupObj->groups->length;
        /*Iterate through the list and counter++ when length is equal to the group length*/
        if (numOfGroups == len) {
            counter++;
        }
    }

    /*Free and return the length*/
    free(groupList);
    return counter;
}

int numAttr(const SVG* img) {
    int counter = 0;

    if (img == NULL) {
        return counter;
    }

    ListIterator listIter;
    void *elem;

    List* rectList = getRects(img);
    List* circleList = getCircles(img);
    List* pathList = getPaths(img);
    List* groupList = getGroups(img);
    /*Starts off with the other attributes of the SVG file-*/
    counter = getLength(img->otherAttributes);

    /*Calculates and adds all individual objects other attributes*/
    /*For object Rectangle*/
    listIter = createIterator(rectList);
    while ((elem = nextElement(&listIter)) != NULL) {
        counter += getLength(((Rectangle*)elem)->otherAttributes);
    }
    /*For object Circle*/
    listIter = createIterator(circleList);
    while ((elem = nextElement(&listIter)) != NULL) {
        counter += getLength(((Circle*)elem)->otherAttributes);
    }
    /*For object Path*/
    listIter = createIterator(pathList);
    while ((elem = nextElement(&listIter)) != NULL) {
        counter += getLength(((Path*)elem)->otherAttributes);
    }
    /*For object Group*/
    listIter = createIterator(groupList);
    while ((elem = nextElement(&listIter)) != NULL) {
        counter += getLength(((Group*)elem)->otherAttributes);
    }

    /*Free the lists*/
    free(pathList);
    free(circleList);
    free(rectList);
    free(groupList);

    return counter;
}

/********************************* A2 Functions *************************************/

/**
 * @brief Function to setting an attribute in an SVG or component
 * @param img 
 * @param elemType 
 * @param elemIndex 
 * @param newAttribute 
 * @return true 
 * @return false 
 */
bool setAttribute(SVG* img, elementType elemType, int elemIndex, Attribute* newAttribute) {
    /*Check for any NULL values*/
    if (img == NULL || newAttribute == NULL || elemIndex < -1) {
        return false;
    }
    if (newAttribute->name == NULL || newAttribute->value == NULL) {
        return false;
    }
    
    /*Check if elemType has proper enum value*/
    if (elemType != RECT && elemType != CIRC && elemType != PATH && elemType != GROUP && elemType != SVG_IMG) {
        return false;
    }
    
    /*Sets/Updates attributes based on RECT*/
    if (elemType == SVG_IMG || elemType == 0) {
        // if(strcmp(newAttribute->name,"title") == 0){
        //     strncpy(img->title, newAttribute->value, 255);
        //     (img->title)[255] = '\0';
        // } else if (strcmp(newAttribute->name,"desc") == 0){
        //     strncpy(img->title, newAttribute->value, 255);
        //     (img->description)[255] = '\0';
        // } else {
            /*If the attribute with the specified name exists in list, update the value*/
            ListIterator iter = createIterator(img->otherAttributes);
            void* elem;
        
            while ((elem = nextElement(&iter)) != NULL) {
                Attribute* otherAttr = (Attribute*)elem;

                if (strcmp(newAttribute->name, otherAttr->name) == 0) {
                    char tmpStr[1000];
                    sprintf(tmpStr, newAttribute->value);
                    //free(otherAttr->value);
                    strcpy(otherAttr->value, tmpStr);

                    if (newAttribute == NULL) {
                        deleteAttribute(newAttribute);
                    }
                    
                    return true;
                }
            }
            /*If the attribute with the specified name does not exist in list, append to the list*/
            insertBack(img->otherAttributes, (void*)newAttribute);
        //}

        if (newAttribute == NULL) {
            deleteAttribute(newAttribute);
        }
        /*Successful, return true*/
        return true;
    } else if (elemType == CIRC) {
        if (elemIndex > getLength(img->circles) - 1) {
            return false;
        }

        ListIterator iter = createIterator(img->circles);
        Circle* circ;
        void *elem;

        int i;
        for (i = 0; (elem = nextElement(&iter)) != NULL; i++) {
            if (i == elemIndex) {
                circ = (Circle*)elem;
                //printf("Before: \t%s\n", circleToString(circ));
                if (strcmp(newAttribute->name, "cx") == 0) {
                    /*Change cx value based on attribute*/
                    circ->cx = atof(newAttribute->value);
                } else if (strcmp(newAttribute->name, "cy") == 0) {
                    /*Change cy value based on attribute*/
                    circ->cy = atof(newAttribute->value);
                } else if (strcmp(newAttribute->name, "r") == 0) {
                    /*Change r value based on attribute*/
                    circ->r = atof(newAttribute->value);
                } else {
                    /*Update or append otherAttributes of Circle*/
                    setOtherAttribute(circ->otherAttributes, newAttribute);
                }
                //printf("After: \t\t%s\n\n", circleToString(circ));
            }
        }

        if (newAttribute == NULL) {
            deleteAttribute(newAttribute);
        }
        /*Successful, return true*/
        return true;
    /*Sets/Updates attributes based on PATH*/
    } else if (elemType == RECT) {
        if (elemIndex > getLength(img->rectangles) - 1) {
            return false;
        }

        ListIterator iter = createIterator(img->rectangles);
        Rectangle* rect;
        void *elem;

        int i;
        for (i = 0; (elem = nextElement(&iter)) != NULL; i++) {
            if (i == elemIndex) {
                //printf("Index: %d - Num of rectangles: %d\n", elemIndex, getLength(img->rectangles));
                //printf("%s\t%s\n", newAttribute->name, newAttribute->value);
                
                rect = (Rectangle*)elem;
                //printf("Before: \t%s\n", rectangleToString(rect));
                if (strcmp(newAttribute->name, "x") == 0) {
                    /*Change x value based on attribute*/
                    rect->x = atof(newAttribute->value);
                } else if (strcmp(newAttribute->name, "y") == 0) {
                    /*Change y value based on attribute*/
                    rect->y = atof(newAttribute->value);
                } else if (strcmp(newAttribute->name, "width") == 0) {
                    /*Change width value based on attribute*/
                    rect->width = atof(newAttribute->value);
                } else if (strcmp(newAttribute->name, "height") == 0) {
                    /*Change height value based on attribute*/
                    rect->height = atof(newAttribute->value);
                } else {
                    /*Update or append otherAttributes of Rectangle*/
                    setOtherAttribute(rect->otherAttributes, newAttribute);
                }
                //printf("After: \t\t%d = %s\n\n", i, rectangleToString(rect));
            }
        }

        if (newAttribute == NULL) {
            deleteAttribute(newAttribute);
        }
        /*Successful, return true*/
        return true;
    /*Sets/Updates attributes based on CIRC*/
    } else if (elemType == PATH) {
        if (elemIndex > getLength(img->paths) - 1) {
            return false;
        }

        ListIterator iter = createIterator(img->paths);
        Path* path;
        void *elem;

        int i;
        for (i = 0; (elem = nextElement(&iter)) != NULL; i++) {
            if (i == elemIndex) {
                path = (Path*)elem;
                //printf("Before: \t%s\n", pathToString(path));
                if (strcmp(newAttribute->name, "d") == 0) {
                    /*Change data value based on attribute*/
                    char tmpStr[1000];
                    sprintf(tmpStr, newAttribute->value);
                    strcpy(path->data, tmpStr);
                } else {
                    /*Update or append otherAttributes of path*/
                    setOtherAttribute(path->otherAttributes, newAttribute);
                }
                //printf("After: \t\t%s\n\n", pathToString(path));
            }
        }

        if (newAttribute == NULL) {
            deleteAttribute(newAttribute);
        }
        /*Successful, return true*/
        return true;
    /*Sets/Updates attributes based on GROUP*/
    } else if (elemType == GROUP) {
        if (elemIndex > getLength(img->groups) - 1) {
            return false;
        }

        ListIterator iter = createIterator(img->groups);
        Group* group;
        void *elem;

        int i;
        for (i = 0; (elem = nextElement(&iter)) != NULL; i++) {
            if (i == elemIndex) {
                group = (Group*)elem;
                //printf("Before: \t%s\n", groupToString(group));
                if (group->otherAttributes != NULL) {
                    /*Update or append otherAttributes of group*/
                    setOtherAttribute(group->otherAttributes, newAttribute);
                }
                //printf("After: \t\t%s\n\n", groupToString(group));
            }
        }

        if (newAttribute == NULL) {
            deleteAttribute(newAttribute);
        }
        /*Successful, return true*/
        return true;
    /*Sets/Updates attributes based on SVG_IMG*/
    } else {
        /*Otherwise, return false*/
        return false;
    }
}

/**
 * @brief Function to adding an element - Circle, Rectangle, or Path - to an SVG
 * @param img 
 * @param type 
 * @param newElement 
 */
void addComponent(SVG* img, elementType type, void* newElement) {
    /*Check for any NULL values*/
    if (img == NULL || newElement == NULL) {
        return ;
    } else {
    /*Otherwise will check what element type the Enum Value is*/
        if (type == RECT && img->rectangles != NULL) {
            insertBack(img->rectangles, newElement);
        } else if (type == CIRC && img->circles != NULL) {
            insertBack(img->circles, newElement);
        } else if (type == PATH && img->paths != NULL) {
            insertBack(img->paths, newElement);
        } else {
            return ;
        }
    }
}

char* attrToJSON(const Attribute *a) {
    char* jsonAttr = NULL;
    /*Check for object to see if its NULL*/
    if (a == NULL) {
        return NULL;
        jsonAttr = malloc(sizeof(char) * (strlen("{}") + 1));
        strcpy(jsonAttr, "{}");
        /*Returns empty string {}*/
        return jsonAttr;
    }

    char attrName[1000];
    char attrValue[1000];
    Attribute* attr = (Attribute*)a;

    /*Puts number of attributes in Attributes into string*/
    if (attr->name == NULL) {
        return jsonAttr;
    }

    /*Put values of attribute name and value into string*/
    sprintf(attrName,"%s", attr->name);
    sprintf(attrValue,"%s", attr->value);

    /*Malloc proper amount of memory for string, then catonate whole string in JSON format*/
    jsonAttr = malloc(sizeof(char) * (strlen("{\"name\":\"\",\"value\":\"\"}") + strlen(attrName) + strlen(attrValue) + 1));
    sprintf(jsonAttr, "{\"name\":\"%s\",\"value\":\"%s\"}", attrName, attrValue);

    return jsonAttr;
}

char* circleToJSON(const Circle *c) {
    char* jsonCirc = NULL;

    /*Check for object to see if its NULL*/
    if (c == NULL) {
        jsonCirc = malloc(sizeof(char) * (strlen("{}") + 1));
        strcpy(jsonCirc, "{}");
        /*Returns empty string {}*/
        return jsonCirc;
    }

    /*Otherwise will return newly allocated string in proper format*/
    char xVal[1000];
    char yVal[1000];
    char rVal[1000];
    char attVal[1000];
    char unitStr[1000];
    Circle* circle = (Circle*)c;

    /*Put values of x, y, and radius into string*/
    sprintf(xVal,"%.2f", circle->cx);
    sprintf(yVal,"%.2f", circle->cy);
    sprintf(rVal,"%.2f", circle->r);
    /*Puts units of circle into string*/
    if (strcmp(circle->units, "") == 0) {
        strcpy(unitStr, "");
    } else {
        sprintf(unitStr, "%s", circle->units);
    }
    /*Puts number of attributes in circle into string*/
    List* temp = (List*)circle->otherAttributes;
    if (circle->otherAttributes == NULL || getLength(temp) == 0) {
        strcpy(attVal, "0");
    } else {
        sprintf(attVal, "%d", getLength(circle->otherAttributes));
    }

    /*Malloc proper amount of memory for string, then catonate whole string in JSON format*/
    jsonCirc = malloc(sizeof(char) * (strlen("{\"cx\":,\"cy\":,\"r\":,\"numAttr\":,\"units\":\"\"}") + strlen(unitStr) + strlen(xVal) + strlen(yVal) + strlen(rVal) + strlen(attVal) + 1));
    sprintf(jsonCirc, "{\"cx\":%s,\"cy\":%s,\"r\":%s,\"numAttr\":%s,\"units\":\"%s\"}", xVal, yVal, rVal, attVal, unitStr);

    return jsonCirc;
}

char* rectToJSON(const Rectangle *r) {
    char* jsonRect = NULL;

    /*Check for object to see if its NULL*/
    if (r == NULL) {
        jsonRect = malloc(sizeof(char) * (strlen("{}") + 1));
        strcpy(jsonRect, "{}");
        /*Returns empty string {}*/
        return jsonRect;
    }

    /*Otherwise will return newly allocated string in proper format*/
    char xVal[1000];
    char yVal[1000];
    char wVal[1000];
    char hVal[1000];
    char attVal[1000];
    char unitStr[1000];
    Rectangle* rect = (Rectangle*)r;

    /*Put values of x, y, height, and width into string*/
    sprintf(xVal,"%.2f", rect->x);
    sprintf(yVal,"%.2f", rect->y);
    sprintf(wVal,"%.2f", rect->width);
    sprintf(hVal,"%.2f", rect->height);
    /*Puts units of Rectangle into string*/
    if (strcmp(rect->units, "") == 0) {
        strcpy(unitStr, "");
    } else {
        sprintf(unitStr, "%s", rect->units);
    }
    /*Puts number of attributes in Rectangle into string*/
    List* temp = (List*)rect->otherAttributes;
    if (rect->otherAttributes == NULL || getLength(temp) == 0) {
        strcpy(attVal, "0");
    } else {
        sprintf(attVal, "%d", getLength(rect->otherAttributes));
    }

    /*Malloc proper amount of memory for string, then catonate whole string in JSON format*/
    jsonRect = malloc(sizeof(char) * (strlen("{\"x\":,\"y\":,\"w\":,\"h\":,\"numAttr\":,\"units\":\"\"}") + strlen(unitStr) + strlen(xVal) + strlen(yVal) + strlen(wVal) + strlen(hVal) + strlen(attVal) + 1));
    sprintf(jsonRect, "{\"x\":%s,\"y\":%s,\"w\":%s,\"h\":%s,\"numAttr\":%s,\"units\":\"%s\"}", xVal, yVal, wVal, hVal, attVal, unitStr);

    return jsonRect;
}

char* pathToJSON(const Path *p) {
    char* jsonPath = NULL;

    /*Check for object to see if its NULL*/
    if (p == NULL) {
        jsonPath = malloc(sizeof(char) * (strlen("{}") + 1));
        strcpy(jsonPath, "{}");
        /*Returns empty string {}*/
        return jsonPath;
    }

    /*Otherwise will return newly allocated string in proper format*/
    Path* path = (Path*)p;
    int length = strlen(path->data);
    char dVal[length];
    char attVal[1000];

    /*Put value of data into string*/
    sprintf(dVal,"%s", path->data);
    /*Puts number of attributes in Path into string*/
    List* temp = (List*)path->otherAttributes;
    if (path->otherAttributes == NULL || getLength(temp) == 0) {
        strcpy(attVal, "0");
    } else {
        sprintf(attVal, "%d", getLength(path->otherAttributes));
    }

    /*Malloc proper amount of memory for string, then catonate whole string in JSON format*/
    jsonPath = malloc(sizeof(char) * (strlen("{\"d\":\"\",\"numAttr\":}") + strlen(dVal) + strlen(attVal) + 1));
    sprintf(jsonPath, "{\"d\":\"%s\",\"numAttr\":%s}", dVal, attVal);

    return jsonPath;
}

char* groupToJSON(const Group *g) {
    char* jsonGroup = NULL;

    /*Check for object to see if its NULL*/
    if (g == NULL) {
        jsonGroup = malloc(sizeof(char) * (strlen("{}") + 1));
        strcpy(jsonGroup, "{}");
        /*Returns empty string {}*/
        return jsonGroup;
    }

    /*Otherwise will return newly allocated string in proper format*/
    char cVal[1000];
    char attVal[1000];
    Group* group = (Group*)g;

    /*Put value of number of children into string*/
    int children = 0;
    children += getLength(group->rectangles);
    children += getLength(group->circles);
    children += getLength(group->paths);
    children += getLength(group->groups);

    sprintf(cVal,"%d", children);
    /*Puts number of attributes in Group into string*/
    List* temp = (List*)group->otherAttributes;
    if (group->otherAttributes == NULL || getLength(temp) == 0) {
        strcpy(attVal, "0");
    } else {
        sprintf(attVal, "%d", getLength(group->otherAttributes));
    }

    /*Malloc proper amount of memory for string, then catonate whole string in JSON format*/
    jsonGroup = malloc(sizeof(char) * (strlen("{\"children\":,\"numAttr\":}") + strlen(cVal) + strlen(attVal) + 1));
    sprintf(jsonGroup, "{\"children\":%s,\"numAttr\":%s}", cVal, attVal);

    return jsonGroup;
}

char* SVGtoJSON(const SVG* img) {
    char* jsonSVG = NULL;

    /*Check for object to see if its NULL*/
    if (img == NULL) {
        jsonSVG = malloc(sizeof(char) * (strlen("{}") + 1));
        strcpy(jsonSVG, "{}");
        /*Returns empty string {}*/
        return jsonSVG;
    }

    /*Otherwise will return newly allocated string in proper format*/
    char numR[1000];
    char numC[1000];
    char numP[1000];
    char numG[1000];

    /*Get length of rectangles*/
    if (img->rectangles == NULL) {
        strcpy(numR, "0");
    } else {
        sprintf(numR, "%d", getLength(getRects(img)));
    }
    /*Get length of circles*/
    if (img->circles == NULL) {
        strcpy(numC, "0");
    } else {
        sprintf(numC, "%d", getLength(getCircles(img)));
    }
    /*Get length of paths*/
    if (img->paths == NULL) {
        strcpy(numP, "0");
    } else {
        sprintf(numP, "%d", getLength(getPaths(img)));
    }
    /*Get length of groups*/
    if (img->groups == NULL) {
        strcpy(numG, "0");
    } else {
        sprintf(numG, "%d", getLength(getGroups(img)));
    }

    /*Malloc proper amount of memory for string, then catonate whole string in JSON format*/
    jsonSVG = malloc(sizeof(char) * strlen("{\"numRect\":,\"numCirc\":,\"numPaths\":,\"numGroups\":}") + strlen(numR) + strlen(numC) + strlen(numP) + strlen(numG) + 1);
    sprintf(jsonSVG, "{\"numRect\":%s,\"numCirc\":%s,\"numPaths\":%s,\"numGroups\":%s}", numR, numC, numP, numG);

    return jsonSVG;
}

char* attrListToJSON(const List *list) {
    char* jsonAttr = NULL;
    char* attrString = NULL;
    /*If list is empty*/
    List *temp = (List*)list;
    if (list == NULL || getLength(temp) <= 0) {
        jsonAttr = malloc(sizeof(char) * (strlen("[]") + 1));
        strcpy(jsonAttr, "[]");
        /*Returns empty string []*/
        return jsonAttr;
    }
    /*If list is not empty, start string with "["*/
    jsonAttr = malloc(sizeof(char) * (strlen("[") + 1));
    strcpy(jsonAttr,"[");

    void* elem;
    List *tempList = (List*)list;
    ListIterator iter = createIterator(tempList);
    /*Go through the elements of Attributes List*/
    while ((elem = nextElement(&iter)) != NULL) {
        /*Using the string returned from attrToJSON() function...*/
        Attribute* tmpAttr = (Attribute*)elem;
        attrString = attrToJSON(tmpAttr);
        jsonAttr = realloc(jsonAttr, sizeof(char) * (strlen(jsonAttr) + strlen(attrString) + 2));
        /*concatenate onto main jsonAttr string*/
        strcat(jsonAttr, attrString);
        strcat(jsonAttr, ",");
        /*Free the string*/
        free(attrString);
    }
    /*End string with "]"*/
    int length = strlen(jsonAttr);
    jsonAttr[length - 1] = ']';
    /*Return string*/
    return jsonAttr;
}

char* circListToJSON(const List *list) {
    char* jsonCirc = NULL;
    char* circString = NULL;
    /*If list is empty*/
    List *temp = (List*)list;
    if (list == NULL || getLength(temp) <= 0) {
        jsonCirc = malloc(sizeof(char) * (strlen("[]") + 1));
        strcpy(jsonCirc, "[]");
        /*Returns empty string []*/
        return jsonCirc;
    }
    /*If list is not empty, start string with "["*/
    jsonCirc = malloc(sizeof(char) * (strlen("[") + 1));
    strcpy(jsonCirc,"[");

    void* elem;
    List *tempList = (List*)list;
    ListIterator iter = createIterator(tempList);
    /*Go through the elements of Circles List*/
    while ((elem = nextElement(&iter)) != NULL) {
        /*Using the string returned from circleToJSON() function...*/
        Circle* tmpCirc = (Circle*)elem;
        circString = circleToJSON(tmpCirc);
        jsonCirc = realloc(jsonCirc, sizeof(char) * (strlen(jsonCirc) + strlen(circString) + 2));
        /*concatenate onto main jsonCirc string*/
        strcat(jsonCirc, circString);
        strcat(jsonCirc, ",");
        /*Free the string*/
        free(circString);
    }
    /*End string with "]"*/
    int length = strlen(jsonCirc);
    jsonCirc[length - 1] = ']';
    /*Return string*/
    return jsonCirc;
}

char* rectListToJSON(const List *list) {
    char* jsonRect = NULL;
    char* rectString = NULL;
    /*If list is empty*/
    List *temp = (List*)list;
    if (list == NULL || getLength(temp) <= 0) {
        jsonRect = malloc(sizeof(char) * (strlen("[]") + 1));
        strcpy(jsonRect, "[]");
        /*Returns empty string []*/
        return jsonRect;
    }
    /*If list is not empty, start string with "["*/
    jsonRect = malloc(sizeof(char) * (strlen("[") + 1));
    strcpy(jsonRect,"[");

    void* elem;
    List *tempList = (List*)list;
    ListIterator iter = createIterator(tempList);
    /*Go through the elements of Rectangle List*/
    while ((elem = nextElement(&iter)) != NULL) {
        /*Using the string returned from rectToJSON() function...*/
        Rectangle* tmpRect = (Rectangle*)elem;
        rectString = rectToJSON(tmpRect);
        jsonRect = realloc(jsonRect, sizeof(char) * (strlen(jsonRect) + strlen(rectString) + 2));
        /*concatenate onto main jsonRect string*/
        strcat(jsonRect, rectString);
        strcat(jsonRect, ",");
        /*Free the string*/
        free(rectString);
    }
    /*End string with "]"*/
    int length = strlen(jsonRect);
    jsonRect[length - 1] = ']';
    /*Return string*/
    return jsonRect;
}

char* pathListToJSON(const List *list) {
    char* jsonPath = NULL;
    char* pathString = NULL;
    /*If list is empty*/
    List *temp = (List*)list;
    if (list == NULL || getLength(temp) <= 0) {
        jsonPath = malloc(sizeof(char) * (strlen("[]") + 1));
        strcpy(jsonPath, "[]");
        /*Returns empty string []*/
        return jsonPath;
    }
    /*If list is not empty, start string with "["*/
    jsonPath = malloc(sizeof(char) * (strlen("[") + 1));
    strcpy(jsonPath,"[");

    void* elem;
    List *tempList = (List*)list;
    ListIterator iter = createIterator(tempList);
    /*Go through the elements of Path List*/
    while ((elem = nextElement(&iter)) != NULL) {
        /*Using the string returned from pathToJSON() function...*/
        Path* tmpPath = (Path*)elem;
        pathString = pathToJSON(tmpPath);
        jsonPath = realloc(jsonPath, sizeof(char) * (strlen(jsonPath) + strlen(pathString) + 2));
        /*concatenate onto main jsonPath string*/
        strcat(jsonPath, pathString);
        strcat(jsonPath, ",");
        /*Free the string*/
        free(pathString);
    }
    /*End string with "]"*/
    int length = strlen(jsonPath);
    jsonPath[length - 1] = ']';
    /*Return string*/
    return jsonPath;
}

char* groupListToJSON(const List *list) {
    char* jsonGroup = NULL;
    char* groupString = NULL;
    /*If list is empty*/
    List *temp = (List*)list;
    if (list == NULL || getLength(temp) <= 0) {
        jsonGroup = malloc(sizeof(char) * (strlen("[]") + 1));
        strcpy(jsonGroup, "[]");
        /*Returns empty string []*/
        return jsonGroup;
    }
    /*If list is not empty, start string with "["*/
    jsonGroup = malloc(sizeof(char) * (strlen("[") + 1));
    strcpy(jsonGroup,"[");

    void* elem;
    List *tempList = (List*)list;
    ListIterator iter = createIterator(tempList);
    /*Go through the elements of Group List*/
    while ((elem = nextElement(&iter)) != NULL) {
        /*Using the string returned from groupToJSON() function...*/
        Group* tmpGroup = (Group*)elem;
        groupString = groupToJSON(tmpGroup);
        jsonGroup = realloc(jsonGroup, sizeof(char) * (strlen(jsonGroup) + strlen(groupString) + 2));
        /*concatenate onto main jsonGroup string*/
        strcat(jsonGroup, groupString);
        strcat(jsonGroup, ",");
        /*Free the string*/
        free(groupString);
    }
    /*End string with "]"*/
    int length = strlen(jsonGroup);
    jsonGroup[length - 1] = ']';
    /*Return string*/
    return jsonGroup;
}

/************************* Bonus A2 functions *************************/

SVG* JSONtoSVG(const char* svgString) {
    SVG *tmpSVG = NULL;

    if (svgString == NULL  || strcmp(svgString, "") == 0) {
        return tmpSVG;
    } else {
        return tmpSVG;
    }
}

Rectangle* JSONtoRect(const char* svgString) {
    Rectangle *tmpRect = NULL;

    if (svgString == NULL  || strcmp(svgString, "") == 0) {
        return tmpRect;
    } else {
        return tmpRect;
    }
}

Circle* JSONtoCircle(const char* svgString) {
    Circle *tmpCirc = NULL;

    if (svgString == NULL  || strcmp(svgString, "") == 0) {
        return tmpCirc;
    } else {
        return tmpCirc;
    }
}

/**************************** Validate SVG File Functions ***************************/

/**
 * @brief This function takes a SVG struct and saves it to a file in SVG Format
 * Most code used from the documentation references that Prof. Denis has provided
 * in the Modules
 * @param img 
 * @param fileName 
 * @return true 
 * @return false 
 */
bool writeSVG(const SVG* img, const char* fileName) {
    /*If SVG object or fileName is NULL, return false*/
    if (img == NULL || fileName == NULL) {
        return false;
    }

    xmlDocPtr doc = NULL;
    xmlNodePtr root_node = NULL;

    LIBXML_TEST_VERSION;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    if (doc == NULL) {
        return false;
    }

    /*Sets the namespace in the XML tree*/
    if (strcmp(img->namespace, "") == 0 || (img->namespace) == NULL) {
        return false;
    } else {
        xmlNsPtr ns = xmlNewNs(root_node, BAD_CAST img->namespace, NULL);
        xmlSetNs(root_node, ns);
    }

    /*Sets the title of SVG image in the XML tree*/
    if (strcmp(img->title, "") != 0) {
        xmlNewChild(root_node, NULL, BAD_CAST "title", BAD_CAST img->title);
    }

    /*Sets the description of SVG image in the XML tree*/
    if (strcmp(img->description, "") != 0) {
        xmlNewChild(root_node, NULL, BAD_CAST "desc", BAD_CAST img->description);
    }

    rectToNode(root_node, img->rectangles);
    circleToNode(root_node, img->circles);
    pathToNode(root_node, img->paths);
    groupToNode(root_node, img->groups);

    ListIterator attrIter;
    void* elem;
    attrIter = createIterator(img->otherAttributes);

    /*Adding other attributes to the SVG Object node*/
    while((elem = nextElement(&attrIter)) != NULL){
        Attribute* tmpAttr = (Attribute*)elem;

        if ((tmpAttr->name) == NULL && (tmpAttr->value) == NULL) {
            continue;
        }

        xmlNewProp(root_node, BAD_CAST tmpAttr->name, BAD_CAST tmpAttr->value);
    }

    if (doc != NULL) {
        /*Dumping document to file*/
        xmlSaveFormatFileEnc(fileName, doc, "UTF-8", 1);
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        /*Return true*/
        return true;
    } else {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        /*Return false*/
        return false;
    }
}

/**
 * @brief Function to validating an existing an SVG struct against a SVG schema file
 * and make sure all content within is valid. Return true if valid, false otherwise.
 * @param img 
 * @param schemaFile 
 * @return true 
 * @return false 
 */
bool validateSVG(const SVG* img, const char* schemaFile) {
    /*Any arguments NULL, must return NULL*/
    if (img == NULL || schemaFile == NULL) {
        return false;
    }
    /*Namespace may not be null or empty*/
    if (strcmp(img->namespace, "") == 0 || img->namespace == NULL) {
        return false;
    }
    /*Lists cannot be NULL*/
    if (img->circles == NULL || img->rectangles == NULL || img->paths == NULL || img->groups == NULL || img->otherAttributes == NULL) {
        return false;
    }

    /*Checks if SVG violates any of the constraints specified in SVGParser.h*/
    if (!validateRect(img->rectangles) || !validateCirc(img->circles) || !validatePath(img->paths) || !validateGroup(img->groups)) {
        return false;
    }

    ListIterator attrIter;
    void* elem;
    attrIter = createIterator(img->otherAttributes);

    while((elem = nextElement(&attrIter)) != NULL){
        Attribute* tmpAttr = (Attribute*)elem;

        if (tmpAttr->name == NULL) {
            return false;
        }
    }

    /*SVG contents must represent a valid SVG struct once converted to XMl.*/
    xmlDoc* doc = svgToXML(img);
    if (doc == NULL) {
        return false;
    }

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    char buf[20000];
    setbuf(stderr, buf);

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(schemaFile);

    //xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) sprintf, (xmlSchemaValidityWarningFunc) sprintf, buf);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    
    if (doc == NULL) {
        if(schema != NULL) {
            xmlSchemaFree(schema);
        }

        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();

        return false;
    } else {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;

        ctxt = xmlSchemaNewValidCtxt(schema);
        //xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) sprintf, (xmlSchemaValidityWarningFunc) sprintf, buf);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        
        if (ret == 0) {
            if(schema != NULL) {
                xmlSchemaFree(schema);
            }

            xmlSchemaCleanupTypes();
            xmlCleanupParser();
            xmlMemoryDump();

            xmlSchemaFreeValidCtxt(ctxt);
            xmlFreeDoc(doc);
            
            return true;
        } else {
            if(schema != NULL) {
                xmlSchemaFree(schema);
            }

            xmlSchemaCleanupTypes();
            xmlCleanupParser();
            xmlMemoryDump();

            xmlSchemaFreeValidCtxt(ctxt);
            xmlFreeDoc(doc);
            
            return false;
        }
    }
}

/**
 * @brief Create a Valid SVG object based on the contents of an SVG file.
 * This function must validate the XML tree generated by libxml against a SVG schema 
 * file before attempting to traverse the tree and create an SVG struct.
 * Most code used from the documentation references that Prof. Denis has provided
 * in the Modules
 * @param fileName 
 * @param schemaFile 
 * @return SVG* validSVGObject
 */
SVG* createValidSVG(const char* fileName, const char* schemaFile) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    char buf[20000];
    setbuf(stderr, buf);
    
    LIBXML_TEST_VERSION

    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    const char *xsdFileName = schemaFile;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(xsdFileName);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) sprintf, (xmlSchemaValidityWarningFunc) sprintf, buf);
    //xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, NULL);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        if (schema != NULL) {
            xmlSchemaFree(schema);
        }

        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();

        return NULL;
    } else {
        xmlSchemaValidCtxtPtr ctxt;
        int ret;
        
        ctxt = xmlSchemaNewValidCtxt(schema);
        //xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, NULL);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) sprintf, (xmlSchemaValidityWarningFunc) sprintf, buf);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        
        /*File is not valid, return NULL*/
        if (ret != 0) {
            if(schema != NULL) {
                xmlSchemaFree(schema);
            }

            xmlSchemaCleanupTypes();
            xmlCleanupParser();
            xmlMemoryDump();

            xmlSchemaFreeValidCtxt(ctxt);
            xmlFreeDoc(doc);

            return NULL;
        }
    }

    /********* Continues to create regular SVG Object - createSVG() *********/

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    if (doc == NULL) {
        /*free the document */
        xmlFreeDoc(doc);

        /*
        *Free the global variables that may
        *have been allocated by the parser.
        */
        xmlCleanupParser();
        /*Error: could not parse file*/
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    /*Main SVG Struct Object that will be returned*/
    SVG *SVGObject = malloc(sizeof(SVG));

    /*Assign namespace, title, and desc with default values - In case they are empty*/
    strcpy(SVGObject->namespace, "empty-namespace");
    strcpy(SVGObject->title, "");
    strcpy(SVGObject->description, "");

    /*Initializing Lists for the main SVG Object file*/
    SVGObject->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    SVGObject->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    SVGObject->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    SVGObject->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    SVGObject->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    /*Receiving the other attributes for the SVG object*/
    xmlAttr *attr;
    for (attr = root_element->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        char tmpStr[1000];

        /*Adding handling of multiple attributes for an SVG component*/
        Attribute *svgAttributes = malloc(sizeof(Attribute));

        /*For attribute name*/
        svgAttributes->name = malloc(sizeof(char) * strlen(attrName) + 1);
        strcpy(svgAttributes->name, attrName);
        /*For attribute value*/
        sprintf(tmpStr, cont);
        strcpy(svgAttributes->value, tmpStr);
        //svgAttributes->value = malloc(sizeof(char) * strlen(cont) + 1);

        /*Insert into main SVG Object list*/
        insertBack(SVGObject->otherAttributes, (void*)svgAttributes);
    }

    /*Applying namespace onto the SVG Object*/
    xmlNs *nameSpace = root_element->ns;
    if (nameSpace == NULL) {
        return NULL;
    } else {
        char *nameSpaceChar = (char*)(nameSpace->href);
        if (nameSpaceChar != NULL) {
            strcpy(SVGObject->namespace, nameSpaceChar);
        } else{
            return NULL;
        }
    }

    /*Loops through the SVG file to find: Rect, Circles, Paths & Groups*/
    xmlNode *cur_node = NULL;
    for (cur_node = root_element->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            char *nodeName = (char *)cur_node->name;

            if (strcmp(nodeName, "title") == 0) {
                if (cur_node->children->content != NULL) {
                    strcpy(SVGObject->title, (char*)cur_node->children->content);
                }
            } else if (strcmp(nodeName, "desc") == 0) {
                if (cur_node->children->content != NULL) {
                    strcpy(SVGObject->description, (char*)cur_node->children->content);
                }
            } else if (strcmp(nodeName, "rect") == 0) {
                Rectangle *rect = parseRectData(cur_node);
                /*Rectangle object must not be NULL. It may be empty*/
                if (rect == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                /*Insert into main list*/
                insertBack(SVGObject->rectangles, (void*)rect);
            } else if (strcmp(nodeName, "circle") == 0) {
                Circle *circle = parseCircleData(cur_node);
                /*Circle object must not be NULL. It may be empty*/
                if (circle == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                /*Insert into main list*/
                insertBack(SVGObject->circles, (void*)circle);
            } else if (strcmp(nodeName, "path") == 0) {
                Path *path = parsePathData(cur_node);
                /*Path object must not be NULL. It may be empty*/
                if (path == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                
                insertBack(SVGObject->paths, (void*)path);
            } else if (strcmp(nodeName, "g") == 0) {
                Group *group = parseGroupData(cur_node);
                /*Group object must not be NULL. It may be empty*/
                if (group == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                /*Insert into main list*/
                insertBack(SVGObject->groups, (void*)group);
            }
        }
    }

    /*Free the document and the global variables*/
    xmlFreeDoc(doc);

    /*Free the resource*/
    if (schema != NULL) {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    /*Return object*/
    return SVGObject;
}

/**********************************  A1 Functions  **********************************/

/**
 * @brief Function to delete struct content and free all the memory.
 * @param img 
 */
void deleteSVG(SVG* img) {
    if (img == NULL) {
        return ;
    }

    freeList(img->rectangles);
    freeList(img->circles);
    freeList(img->paths);
    freeList(img->groups);
    //freeList(img->otherAttributes);
    free(img);
}

/**
 * @brief Function that returns a readable char for the SVG File
 * Most of it was done through the main function
 * @param img 
 * @return char* 
 */
char* SVGToString(const SVG* img) {
    char *temp;
    const SVG *tempSVG;

    if (img == NULL) {
        return NULL;
    }

    tempSVG = img;

    int length = strlen(tempSVG->namespace) + strlen(tempSVG->title) + strlen(tempSVG->description) + 25;
    temp = (char *) malloc(sizeof(char) * length);

    sprintf(temp,"Namespace: %s Title: %s Desc: %s", tempSVG->namespace, tempSVG->title, tempSVG->description);

    return temp;
}

/******************************* A1 CreateSVG Function ******************************/

/**
 * @brief Function to create an SVG struct based on the contents of an SVG file.
 * SVG Parser Function
 * Includes code from provided libXmlExample.c file - Prof. Denis
 * @param filename 
 * @return SVG* SVGObject
 */
SVG* createSVG(const char* filename) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    
    /*
     * This initializes the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(filename, NULL, 0);

    if (doc == NULL) {
        /*free the document */
        xmlFreeDoc(doc);

        /*
        *Free the global variables that may
        *have been allocated by the parser.
        */
        xmlCleanupParser();
        /*Error: could not parse file*/
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    /*Main SVG Struct Object that will be returned*/
    SVG *SVGObject = malloc(sizeof(SVG));

    /*Assign namespace, title, and desc with default values - In case they are empty*/
    strcpy(SVGObject->namespace, "empty-namespace");
    strcpy(SVGObject->title, "");
    strcpy(SVGObject->description, "");

    /*Initializing Lists for the main SVG Object file*/
    SVGObject->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    SVGObject->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    SVGObject->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    SVGObject->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    SVGObject->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    /*Receiving the other attributes for the SVG object*/
    xmlAttr *attr;
    for (attr = root_element->properties; attr != NULL; attr = attr->next) {
        xmlNode *value = attr->children;
        char *attrName = (char *)attr->name;
        char *cont = (char *)(value->content);
        char tmpStr[256];

        /*Adding handling of multiple attributes for an SVG component*/
        Attribute *svgAttributes = malloc(sizeof(Attribute));

        /*For attribute name*/
        svgAttributes->name = malloc(sizeof(char) * strlen(attrName) + 1);
        strcpy(svgAttributes->name, attrName);
        /*For attribute value*/
        sprintf(tmpStr, cont);
        strcpy(svgAttributes->value, tmpStr);
        //svgAttributes->value = malloc(sizeof(char) * strlen(cont) + 1);

        /*Insert into main SVG Object list*/
        insertBack(SVGObject->otherAttributes, (void*)svgAttributes);
    }

    /*Applying namespace onto the SVG Object*/
    xmlNs *nameSpace = root_element->ns;
    if (nameSpace == NULL) {
        return NULL;
    } else {
        char *nameSpaceChar = (char*)(nameSpace->href);
        if (nameSpaceChar != NULL) {
            strcpy(SVGObject->namespace, nameSpaceChar);
        } else{
            return NULL;
        }
    }

    /*Loops through the SVG file to find: Rect, Circles, Paths & Groups*/
    xmlNode *cur_node = NULL;
    for (cur_node = root_element->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            char *nodeName = (char *)cur_node->name;

            if (strcmp(nodeName, "title") == 0) {
                if (cur_node->children->content != NULL) {
                    strcpy(SVGObject->title, (char*)cur_node->children->content);
                }
            } else if (strcmp(nodeName, "desc") == 0) {
                if (cur_node->children->content != NULL) {
                    strcpy(SVGObject->description, (char*)cur_node->children->content);
                }
            } else if (strcmp(nodeName, "rect") == 0) {
                Rectangle *rect = parseRectData(cur_node);
                /*Rectangle object must not be NULL. It may be empty*/
                if (rect == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                /*Insert into main list*/
                insertBack(SVGObject->rectangles, (void*)rect);
            } else if (strcmp(nodeName, "circle") == 0) {
                Circle *circle = parseCircleData(cur_node);
                /*Circle object must not be NULL. It may be empty*/
                if (circle == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                /*Insert into main list*/
                insertBack(SVGObject->circles, (void*)circle);
            } else if (strcmp(nodeName, "path") == 0) {
                Path *path = parsePathData(cur_node);
                /*Path object must not be NULL. It may be empty*/
                if (path == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                
                insertBack(SVGObject->paths, (void*)path);
            } else if (strcmp(nodeName, "g") == 0) {
                Group *group = parseGroupData(cur_node);
                /*Group object must not be NULL. It may be empty*/
                if (group == NULL) {
                    deleteSVG(SVGObject);
                    xmlFreeDoc(doc);
                    xmlCleanupParser();
                    return NULL;
                }
                /*Insert into main list*/
                insertBack(SVGObject->groups, (void*)group);
            }
        }
    }  

    /*Free the document and the global variables*/
    xmlFreeDoc(doc);
    xmlCleanupParser();
    /*Return object*/
    return SVGObject;
}
