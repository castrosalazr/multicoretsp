//
//  PGAAppDelegate.m
//  geneticalgorithm
//
//  Created by Rafael Castro Salazar on 15/04/14.
//  Copyright (c) 2014 ITESM CCM. All rights reserved.
//

#import "PGAAppDelegate.h"
#import "genetico_serial.h"
#import "City.h"
#import "City_Utils.h"
#import <WebKit/WebFrame.h>
#import <math.h>

#define GOOGLE_MAP_URL "http://maps.googleapis.com/maps/api/staticmap?sensor=false&size=520x280&maptype=roadmap&key=AIzaSyC-5Y7xufLzMEL2w3eDw08GyDAzhpHyrJc"

@implementation PGAAppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    self.algorithmNames = [NSArray arrayWithObjects:
                           @"Algoritmo Genético Serial (CPU)",
                           @"Algoritmo Genético Paralelo (GPU)",
                           nil];
    [self.algorithms addItemsWithTitles:self.algorithmNames];
    self.inputCities = [[NSMutableArray alloc] init];
    [self.inputFileLocation setEditable:NO];
    [self.startSolving setEnabled:NO];
    self.zoomOptimo = 1;
    self.centerLat = 20;
    self.centerLong = 2;
    [[self.webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:[NSString
           stringWithFormat:@"%s&zoom=%d&center=%f,%f", GOOGLE_MAP_URL, self.zoomOptimo, self.centerLat, self.centerLong]]]];
}

- (IBAction)browseForFile:(id)sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseDirectories:NO];
    [panel setCanChooseFiles:YES];
    [panel setCanCreateDirectories:NO];
    [panel setAllowsMultipleSelection:NO];
    [panel setAllowsOtherFileTypes:NO];
    [panel setAllowedFileTypes:[NSArray arrayWithObject:@"xml"]];
    [panel beginSheetModalForWindow:self.window completionHandler:^(NSInteger result){
        if( result == NSFileHandlingPanelOKButton ) {
            NSMutableString *map_parameters = [[NSMutableString alloc] init];
            [self.inputFileLocation setStringValue:[[panel URL] path]];
            [self.inputCities removeAllObjects];
            inicializarCiudades( [self.inputFileLocation.stringValue UTF8String] );
            // Recorrer ciudades para agregarlas al tableview.
            float promedioLat=0.0f, promedioLong=0.0f, minLat=0.0f, minLong=0.0f, maxLat=0.0f, maxLong=0.0f;
            City *ptr = inicio;
            for( int i=0; i<ciudades; i++ ) {
                NSValue *tmp = [NSValue value:&ptr withObjCType:@encode(City)];
                [self.inputCities addObject:tmp];
                [map_parameters appendString:[NSString stringWithFormat:@"&markers=color:red%%7Clabel:%d%%7C%f,%f", i, ptr->latitud, ptr->longitud]];
                promedioLat += ptr->latitud;
                promedioLong += ptr->longitud;
                if( !i ) {
                    minLat = ptr->latitud;
                    maxLat = ptr->latitud;
                    minLong = ptr->longitud;
                    maxLong = ptr->longitud;
                }
                else {
                    if( ptr->latitud < minLat ) minLat = ptr->latitud;
                    if( ptr->latitud > maxLat ) maxLat = ptr->latitud;
                    if( ptr->longitud < minLong ) minLong = ptr->longitud;
                    if( ptr->longitud > maxLong ) maxLong = ptr->longitud;
                }
                ptr = ptr->next;
            }
            [self.citiesTableView reloadData];
            [[self.outputTextView textStorage] appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"\nSe cargaron %d ciudades.\n", ciudades]]];
            [self.startSolving setEnabled:YES];
            // Recargar mapa con marcadores.
            self.centerLat = promedioLat/ciudades;
            self.centerLong = promedioLong / ciudades;
            // Distancia entre el punto más al suroeste y el punto más al noreste (en grados) para determinar el mejor zoom.
            float distanciaFrame = sqrt(pow(maxLat-minLat, 2)+pow(maxLong-minLong, 2));
            self.zoomOptimo = distanciaFrame/10; // Al tanteo. :P
            [[self.webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:[NSString
                stringWithFormat:@"%s%@&zoom=%d&center=%f,%f", GOOGLE_MAP_URL, map_parameters, self.zoomOptimo, self.centerLat, self.centerLong]]]];
        }
    }];
}

- (IBAction)solve:(id)sender {
    NSLog(@"Algoritmo seleccionado: %@ (%ld)", [self.algorithmNames objectAtIndex:self.algorithms.indexOfSelectedItem], (long)self.algorithms.indexOfSelectedItem);
    [self.algorithms setEnabled:NO];
    [self.startSolving setEnabled:NO];
    [self.browse setEnabled:NO];
    [self.progress startAnimation:self.progress];
    dispatch_async(dispatch_get_global_queue(0,0), ^{
        [self resolver:self.algorithms.indexOfSelectedItem];
    });
}

- (void)resolver:(long)indiceAlgoritmo {
    __block int *rutaOptima = NULL;
    NSDate *start = [NSDate date];
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_HIGH, 0), ^{
        if( !indiceAlgoritmo ) // Genético Serial
            rutaOptima = genetico_serial_rutaOptima((int)self.inputCities.count, latitudes, longitudes);
        else if( indiceAlgoritmo == 1 ) // Genético Paralelo
            ;
        dispatch_async(dispatch_get_main_queue(), ^{
            // Actualizar interfaz gráfica después de haber encontrado la ruta óptima.
            NSMutableString *map_markers = [[NSMutableString alloc] init];
            for( int i=0; i<ciudades; i++ ) {
                if( !i ) {
                    [[self.outputTextView textStorage] appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"\nRuta Óptima: [%d", rutaOptima[i]]]];
                    [map_markers appendString:[NSString stringWithFormat:@"&markers=color:green%%7Clabel:%d%%7C%.4f,%.4f", i,
                                               latitudes[rutaOptima[i]], longitudes[rutaOptima[i]]]];
                }
                else if( i==ciudades-1 ) {
                    [[self.outputTextView textStorage] appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@", %d, %d]\n", rutaOptima[i], rutaOptima[0]]]];
                    [map_markers appendString:[NSString stringWithFormat:@"&markers=color:red%%7Csize:med%%7Clabel:%d%%7C%.4f,%.4f", i,
                                               latitudes[rutaOptima[i]], longitudes[rutaOptima[i]]]];
                }
                else {
                    [[self.outputTextView textStorage] appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@", %d", rutaOptima[i]]]];
                    [map_markers appendString:[NSString stringWithFormat:@"&markers=color:yellow%%7Csize:tiny%%7Clabel:%d%%7C%.2f,%.2f", i,
                                               latitudes[rutaOptima[i]], longitudes[rutaOptima[i]]]];
                }
            }
            [[self.outputTextView textStorage] appendAttributedString:[[NSAttributedString alloc] initWithString:[NSString stringWithFormat:@"Distancia total: %.2fkm\n", distanciaRuta(ciudades, rutaOptima, latitudes, longitudes)]]];
            NSString *url = [NSString stringWithFormat:@"%s%@&zoom=%d&center=%.2f,%.2f",
                             GOOGLE_MAP_URL, map_markers, self.zoomOptimo, self.centerLat, self.centerLong];
            [[self.webView mainFrame] loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:url]]];
            [self.algorithms setEnabled:YES];
            [self.startSolving setEnabled:YES];
            [self.browse setEnabled:YES];
            [self.progress stopAnimation:self.progress];
        });
    });
    while( !rutaOptima ) {
        dispatch_async(dispatch_get_main_queue(), ^{
            NSTimeInterval tiempo = [start timeIntervalSinceNow]*-1.0;
            [self.timeLabel setStringValue:[NSString stringWithFormat:@"%02d:%02d", (int)tiempo/60, (int)tiempo%60]];
        });
        usleep(500);
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView {
    return self.inputCities.count;
}

- (NSView*)tableView:(NSTableView *)tableView viewForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
    NSTextField *text = [[NSTextField alloc] initWithFrame:NSRectFromCGRect(CGRectMake(0, 0, tableColumn.width, tableView.rowHeight))];
    City *city = (City*)malloc(sizeof(City));
    NSValue *tmp = [self.inputCities objectAtIndex:row];
    [tmp getValue:&city];
    if( [[tableColumn identifier] isEqualToString:@"CityColumn"] )
        text.stringValue = [NSString stringWithUTF8String:city->nombre];
    else if( [[tableColumn identifier] isEqualToString:@"LatColumn"] )
        text.floatValue = city->latitud;
    else if( [[tableColumn identifier] isEqualToString:@"LongColumn"] )
        text.floatValue = city->longitud;
    else
        text.integerValue = row;
    [text setEditable:NO];
    return text;
}

@end
