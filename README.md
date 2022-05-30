#CIS*2750 SVG Image Web Application

## CIS*2750 - Assignment 1 (Parsing)

This project/code focuses on creating a set of structs representing SVG files
using libxml2. It then goes to using a set of accessor and search functions that will
accompany the parser and the structs.

## CIS*2750 - Assignment 2 (C to JS)

Assignment 2 modules:
1. Functions for creating valid SVGs, validating SVGs, and writing SVGs to a file.
2. Functions for adding components to SVGs.
3. A set of "glue" functions to convert an SVG and its components into JSON strings - and vice versa.

## Author
Abdullah Khan - (mkhan94@uoguelph.ca)

## How to Compile/Run
 * Compilation: 
```
gcc -Wall -I/usr/include/libxml2/ -lm -I../include/ main.c SVGParser.c SVGHelpers.c LinkedListAPI.c -lxml2 -ggdb3
```
 * Run: 
```
./a.out [.xml/.svg file]
```
## Date
2022-01-20

Copyright (c) 2022
