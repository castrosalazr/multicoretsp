//
//  genetico_serial.c
//  geneticalgorithm
//
//  Created by Rafael Castro Salazar on 15/04/14.
//  Copyright (c) 2014 ITESM CCM. All rights reserved.
//

#import "genetico_serial.h"
#import "City_Utils.h"
#import <stdio.h>
#import <stdlib.h>
#import <unistd.h>
#import <time.h>

#define TAM_POBLACION 50
#define MAX_ITERACIONES 250
#define PROB_MUTACION 0.01
#define PROB_CRUCE 0.85
#define DEBUG 1

int **poblacion;
int ciudades;
int iteraciones;
double mejorDistancia;
double deltaDistancia;
float *latitudes;
float *longitudes;

int* genetico_serial_rutaOptima( int _ciudades, float *_latitudes, float *_longitudes ) {
    int *rutaOptima = NULL;
    int poblaciones = 0;
    mejorDistancia = 0;
    deltaDistancia = 0;
    iteraciones = 0;
    ciudades = _ciudades;
    latitudes = _latitudes;
    longitudes = _longitudes;
    // Generar poblacion inicial.
    poblacion = poblacionInicial();
    // Inicializar mejor distancia y mejor ruta.
    mejorDistancia = fitness(poblacion[0]);
    do {
        if( DEBUG ) printf("\n\nPoblaciones: %d", ++poblaciones);
        deltaDistancia = mejorDistancia;
        // Buscar la ruta con mejor distancia entre la población.
        for( int i=0; i<TAM_POBLACION; i++ ) {
            // Imprimir población.
            if( DEBUG ) {
                printf("\n> Ruta %d: ", i);
                for( int j=0; j<ciudades; j++ )
                    printf("%d ", poblacion[i][j]);
            }
            double distanciaTmp = fitness(poblacion[i]);
            if( distanciaTmp < mejorDistancia ) {
                mejorDistancia = distanciaTmp;
                rutaOptima = poblacion[i];
            }
        }
        deltaDistancia = mejorDistancia - deltaDistancia;
        if( !abs(deltaDistancia) ) iteraciones++;
        else iteraciones = 0;
        // Generar nueva población.
        int **nuevaPoblacion = (int**)malloc( sizeof(int)*ciudades*TAM_POBLACION );
        for( int i=0; i<TAM_POBLACION; i++ ) {
            nuevaPoblacion[i] = (int*)malloc( sizeof(int)*ciudades );
            for( int j=0; j<ciudades; j++ )
                nuevaPoblacion[i][j] = poblacion[i][j];
        }
        srand( (unsigned int)time(NULL) );
        for( int i=0; i<TAM_POBLACION; i+=2 ) {
            int *padre1 = poblacion[ rand()%TAM_POBLACION ];
            int *padre2 = poblacion[ rand()%TAM_POBLACION ];
            // Si se cumple la probabilidad de cruce, cruzar; si no, copiar.
            if( (double)rand()/(double)RAND_MAX < PROB_CRUCE ) {
                int **hijos = cruzar(padre1, padre2);
                if( (double)rand()/(double)RAND_MAX < PROB_MUTACION ) {
                    hijos[0] = mutar(hijos[0]);
                    hijos[1] = mutar(hijos[1]);
                }
                nuevaPoblacion[i] = hijos[0];
                nuevaPoblacion[i+1] = hijos[1];
            }
            else {
                nuevaPoblacion[i] = (int*)malloc(sizeof(int)*ciudades);
                nuevaPoblacion[i+1] = (int*)malloc(sizeof(int)*ciudades);
                for( int j=0; j<ciudades; j++ ) {
                    nuevaPoblacion[i][j] = padre1[j];
                    nuevaPoblacion[i+1][j] = padre2[j];
                }
            }
        }
        poblacion = nuevaPoblacion;
    }while( iteraciones<MAX_ITERACIONES );
    if( DEBUG ) {
        printf("\nMejor distancia: %f\n[", mejorDistancia);
        for( int i=0; i<ciudades; i++ )
            printf("%d ", rutaOptima[i]);
        printf("]\n");
    }
    return rutaOptima;
}

int** poblacionInicial() {
    int** poblacionInicial = (int**)malloc( sizeof(int)*ciudades*TAM_POBLACION );
    srand( (unsigned int)time(NULL) );
    // Llenar población inicial.
    for( int i=0; i<TAM_POBLACION; i++ ) {
        poblacionInicial[i] = (int*)malloc( sizeof(int)*ciudades );
        // Crear ruta con todos los índices.
        for( int j=0; j<ciudades; j++ )
            poblacionInicial[i][j] = j;
        // Barajar elementos de la ruta.
        for( int j=0; j<ciudades; j++ ) {
            int index = rand()%ciudades;
            int tmp = poblacionInicial[i][index];
            poblacionInicial[i][index] = poblacionInicial[i][j];
            poblacionInicial[i][j] = tmp;
        }
    }
    return poblacionInicial;
}

float averageFitness() {
    float promedio = 0.0f;
    for( int i=0; i<TAM_POBLACION; i++ )
        promedio += fitness( poblacion[i] );
    promedio /= TAM_POBLACION;
    return promedio;
}

float fitness(int *ruta) {
    return distanciaRuta(ciudades, ruta, latitudes, longitudes);
}

int** cruzar(int *ruta1, int *ruta2) {
    int **cruce = (int**)malloc(sizeof(int)*ciudades*2);
    // Inicializar nuevas rutas.
    cruce[0] = (int*)malloc(sizeof(int)*ciudades);
    cruce[1] = (int*)malloc(sizeof(int)*ciudades);
    // Establecer un punto de cruce aleatorio.
    int indiceCruce = 1+rand()%(ciudades-2);
    // Hasta el punto de cruce, los hijitos permanecen igual.
    for( int i=0; i<indiceCruce; i++ ) {
        cruce[0][i] = ruta1[i];
        cruce[1][i] = ruta2[i];
    }
    // A partir del punto de cruce, se ponen los dígitos del otro padre, siempre y cuando no se repitan.
    aux_cruce(cruce[0], indiceCruce, ruta1, ruta2);
    aux_cruce(cruce[1], indiceCruce, ruta2, ruta1);
    if( DEBUG ) {
        printf("\nCruzando ");
        for( int i=0; i<ciudades; i++ )
            printf("%d ", ruta1[i]);
        printf("y ");
        for( int i=0; i<ciudades; i++ )
            printf("%d ", ruta2[i]);
        printf(":\nCruce 1:");
        for( int i=0; i<ciudades; i++ )
            printf("%d ", cruce[0][i]);
        printf("\nCruce 2:");
        for( int i=0; i<ciudades; i++ )
            printf("%d ", cruce[1][i]);
        printf("\n");
    }
    return cruce;
}

void aux_cruce(int *cruce, int indiceCruce, int *ruta1, int *ruta2) {
    // Copiar índices no repetidos de la cadena contraria.
    for( int i=indiceCruce; i<ciudades; i++ ) {
        int tmp = ruta2[i];
        int encontrado = 0;
        for( int j=0; j<indiceCruce && !encontrado; j++ )
            if( cruce[j] == tmp )
                encontrado = 1;
        if( encontrado ) {
            for( int j=0; j<indiceCruce; j++ ) {
                int indice = ruta2[j];
                int igual = 0;
                for( int k=0; k<i && !igual; k++ )
                    if( cruce[k] == indice )
                        igual = 1;
                if( !igual )
                    tmp = indice;
            }
        }
        cruce[i] = tmp;
    }
}

int* mutar(int *ruta) {
    srand( (unsigned int)time(NULL) );
    int *mutacion = (int*)malloc(sizeof(int)*ciudades);
    // Copiar solución original.
    for( int i=0; i<ciudades; i++ )
        mutacion[i] = ruta[i];
    // Escoger dos índices aleatorios para hacer un swap.
    int index1 = rand()%ciudades;
    int index2 = -1;
    do {
        int tmp = rand()%ciudades;
        if( tmp != index1 )
            index2 = tmp;
    } while( index2 < 0 );
    int swap = mutacion[index2];
    mutacion[index2] = mutacion[index1];
    mutacion[index1] = swap;
    return mutacion;
}