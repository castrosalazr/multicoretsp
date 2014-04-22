//
//  genetico_serial.h
//  geneticalgorithm
//
//  Created by Rafael Castro Salazar on 15/04/14.
//  Copyright (c) 2014 ITESM CCM. All rights reserved.
//

#ifndef geneticalgorithm_genetico_serial_h
#define geneticalgorithm_genetico_serial_h

int* genetico_serial_rutaOptima(int, float*, float*);
float fitness(int*);
float averageFitness();
int** cruzar(int*, int*);
void aux_cruce(int*, int, int*, int*);
int* mutar(int*);
int** poblacionInicial();

#endif
