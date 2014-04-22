//
//  PGAAppDelegate.h
//  geneticalgorithm
//
//  Created by Rafael Castro Salazar on 15/04/14.
//  Copyright (c) 2014 ITESM CCM. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WebView.h>

@interface PGAAppDelegate : NSObject <NSApplicationDelegate, NSTableViewDataSource, NSTableViewDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (nonatomic, retain) IBOutlet WebView *webView;
@property (nonatomic, retain) IBOutlet NSTextView *outputTextView;
@property (nonatomic, retain) IBOutlet NSTableView *citiesTableView;
@property (nonatomic, retain) IBOutlet NSTextField *inputFileLocation;
@property (nonatomic, retain) IBOutlet NSPopUpButton *algorithms;
@property (nonatomic, retain) IBOutlet NSButton *startSolving;
@property (nonatomic, retain) IBOutlet NSButton *browse;
@property (nonatomic, retain) IBOutlet NSTextField *timeLabel;
@property (nonatomic, retain) IBOutlet NSProgressIndicator *progress;
@property (retain) NSArray *algorithmNames;
@property (retain) NSMutableArray *inputCities;
@property int zoomOptimo;
@property float centerLat;
@property float centerLong;

- (IBAction)browseForFile:(id)sender;
- (IBAction)solve:(id)sender;
- (void)resolver:(long)indiceAlgoritmo;

@end
