//
//  city.c
//  multicoreTSP
//
//  Created by Rafael Castro Salazar on 07/04/14.
//  Copyright (c) 2014 ITESM CCM. All rights reserved.
//

#import "City.h"
#import <libxml/xmlmemory.h>
#import <libxml/parser.h>
#import <math.h>
#import <string.h>

int ciudades = 0;
char **nombresCiudades;
float *latitudes, *longitudes;
City *inicio, *ultimo;

void parseCity(xmlDocPtr doc, xmlNodePtr cur) {
    // Crear ciudad.
    xmlNodePtr cityPtr = cur->xmlChildrenNode;
    City *ciudad = (City*)malloc(sizeof(City));
    unsigned char *country = NULL, *state = NULL, *cityName = NULL;
    // Asignar valores a las variables.
    while( cityPtr != NULL ) {
        if( (!xmlStrcmp(cityPtr->name, (const xmlChar *)"name")) )
            cityName = xmlNodeListGetString(doc, cityPtr->xmlChildrenNode, 1);
        else if( (!xmlStrcmp(cityPtr->name, (const xmlChar *)"state")) )
            state = xmlNodeListGetString(doc, cityPtr->xmlChildrenNode, 1);
        else if( (!xmlStrcmp(cityPtr->name, (const xmlChar *)"country")) )
            country = xmlNodeListGetString(doc, cityPtr->xmlChildrenNode, 1);
        else if( (!xmlStrcmp(cityPtr->name, (const xmlChar *)"lat")) )
            ciudad->latitud = atof((char*)xmlNodeListGetString(doc, cityPtr->xmlChildrenNode, 1));
        else if( (!xmlStrcmp(cityPtr->name, (const xmlChar *)"long")) )
            ciudad->longitud = atof((char*)xmlNodeListGetString(doc, cityPtr->xmlChildrenNode, 1));
        cityPtr = cityPtr->next;
    }
    char nameBuffer[255];
    int nameLength = 0;
    if( country && state && cityName )
        nameLength = sprintf(nameBuffer, "%s, %s, %s", cityName, state, country);
    else if( cityName )
        nameLength = sprintf(nameBuffer, "%s", cityName);
    else
        nameLength = sprintf(nameBuffer, "(%.4f, %.4f)", ciudad->latitud, ciudad->longitud);
    ciudad->nombre = (char *)malloc(sizeof(char)*(nameLength+1));
    strcpy(ciudad->nombre, nameBuffer);
    // Aumentar el contador global de ciudades.
    ciudades++;
    // Enlistar ciudad.
    if( inicio == NULL )
        inicio = ciudad;
    else
        ultimo->next = ciudad;
    ultimo = ciudad;
}

int inicializarCiudades(const char *archivoXML) {
    ciudades = 0;
    inicio = NULL;
    ultimo = NULL;
    // Parsear lista de ciudades.
    xmlDocPtr doc = xmlParseFile(archivoXML);
    if( doc == NULL ) {
		fprintf(stderr, "No se pudo parsear el archivo XML.\n");
		return 1;
	}
    // Comprobar archivo XML.
   	xmlNodePtr cur = xmlDocGetRootElement(doc);
    if( cur == NULL ) {
		fprintf(stderr, "XML Vacío.\n");
		xmlFreeDoc(doc);
		return 1;
	}
	if( xmlStrcmp(cur->name, (const xmlChar *) "cities") ) {
		fprintf(stderr, "XML Inválido.");
		xmlFreeDoc(doc);
		return 1;
	}
    // Procesar datos contenidos en el XML.
    cur = cur->xmlChildrenNode;
	while( cur != NULL ) {
		if( (!xmlStrcmp(cur->name, (const xmlChar *)"city")) )
			parseCity(doc, cur);
        cur = cur->next;
	}
    // Comprobar máximo de ciudades.
    if( ciudades > MAX_CITIES ) {
        fprintf(stderr, "El máximo de ciudades soportadas por este programa es %d.", MAX_CITIES);
        return 1;
    }
    // Comprobar que haya ciudades!
    if( ciudades < 1 ) {
        fprintf(stderr, "¡El archivo XML no contenía ciudades! D:<");
        return 1;
    }
    latitudes = (float*)malloc( sizeof(float)*(ciudades+1) );
    longitudes = (float*)malloc( sizeof(float)*(ciudades+1) );
    nombresCiudades = (char**)malloc( sizeof(char)*255*(ciudades+1) );
    City *ptr = inicio;
    for( int index=0; index<ciudades; index++ ) {
        latitudes[index] = ptr->latitud;
        longitudes[index] = ptr->longitud;
        nombresCiudades[index] = ptr->nombre;
        ptr = ptr->next;
    }
    return 0;
}