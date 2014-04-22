//
//  city.h
//  multicoreTSP
//
//  Created by Rafael Castro Salazar on 07/04/14.
//  Copyright (c) 2014 ITESM CCM. All rights reserved.
//

#ifndef multicoreTSP_city_h
#define multicoreTSP_city_h

// Al copiarse a memoria constante (en OpenCL o CUDA), el máximo son 64K; cada ciudad ocupa 64 bits. .: 2^16/2^6=2^10 ciudades.
#define MAX_CITIES 1024

typedef struct cityNode {
    float latitud;
    float longitud;
    char *nombre;
    struct cityNode *next;
} City;

extern int ciudades;
extern char **nombresCiudades;
extern float *latitudes, *longitudes;
extern City *inicio, *ultimo;

int inicializarCiudades(const char *);

#endif
