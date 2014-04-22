//
//  City_Utils.c
//  parallelTSP
//
//  Created by Rafael Castro Salazar on 18/04/14.
//  Copyright (c) 2014 ITESM CCM. All rights reserved.
//

#include <math.h>

float distanciaRuta(int ciudades, int *ruta, float *latitudes, float *longitudes) {
    float distancia = 0.0f;
     // Calcular la distancia entre cada par de ciudades.
     for( int i=1; i<ciudades; i++ )
     distancia += sqrt( pow(latitudes[ruta[i]]-latitudes[ruta[i-1]],2) + pow(longitudes[ruta[i]]-longitudes[ruta[i-1]],2) );
     // Calcular la distancia entre la ciudad final y la ciudad inicial.
     distancia += sqrt( pow(latitudes[ruta[ciudades-1]]-latitudes[ruta[0]],2) + pow(longitudes[ruta[ciudades-1]]-longitudes[ruta[0]],2) );
    return distancia * 111.32; // Aprox. 1 grado = 111.32 km.
}