#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "SVGParser.h"
#include "LinkedListAPI.h"

/*Defining the PI constant*/
#define PI 3.14159265358979323846

/**
 * @brief Finds any objects that are within a Group object
 * @param list 
 * @param group 
 */
void parseGroupWithinGroup(List* list, Group* group);

/**
 * @brief Parses all data that is required for Path object 
 * @param tmp_Node 
 * @return Path* 
 */
Path *parsePathData(xmlNode *tmp_Node);

/**
 * @brief Parses all data that is required for Circle object 
 * @param tmp_Node 
 * @return Circle* 
 */
Circle *parseCircleData(xmlNode *tmp_Node);

/**
 * @brief Parses all data that is required for Rectangle object 
 * @param tmp_Node 
 * @return Rectangle* 
 */
Rectangle *parseRectData(xmlNode *tmp_Node);

/**
 * @brief Parses all data that is required for Group object 
 * @param tmp_Node 
 * @return Group* 
 */
Group *parseGroupData(xmlNode *tmp_Node);

/********************************* A2 Functions *************************************/

/**
 * @brief 
 * @param root_node 
 * @param rectList 
 */
void rectToNode(xmlNodePtr root_node, List *rectList);

/**
 * @brief 
 * 
 * @param root_node 
 * @param pathList 
 */
void pathToNode(xmlNodePtr root_node, List *pathList);

/**
 * @brief 
 * 
 * @param root_node 
 * @param circList 
 */
void circleToNode(xmlNodePtr root_node, List *circList);

/**
 * @brief 
 * 
 * @param root_node 
 * @param groupList 
 */
void groupToNode(xmlNodePtr root_node, List *groupList);

/**
 * @brief Validates each element in the Rect List
 * @param rectList 
 * @return true 
 * @return false 
 */
bool validateRect(List *rectList);

/**
 * @brief Validates each element in the Circle list
 * @param circList 
 * @return true 
 * @return false 
 */
bool validateCirc(List *circList);

/**
 * @brief Validates each element in the Path list
 * @param pathList 
 * @return true 
 * @return false 
 */
bool validatePath(List *pathList);

/**
 * @brief Validates each element in the Group list
 * @param groupList 
 * @return true 
 * @return false 
 */
bool validateGroup(List *groupList);

/**
 * @brief Converts SVG Object to XML Doc
 * @param tmpImage 
 * @return xmlDoc* 
 */
xmlDoc* svgToXML(const SVG *tmpImage);

/**
 * @brief Set the Other Attribute object for either Rect, Circ, Path, Group or SVG_Image
 * @param list 
 * @param newAttribute 
 */
void setOtherAttribute(List *list, Attribute *newAttribute);

/**
 * @brief Helper function to return a JSON string of an SVG Object
 * @param fileName 
 * @param schemaFile 
 * @return char* 
 */
char* validImageToJSON(const char* fileName, const char* schemaFile);