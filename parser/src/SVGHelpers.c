/**
 * @file SVGParser.c
 * @author Abdullah Khan (mkhan94@uoguelph.ca) - 1101209
 * @brief This file contains all the functions that are needed for helping with the
 * parsing
 * @date 2022-01-20
 * @copyright Copyright (c) 2022
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGParser.h"
#include "LinkedListAPI.h"

/********************************* A1 Functions *************************************/

/**
 * @brief Parses all data that is required for Path object 
 * @param tmp_Node 
 * @return Path* 
 */
Path *parsePathData(xmlNode *tmp_Node) {
    if (tmp_Node == NULL) {
        return NULL;
    }
    
    Path *path;
    xmlAttr *attr;
    int length;
    
    for (attr = tmp_Node->properties; attr != NULL; attr = attr->next) {
        char *attrName = (char *)attr->name;
        xmlNode *value = attr->children;
        char *cont = (char *)(value->content);

        /*Does if-statements based on attributes*/
        if (strcmp(attrName, "d") == 0) {
            if(cont != NULL) {
                length = strlen((char*)cont);
                char tmpStr2[length];
                path = malloc(sizeof(Path) + sizeof(char) * (length + 1));
                
                sprintf(tmpStr2, cont);
                strcpy(path->data, tmpStr2);
            }
        }
    }
    
    path->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    /*If anything else, puts into other attributes*/
    attr = NULL;
    for (attr = tmp_Node->properties; attr != NULL; attr = attr->next) {
        char *attrName = (char *)attr->name;
        xmlNode *value = attr->children;
        char *cont = (char *)(value->content);

        if (strcmp(attrName, "d") != 0) {
            if (path == NULL) {
                return NULL;
            }
            
            /*Goes to this section if 'path' has any other attributes*/
            char tmpStr[256];
            int memLength;
            Attribute *pathOtherAttr = malloc(sizeof(Attribute));

            /*For attribute name*/
            sprintf(tmpStr, attrName);
            memLength = strlen(tmpStr) + 2;
            pathOtherAttr->name = (char*)malloc(sizeof(char) * memLength);
            
            strcpy(pathOtherAttr->name, (char*)tmpStr);
            /*For attribute value*/
            sprintf(tmpStr, cont);
            strcpy(pathOtherAttr->value, tmpStr);
            //memLength = strlen(tmpStr) + 2;
            //pathOtherAttr->value = (char *)malloc(sizeof(char) * memLength);
            /*Prints out other attributes*/
            //printf("Name: %s Value: %s\n", pathOtherAttr->name, pathOtherAttr->value);
            
            insertBack(path->otherAttributes, (void*)pathOtherAttr);
        }
    }

    return path;
}

/**
 * @brief Parses all data that is required for Circle object 
 * @param tmp_Node 
 * @return Circle* 
 */
Circle *parseCircleData(xmlNode *tmp_Node) {
    /*If NULL, return NULL*/
    if (tmp_Node == NULL) {
        return NULL;
    }

    Circle *circle = malloc(sizeof(Circle));

    circle->cx = 0;
    circle->cy = 0;
    circle->r = 0;
    circle->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    /*If NULL, return NULL*/
    if(circle == NULL) {
        return NULL;
    }

    xmlAttr *attr;
    for (attr = tmp_Node->properties; attr != NULL; attr = attr->next) {
        char *attrName = (char *)attr->name;
        xmlNode *value = attr->children;
        char *cont = (char *)(value->content);
        /*When 'cx' is found, put the value inside the object*/
        if (strcmp(attrName, "cx") == 0) {
            circle->cx = atof(cont);
            
            char s2[256];
            int i = 0;
            int j = 0;

            for(i = 0, j = 0; cont[i] != '\0'; i++) {
                if((cont[i] >= 'A' && cont[i] <= 'Z') || (cont[i] >= 'a' && cont[i] <= 'z')) {
                    s2[j] = cont[i];
                    j++;
                }
            }
            s2[j]='\0';

            // char *unit = parseUnits(cont);
            // int length = strlen(unit);
            //strncpy(circle->units, unit, (length + 1));
            int length = strlen(s2);
            strncpy(circle->units, s2, (length + 1));
        /*When 'cy' is found, put the value inside the object*/
        } else if (strcmp(attrName, "cy") == 0) {
            circle->cy = atof(cont);

            char s2[256];
            int i = 0;
            int j = 0;

            for(i = 0, j = 0; cont[i] != '\0'; i++) {
                if((cont[i] >= 'A' && cont[i] <= 'Z') || (cont[i] >= 'a' && cont[i] <= 'z')) {
                    s2[j] = cont[i];
                    j++;
                }
            }
            s2[j]='\0';

            int length = strlen(s2);
            strncpy(circle->units, s2, (length + 1));
        /*When 'r' is found, put the value inside the object*/
        } else if (strcmp(attrName, "r") == 0) {
            circle->r = atof(cont);

            char s2[256];
            int i = 0;
            int j = 0;

            for(i = 0, j = 0; cont[i] != '\0'; i++) {
                if((cont[i] >= 'A' && cont[i] <= 'Z') || (cont[i] >= 'a' && cont[i] <= 'z')) {
                    s2[j] = cont[i];
                    j++;
                }
            }
            s2[j]='\0';

            int length = strlen(s2);
            strncpy(circle->units, s2, (length + 1));
        /*If anything else, puts into other attributes*/
        } else {
            char tmpStr[256];
            int memLength;
            Attribute *circleOtherAttr = malloc(sizeof(Attribute));

            /*For attribute name*/
            sprintf(tmpStr, attrName);
            memLength = strlen(tmpStr) + 2;
            circleOtherAttr->name = (char*)malloc(sizeof(char) * memLength);
            strcpy(circleOtherAttr->name, (char*)tmpStr);
            /*For attribute value*/
            sprintf(tmpStr, cont);
            strcpy(circleOtherAttr->value, tmpStr);
            //memLength = strlen(tmpStr) + 2;
            //circleOtherAttr->value = (char *)malloc(sizeof(char) * memLength);
            /*Prints out other attributes*/
            //printf("Name: %s Value: %s\n", circleOtherAttr->name, circleOtherAttr->value);
            insertBack(circle->otherAttributes, (void*)circleOtherAttr);
        }
    }

    return circle;
}

/**
 * @brief Parses all data that is required for Rectangle object 
 * @param tmp_Node 
 * @return Rectangle* 
 */
Rectangle *parseRectData(xmlNode *tmp_Node) {
    /*If NULL, return NULL*/
    if (tmp_Node == NULL) {
        return NULL;
    }
    
    Rectangle *rect = malloc(sizeof(Rectangle));

    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;
    rect->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);
    /*If NULL, return NULL*/
    if (rect == NULL) {
        return NULL;
    }

    xmlAttr *attr;
    for (attr = tmp_Node->properties; attr != NULL; attr = attr->next) {
        char *attrName = (char *)attr->name;
        xmlNode *value = attr->children;
        char *cont = (char *)(value->content);

        /*When 'x'' is found, put the value inside the object*/
        if (strcmp(attrName, "x") == 0) {
            rect->x = atof(cont);
            char s2[256];
            int i = 0;
            int j = 0;

            for(i = 0, j = 0; cont[i] != '\0'; i++) {
                if((cont[i] >= 'A' && cont[i] <= 'Z') || (cont[i] >= 'a' && cont[i] <= 'z')) {
                    s2[j] = cont[i];
                    j++;
                }
            }
            s2[j]='\0';

            int length = strlen(s2);
            strncpy(rect->units, s2, (length + 1));
            /*When 'y'' is found, put the value inside the object*/
        } else if (strcmp(attrName, "y") == 0) {
            rect->y = atof(cont);

            char s2[256];
            int i = 0;
            int j = 0;

            for(i = 0, j = 0; cont[i] != '\0'; i++) {
                if((cont[i] >= 'A' && cont[i] <= 'Z') || (cont[i] >= 'a' && cont[i] <= 'z')) {
                    s2[j] = cont[i];
                    j++;
                }
            }
            s2[j]='\0';

            int length = strlen(s2);
            strncpy(rect->units, s2, (length + 1));
            /*When width is found, put the value inside the object*/
        } else if (strcmp(attrName, "width") == 0) {
            rect->width = atof(cont);

            char s2[256];
            int i = 0;
            int j = 0;

            for(i = 0, j = 0; cont[i] != '\0'; i++) {
                if((cont[i] >= 'A' && cont[i] <= 'Z') || (cont[i] >= 'a' && cont[i] <= 'z')) {
                    s2[j] = cont[i];
                    j++;
                }
            }
            s2[j]='\0';

            int length = strlen(s2);
            strncpy(rect->units, s2, (length + 1));
            /*When height is found, put the value inside the object*/
        } else if (strcmp(attrName, "height") == 0) {
            rect->height = atof(cont);

            char s2[256];
            int i = 0;
            int j = 0;

            for(i = 0, j = 0; cont[i] != '\0'; i++) {
                if((cont[i] >= 'A' && cont[i] <= 'Z') || (cont[i] >= 'a' && cont[i] <= 'z')) {
                    s2[j] = cont[i];
                    j++;
                }
            }
            s2[j]='\0';

            int length = strlen(s2);
            strncpy(rect->units, s2, (length + 1));
            /*If anything else, puts into other attributes*/
        } else {
            char tmpStr[256];
            int memLength;
            Attribute *rectOtherAttr = malloc(sizeof(Attribute));

            /*For attribute name*/
            sprintf(tmpStr, attrName);
            memLength = strlen(tmpStr) + 2;
            rectOtherAttr->name = (char*)malloc(sizeof(char) * memLength);
            strcpy(rectOtherAttr->name, (char*)tmpStr);
            /*For attribute value*/
            sprintf(tmpStr, cont);
            strcpy(rectOtherAttr->value, tmpStr);
            /*Prints out other attributes*/
            //printf("Name: %s Value: %s\n", rectOtherAttr->name, rectOtherAttr->value);
            insertBack(rect->otherAttributes, (void*)rectOtherAttr);
        }
    }

    return rect;
}

/**
 * @brief Parses all data that is required for Group object 
 * @param tmp_Node 
 * @return Group* 
 */
Group *parseGroupData(xmlNode *tmp_Node) {
    if (tmp_Node == NULL) {
        return NULL;
    }

    /*Initialize Group List to be returned*/
    Group *group = malloc(sizeof(Group));
    group->rectangles = initializeList(&rectangleToString, &deleteRectangle, &compareRectangles);
    group->circles = initializeList(&circleToString, &deleteCircle, &compareCircles);
    group->paths = initializeList(&pathToString, &deletePath, &comparePaths);
    group->groups = initializeList(&groupToString, &deleteGroup, &compareGroups);
    group->otherAttributes = initializeList(&attributeToString, &deleteAttribute, &compareAttributes);

    /*If NULL, return NULL*/
    if (tmp_Node == NULL) {
        return group;
    }

    /*Parses all data for attributes of the Group object*/
    xmlAttr *attr;
    for (attr = tmp_Node->properties; attr != NULL; attr = attr->next) {
        char *attrName = (char *)attr->name;
        xmlNode *value = attr->children;
        char *cont = (char *)(value->content); 
    
        char tmpStr[256];
        int memLength;
        Attribute *groupOtherAttr = malloc(sizeof(Attribute));

        /*For attribute name*/
        sprintf(tmpStr, attrName);
        memLength = strlen(tmpStr) + 2;
        groupOtherAttr->name = (char*)malloc(sizeof(char) * memLength);
        strcpy(groupOtherAttr->name, (char*)tmpStr);
        /*For attribute value*/
        sprintf(tmpStr, cont);
        strcpy(groupOtherAttr->value, tmpStr);
        //printf("Name: %s Value: %s\n", groupOtherAttr->name, groupOtherAttr->value);
        insertBack(group->otherAttributes, (void*)groupOtherAttr);
    }

    /*Loops through the Group node find: Rect, Circles, Paths & Groups*/
    xmlNode *cur_node;
    for (cur_node = tmp_Node->children; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            char *nodeName = (char *)cur_node->name;

            if (strcmp(nodeName, "rect") == 0) {
                Rectangle *rect = parseRectData(cur_node);
                /*Rectangle object must not be NULL. It may be empty*/
                if (rect == NULL) {
                    //deleteRectangle(rect);
                    deleteGroup(group);
                    return NULL;
                }
                
                insertBack(group->rectangles, (void*)rect);
            } else if (strcmp(nodeName, "circle") == 0) {
                Circle *circle = parseCircleData(cur_node);
                /*Circle object must not be NULL. It may be empty*/
                if (circle == NULL) {
                    //deleteCircle(circle);
                    deleteGroup(group);
                    return NULL;
                }
                
                insertBack(group->circles, (void*)circle);
            } else if (strcmp(nodeName, "path") == 0) {
                Path *path = parsePathData(cur_node);
                /*Path object must not be NULL. It may be empty*/
                if (path == NULL) {
                    //deletePath(path);
                    deleteGroup(group);
                    return NULL;
                }
                insertBack(group->paths, (void*)path);
            } else if (strcmp(nodeName, "g") == 0) {
                Group *group = parseGroupData(cur_node);
                /*Group object must not be NULL. It may be empty*/
                if (group == NULL) {
                    deleteGroup(group);
                    return NULL;
                }
                
                insertBack(group->groups, (void*)group);
            }
        }
    }

    return group;
}

/**
 * @brief Finds any objects that are within a Group object
 * @param list 
 * @param group 
 */
void parseGroupWithinGroup(List* list, Group* group) {
    ListIterator listIterator = createIterator(group->groups);
    void* elem;
    /*Iterates thru the list, puts all groups into a list, and then checks
    if there's a group within a group*/
    while ((elem = nextElement(&listIterator)) != NULL) {
        if (elem != NULL) {
            insertBack(list, (void*)elem);
        }

        parseGroupWithinGroup(list, (Group*)elem);
    }
}

/********************************* A2 Functions *************************************/

/**
 * @brief Add a new rectangle node to the Root node
 * @param root_node 
 * @param rectList
 */
void rectToNode(xmlNodePtr root_node, List *rectList) {
    xmlNodePtr groupNode = NULL;
    void *elem;

    ListIterator iter = createIterator(rectList);

    while ((elem = nextElement(&iter)) != NULL) {
        Rectangle *tempRect = (Rectangle*)elem;
        char buff[256];

        groupNode = xmlNewChild(root_node, NULL, BAD_CAST "rect", NULL);
        
        /*Adding attributes to the rectangle node*/
        sprintf(buff, "%f%s", tempRect->x, tempRect->units);
        xmlNewProp(groupNode, BAD_CAST "x", BAD_CAST buff);
        sprintf(buff, "%f%s", tempRect->y, tempRect->units);
        xmlNewProp(groupNode, BAD_CAST "y", BAD_CAST buff);
        sprintf(buff, "%f%s", tempRect->width, tempRect->units);
        xmlNewProp(groupNode, BAD_CAST "width", BAD_CAST buff);
        sprintf(buff, "%f%s", tempRect->height, tempRect->units);
        xmlNewProp(groupNode, BAD_CAST "height", BAD_CAST buff);

        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempRect->otherAttributes);

        /*Checks the other attributes for the Rect object*/
        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            // if ((tmpAttr->name) == NULL && (tmpAttr->value) == NULL) {
            //     continue;
            // }

            xmlNewProp(groupNode, BAD_CAST tmpAttr->name, BAD_CAST tmpAttr->value);
        }
    }
}

/**
 * @brief Add a new circle node to the Root node
 * @param root_node 
 * @param circList 
 */
void circleToNode(xmlNodePtr root_node, List *circList) {
    xmlNodePtr circNode = NULL;
    void *elem;

    ListIterator iter = createIterator(circList);

    while ((elem = nextElement(&iter)) != NULL) {
        Circle *tempCirc = (Circle*)elem;
        char buff[256];

        circNode = xmlNewChild(root_node, NULL, BAD_CAST "circle", NULL);
        
        /*Adding attributes to the Circle node*/
        sprintf(buff, "%f%s", tempCirc->cx, tempCirc->units);
        xmlNewProp(circNode, BAD_CAST "cx", BAD_CAST buff);
        sprintf(buff, "%f%s", tempCirc->cy, tempCirc->units);
        xmlNewProp(circNode, BAD_CAST "cy", BAD_CAST buff);
        sprintf(buff, "%f%s", tempCirc->r, tempCirc->units);
        xmlNewProp(circNode, BAD_CAST "r", BAD_CAST buff);
        //sprintf(buff, "%s", tempCirc->units);
        //xmlNewProp(node, BAD_CAST "units", BAD_CAST buff);

        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempCirc->otherAttributes);

        /*Adding other attributes to the Circle node*/
        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            // if ((tmpAttr->name) == NULL && (tmpAttr->value) == NULL) {
            //     continue;
            // }

            xmlNewProp(circNode, BAD_CAST tmpAttr->name, BAD_CAST tmpAttr->value);
        }
    }
}

/**
 * @brief Add a new path node to the Root node
 * @param root_node 
 * @param pathList 
 */
void pathToNode(xmlNodePtr root_node, List *pathList) {
    xmlNodePtr pathNode = NULL;
    void *elem;

    ListIterator iter = createIterator(pathList);

    while ((elem = nextElement(&iter)) != NULL) {
        Path *tempPath = (Path*)elem;

        pathNode = xmlNewChild(root_node, NULL, BAD_CAST "path", NULL);
        
        /*Adding data to the Path node*/
        xmlNewProp(pathNode, BAD_CAST "d", BAD_CAST tempPath->data);

        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempPath->otherAttributes);

        /*Adding other attributes to the Path node*/
        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            // if ((tmpAttr->name) == NULL && (tmpAttr->value) == NULL) {
            //     continue;
            // }

            xmlNewProp(pathNode, BAD_CAST tmpAttr->name, BAD_CAST tmpAttr->value);
        }
    }
}

/**
 * @brief Add a new group node to the Root node
 * @param root_node 
 * @param groupList 
 */
void groupToNode(xmlNodePtr root_node, List *groupList) {
    xmlNodePtr groupRootNode = NULL;
    void *elem;

    ListIterator iter = createIterator(groupList);

    while ((elem = nextElement(&iter)) != NULL) {
        Group *tempGroup = (Group*)elem;

        groupRootNode = xmlNewChild(root_node, NULL, BAD_CAST "g", NULL);
        
        /*Adding attributes to the Group node*/
        rectToNode(groupRootNode, tempGroup->rectangles);
        circleToNode(groupRootNode, tempGroup->circles);
        pathToNode(groupRootNode, tempGroup->paths);
        groupToNode(groupRootNode, tempGroup->groups);

        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempGroup->otherAttributes);

        /*Adding other attributes to the Group node*/
        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            // if ((tmpAttr->name) == NULL && (tmpAttr->value) == NULL) {
            //     continue;
            // }

            xmlNewProp(groupRootNode, BAD_CAST tmpAttr->name, BAD_CAST tmpAttr->value);
        }
    }
}

/**
 * @brief Validates each element in the Rect List
 * @param rectList 
 * @return true 
 * @return false 
 */
bool validateRect(List *rectList) {
    void *elem;
    ListIterator iter = createIterator(rectList);

    while ((elem = nextElement(&iter)) != NULL) {
        Rectangle *tempRect = (Rectangle*)elem;

        /*Validates based on the properties for Rectangle*/
        if (tempRect->height < 0) {
            return false;
        }
        if (tempRect->width < 0) {
            return false;
        }
        if (tempRect->otherAttributes == NULL) {
            return false;
        }

        /*Checks the other attributes for the Path object*/
        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempRect->otherAttributes);

        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            if (tmpAttr->name == NULL) {
                return false;
            }
        }
    }
    /*If everything is valid, then return true*/
    return true;
}

/**
 * @brief Validates each element in the Circle list
 * @param circList 
 * @return true 
 * @return false 
 */
bool validateCirc(List *circList) {
    void *elem;
    ListIterator iter = createIterator(circList);

    while ((elem = nextElement(&iter)) != NULL) {
        Circle *tempCirc = (Circle*)elem;

        /*Validates based on the properties for Circle*/
        if (tempCirc->r < 0) {
            return false;
        }
        if (tempCirc->otherAttributes == NULL) {
            return false;
        }

        /*Checks the other attributes for the Path object*/
        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempCirc->otherAttributes);

        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            if (tmpAttr->name == NULL) {
                return false;
            }
        }
    }
    /*If everything is valid, then return true*/
    return true;
}

/**
 * @brief Validates each element in the Path list
 * @param pathList 
 * @return true 
 * @return false 
 */
bool validatePath(List *pathList) {
    void *elem;
    ListIterator iter = createIterator(pathList);

    while ((elem = nextElement(&iter)) != NULL) {
        Path *tempPath = (Path*)elem;

        /*Validates based on the properties for Path*/
        if (tempPath->data == NULL) {
            return false;
        }
        if (tempPath->otherAttributes == NULL) {
            return false;
        }

        /*Checks the other attributes for the Path object*/
        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempPath->otherAttributes);

        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            if (tmpAttr->name == NULL) {
                return false;
            }
        }
    }
    /*If everything is valid, then return true*/
    return true;
}

/**
 * @brief Validates each element in the Group list
 * @param groupList 
 * @return true 
 * @return false 
 */
bool validateGroup(List *groupList) {
    void *elem;
    ListIterator iter = createIterator(groupList);

    while ((elem = nextElement(&iter)) != NULL) {
        Group *tempGroup = (Group*)elem;

        /*Validates based on the properties for Group*/
        if (tempGroup->circles == NULL || tempGroup->rectangles == NULL || tempGroup->paths == NULL || tempGroup->groups == NULL) {
            return false;
        }
        if (tempGroup->otherAttributes == NULL) {
            return false;
        }

        /*Checks the other attributes for the Path object*/
        ListIterator attrIter;
        void* elem2;
        attrIter = createIterator(tempGroup->otherAttributes);

        while((elem2 = nextElement(&attrIter)) != NULL){
            Attribute* tmpAttr = (Attribute*)elem2;

            if (tmpAttr->name == NULL) {
                return false;
            }
        }
    }
    /*If everything is valid, then return true*/
    return true;
}

/**
 * @brief Converts SVG Object to XML Doc
 * @param tmpImage 
 * @return xmlDoc* 
 */
xmlDoc* svgToXML(const SVG *tmpImage) {
    /*If SVG image is NULL, return false*/
    if (tmpImage == NULL) {
        return NULL;
    }

    xmlDoc* doc = NULL;
    xmlNode* root_node = NULL;

    LIBXML_TEST_VERSION;

    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "svg");
    xmlDocSetRootElement(doc, root_node);

    if (doc == NULL) {
        return NULL;
    }

    /*Sets the namespace in the XML tree*/
    if (strcmp(tmpImage->namespace, "") == 0 || (tmpImage->namespace) == NULL) {
        return NULL;
    } else {
        xmlNsPtr ns = xmlNewNs(root_node, BAD_CAST tmpImage->namespace, NULL);
        xmlSetNs(root_node, ns);
    }

    /*Sets the title of SVG image in the XML tree*/
    if (strcmp(tmpImage->title, "") != 0) {
        xmlNewChild(root_node, NULL, BAD_CAST "title", BAD_CAST tmpImage->title);
    }

    /*Sets the description of SVG image in the XML tree*/
    if (strcmp(tmpImage->description, "") != 0) {
        xmlNewChild(root_node, NULL, BAD_CAST "desc", BAD_CAST tmpImage->description);
    }

    rectToNode(root_node, tmpImage->rectangles);
    circleToNode(root_node, tmpImage->circles);
    pathToNode(root_node, tmpImage->paths);
    groupToNode(root_node, tmpImage->groups);

    ListIterator attrIter;
    void* elem;
    attrIter = createIterator(tmpImage->otherAttributes);

    /*Adding other attributes to the SVG Object node*/
    while((elem = nextElement(&attrIter)) != NULL){
        Attribute* tmpAttr = (Attribute*)elem;

        if ((tmpAttr->name) == NULL && (tmpAttr->value) == NULL) {
            continue;
        }

        xmlNewProp(root_node, BAD_CAST tmpAttr->name, BAD_CAST tmpAttr->value);
    }

    if (doc != NULL) {
        return doc;
    } else {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        xmlMemoryDump();
        /*Return false*/
        return NULL;
    }
}

/**
 * @brief Set the Other Attribute object for either Rect, Circ, Path, Group or SVG_Image
 * @param list 
 * @param newAttribute 
 */
void setOtherAttribute(List *list, Attribute *newAttribute) {
    /*If NULL, return NULL*/
    if (list == NULL || newAttribute == NULL) {
        return ;
    }

    /*If the attribute with the specified name exists in list, update the value*/
    ListIterator iter = createIterator(list);
    void* elem;
  
    while ((elem = nextElement(&iter)) != NULL) {
        Attribute* otherAttr = (Attribute*)elem;

        if (strcmp(newAttribute->name, otherAttr->name) == 0) {
            char tmpStr[1000];
            sprintf(tmpStr, newAttribute->value);
            //free(otherAttr->value);
            strcpy(otherAttr->value, tmpStr);

            return ;
        }
    }
    /*If the attribute with the specified name does not exist in list, append to the list*/
    insertBack(list, (void*)newAttribute);
}

/**
 * @brief Helper function to return a JSON string of an SVG Object
 * @param fileName 
 * @param schemaFile 
 * @return char* 
 */
char* validImageToJSON(const char* fileName, const char* schemaFile) {
    if (fileName == NULL || schemaFile == NULL) {
        return NULL;
    }

    SVG* imageObj = createValidSVG(fileName, schemaFile);

    if (imageObj == NULL) {
        deleteSVG(imageObj);
        return NULL;
    } else {
        char* jsonChar = SVGtoJSON(imageObj);

        deleteSVG(imageObj);
        return jsonChar;
    }
}

