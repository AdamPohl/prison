void QuitMainWindow (void)
{
   xv_destroy_safe(MainWindow);
}

void SelectAllDispoStrategies (void)
{
   int i, nbrows;
   
   nbrows = xv_get(ListDispo,PANEL_LIST_NROWS);
   
   for (i=0 ; i<nbrows ; i++) {
      xv_set(ListDispo,PANEL_LIST_SELECT,i,TRUE,NULL);
   }
}

void DeselectAllDispoStrategies (void)
{
   int i, nbrows;
   
   nbrows = xv_get(ListDispo,PANEL_LIST_NROWS);
   for (i=nbrows-1 ; i>=0 ; i--) {
      if (xv_get(ListDispo,PANEL_LIST_SELECTED,i)) {
         xv_set(ListDispo,PANEL_LIST_SELECT,i,FALSE,NULL);
      }
   }
}

void SelectAllSelecStrategies (void)
{
   int i, nbrows;
   
   nbrows = xv_get(ListSelec,PANEL_LIST_NROWS);
   
   for (i=0 ; i<nbrows ; i++) {
      xv_set(ListSelec,PANEL_LIST_SELECT,i,TRUE,NULL);
   }
}

void DeselectAllSelecStrategies (void)
{
   int i, nbrows;
   
   nbrows = xv_get(ListSelec,PANEL_LIST_NROWS);
   
   for (i=nbrows-1 ; i>=0 ; i--) {
      if (xv_get(ListSelec,PANEL_LIST_SELECTED,i)) {
         xv_set(ListSelec,PANEL_LIST_SELECT,i,FALSE,NULL);
      }
   }
}

void AddAllDispoStrategies (void)
{
   int i, nbrowsDispo, nbrowsSelec;
   
   nbrowsDispo = xv_get(ListDispo,PANEL_LIST_NROWS);
   for (i=0 ; i<nbrowsDispo ; i++) {
        nbrowsSelec = xv_get(ListSelec,PANEL_LIST_NROWS);
   	xv_set(ListSelec,
               PANEL_LIST_INSERT,nbrowsSelec,
   	       PANEL_LIST_STRING,nbrowsSelec,xv_get(ListDispo,PANEL_LIST_STRING,i),
   	       PANEL_LIST_CLIENT_DATA,nbrowsSelec,xv_get(ListDispo,PANEL_LIST_CLIENT_DATA,i),
   	       NULL);
   }
   NonRandomStrat = xv_get(ListSelec,PANEL_LIST_NROWS);
}

void AddDispoStrategies (void)
{
   int i, nbrowsDispo, nbrowsSelec;
   
   nbrowsDispo = xv_get(ListDispo,PANEL_LIST_NROWS);
   for (i=0 ; i<nbrowsDispo ; i++) {
        nbrowsSelec = xv_get(ListSelec,PANEL_LIST_NROWS);
   	if (xv_get(ListDispo,PANEL_LIST_SELECTED, i)) {
   	   xv_set(ListSelec,
   	          PANEL_LIST_INSERT,nbrowsSelec,
   	          PANEL_LIST_STRING,nbrowsSelec,xv_get(ListDispo,PANEL_LIST_STRING,i),
   	          PANEL_LIST_CLIENT_DATA,nbrowsSelec,xv_get(ListDispo,PANEL_LIST_CLIENT_DATA,i),
   	          NULL);
   	   NonRandomStrat = xv_get(ListSelec,PANEL_LIST_NROWS);
   	}
   }
   DeselectAllDispoStrategies();
}

void RemoveSelecStrategies (void)
{
   int i, nbrows;
   
   nbrows = xv_get(ListSelec,PANEL_LIST_NROWS);
   for (i=nbrows-1 ; i>=0 ; i--) {
   	if (xv_get(ListSelec,PANEL_LIST_SELECTED, i)) {
   	   xv_set(ListSelec,PANEL_LIST_DELETE,i,NULL);
   	   NonRandomStrat = xv_get(ListSelec,PANEL_LIST_NROWS);
   	}
   }
}

void RemoveAllSelecStrategies (void)
{
   int i, nbrows;
   
   NonRandomStrat = 0;
   nbrows = xv_get(ListSelec,PANEL_LIST_NROWS);
   for (i=nbrows-1 ; i>=0 ; i--) {
   	xv_set(ListSelec,PANEL_LIST_DELETE,i,NULL);
   }
}

void SelectStrategiesRandomly (void)
{
   int i, j, integer, nbrowsDispo, nbrowsSelec;
   int Sbetween, Sand, Srandom, Pbetween, Pand, Prandom;
   char *c, s[255], name[255];
   
   nbrowsSelec = xv_get(ListSelec,PANEL_LIST_NROWS);
   for (i=nbrowsSelec-1 ; i>= NonRandomStrat ; i--) {
   	xv_set(ListSelec,PANEL_LIST_DELETE,i,NULL);
   }
   
   nbrowsDispo=xv_get(ListDispo,PANEL_LIST_NROWS);
   Sbetween=xv_get(cStratBetween,PANEL_VALUE);
   Sand=xv_get(cStratAnd,PANEL_VALUE);
   Pbetween=xv_get(cPopBetween,PANEL_VALUE);
   Pand=xv_get(cPopAnd,PANEL_VALUE);
   
   if (Sbetween>Sand) { Srandom=Sbetween; Sbetween=Sand; Sand=Srandom; }
   if (Pbetween>Pand) { Prandom=Pbetween; Pbetween=Pand; Pand=Prandom; }
      
   if (Sbetween<Sand) {
      Srandom=rand()%(Sand-Sbetween+1)+Sbetween;
      while ((Srandom<Sbetween) || (Srandom>Sand)) Srandom=rand()%(Sand-Sbetween+1)+Sbetween;
   } else Srandom=Sbetween;
      
   for (i=0 ; i<Srandom ; i++) {
      nbrowsSelec = xv_get(ListSelec,PANEL_LIST_NROWS);
      j=rand()%nbrowsDispo;
      if (Pbetween<Pand) {
         Prandom=((rand()%(Pand-Pbetween+1)+Pbetween)/10)*10;
         while ((Prandom<Pbetween) || (Prandom>Pand)) Prandom=((rand()%(Pand-Pbetween+1)+Pbetween)/10)*10;
      } else Prandom=Pbetween;
      c=(char *) xv_get(ListDispo,PANEL_LIST_STRING,j);
      sscanf(c,"%d %s",&integer,name);
      sprintf(s,"%5d   %s",Prandom,name);
      xv_set(ListSelec,
             PANEL_LIST_INSERT,nbrowsSelec,
   	     PANEL_LIST_STRING,nbrowsSelec,s,
   	     PANEL_LIST_CLIENT_DATA,nbrowsSelec,xv_get(ListDispo,PANEL_LIST_CLIENT_DATA,j),
   	     NULL);
   }
}

void SetPopulation (void)
{
   int i, integer, nbrows, population;
   char *c, s[255], name[255];
   
   population=(int) xv_get(cPopulation,PANEL_VALUE);
   nbrows = xv_get(ListSelec,PANEL_LIST_NROWS);
   
   for (i=0 ; i<nbrows ; i++) {
      if (xv_get(ListSelec,PANEL_LIST_SELECTED, i)) {
         c=(char *) xv_get(ListSelec,PANEL_LIST_STRING,i);
         sscanf(c,"%d %s",&integer,name);
         sprintf(s,"%5d   %s",population,name);
         xv_set(ListSelec,PANEL_LIST_STRING,i,s,NULL);
      }
   }
   xv_set(cPopulation,PANEL_VALUE,100,NULL);
   DeselectAllSelecStrategies();
}

void LaunchGeneration (void)
{
   int i;
   int nbforeseen=0;
   int nbrowsSelec;
   int indice, population;
   char *c, s[255];
   
   nbrowsSelec = xv_get(ListSelec,PANEL_LIST_NROWS);
   
   if (nbrowsSelec==0) {
      notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "WARNING !!!",
                    "Impossible to run calculations",
                    "Any strategies have been selected",
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      return;
   }
   
   for (nbforeseen=1 ; FORESEEN[nbforeseen].adr!=NULL ; nbforeseen++);
   for (i=0 ; i<nbrowsSelec ; i++) {
      indice = xv_get(ListSelec,PANEL_LIST_CLIENT_DATA,i);
      c=(char *) xv_get(ListSelec,PANEL_LIST_STRING,i);
      sscanf(c,"%d %s",&population,s);
      RECOPSTRAT(indice,i+1,nbforeseen);
      STRATEGIES[i+1].qute=population;
   }
   RECOPSTRAT(nbforeseen,nbrowsSelec+1,nbforeseen);
   
   GENERATION();
   
   ReadData();
   
   CreateTraceWindow();
}

void OpenFile (File_chooser filechooser, char *filename)
{
   int i, j, ok, nbrows, nbrowsDispo, nbrowsSelec;
   char s[255], name[255], population[255], nameDispo[255], populationDispo[255], *str;
   FILE *f;
   
   RemoveAllSelecStrategies();
   
   nbrowsDispo = xv_get(ListDispo,PANEL_LIST_NROWS);
   
   f=fopen(filename,"r");
   if (f==NULL) {
      sprintf(s,"Unable to open file %s",filename);
      notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "ERROR !!!",
                    s,
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      return;
   }
   
   fscanf(f,"%s",s);
   if (strcmp(s,"IDP_CONFIGURATION_FILE")) {
      notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "ERROR !!!",
                    "Not a valid configuration file",
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      return;
   }
   fscanf(f,"%s",s);
   if (strcmp(s,"Do_not_edit_it_by_hand")) {
      notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "ERROR !!!",
                    "Not a valid configuration file",
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      return;
   }
   
   fscanf(f,"%s %d",s,&R);
   fscanf(f,"%s %d",s,&S);
   fscanf(f,"%s %d",s,&T);
   fscanf(f,"%s %d",s,&P);
   fscanf(f,"%s %d",s,&N);
   fscanf(f,"%s %d",s,&GAME_LENGTH);
   fscanf(f,"%s %d",s,&NB_GEN);
   fscanf(f,"%s %d",s,&REPEAT);
   fscanf(f,"%s %d",s,&SEED);
   fscanf(f,"%s %d",s,&LOSS);
   fscanf(f,"%s %d",s,&NOISE);
   fscanf(f,"%s %d",s,&nbrows);
   for (i=0 ; i<nbrows ; i++) {
      fscanf(f,"%s %s",population,name);
      ok=0;
      for (j=0 ; j<nbrowsDispo ; j++) {
         str=(char *) xv_get(ListDispo,PANEL_LIST_STRING,j);
         sscanf(str,"%s %s",populationDispo,nameDispo);
         if (!strcmp(name,nameDispo)) {
            nbrowsSelec = xv_get(ListSelec,PANEL_LIST_NROWS);
            sprintf(s,"%5s   %s",population,name);
   	    xv_set(ListSelec,
               PANEL_LIST_INSERT,nbrowsSelec,
   	       PANEL_LIST_STRING,nbrowsSelec,s,
   	       PANEL_LIST_CLIENT_DATA,nbrowsSelec,xv_get(ListDispo,PANEL_LIST_CLIENT_DATA,j),
   	       NULL);   
   	    ok=1;
         }
      }
      if (!ok) {
         sprintf(s,"Strategy %s isn't in Authorized Strategies",name);
         notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "WARNING !!!",
                    s,
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      }  
   }
   
   if (R<0) R=3;
   if (S<0) S=0;
   if (T<0) T=5;
   if (P<0) P=1;
   if (N<0) N=2;
   if (GAME_LENGTH<0) GAME_LENGTH=MAX_PART;
   if (NB_GEN>MAX_GEN) NB_GEN=MAX_GEN;
   if (REPEAT<0) REPEAT=8;
   if (NB_GEN<0) NB_GEN=0;  /* stop at stabilization */
   if (SEED<0) SEED=0;
   if (LOSS<0) LOSS=0;
   if (NOISE<0) NOISE=0;
   
   if (SetupWindow) {
      xv_set(cR,PANEL_VALUE,R,NULL);
      xv_set(cP,PANEL_VALUE,P,NULL);
      xv_set(cT,PANEL_VALUE,T,NULL);
      xv_set(cN,PANEL_VALUE,N,NULL);
      xv_set(cS,PANEL_VALUE,S,NULL);
      xv_set(cGameLength,PANEL_VALUE,GAME_LENGTH,NULL);
      xv_set(cRepeat,PANEL_VALUE,REPEAT,NULL);
      xv_set(cNbGen,PANEL_VALUE,NB_GEN,NULL);
      xv_set(cSeed,PANEL_VALUE,SEED,NULL);
      xv_set(cLoss,PANEL_VALUE,LOSS,NULL);
      xv_set(cNoise,PANEL_VALUE,NOISE,NULL);
   }
   
   fclose(f);
   xv_set(OpenFileDialog,XV_SHOW,FALSE,NULL);
}

void SaveFile (File_chooser filechooser, char *filename)
{
   int i, nbrows;
   char s[255];
   FILE *f;
   
   nbrows = xv_get(ListSelec,PANEL_LIST_NROWS);
   
   f=fopen(filename,"w");
   if (f==NULL) {
      sprintf(s,"Unable to open file %s",filename);
      notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "ERROR !!!",
                    s,
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      return;
   }
   
   fprintf(f,"IDP_CONFIGURATION_FILE\n");
   fprintf(f,"Do_not_edit_it_by_hand\n");
   fprintf(f,"R %d\n",R);
   fprintf(f,"S %d\n",S);
   fprintf(f,"T %d\n",T);
   fprintf(f,"P %d\n",P);
   fprintf(f,"N %d\n",N);
   fprintf(f,"GAME_LENGHT %d\n",GAME_LENGTH);
   fprintf(f,"NB_GEN %d\n",NB_GEN);
   fprintf(f,"REPEAT %d\n",REPEAT);
   fprintf(f,"SEED %d\n",SEED);
   fprintf(f,"LOOS %d\n",LOSS);
   fprintf(f,"NOISE %d\n",NOISE);
   fprintf(f,"NbStrategies %d\n",nbrows);
   for (i=0 ; i<nbrows ; i++) {
      fprintf(f,"%s\n",xv_get(ListSelec,PANEL_LIST_STRING,i));
   }
   fprintf(f,"END_CONFIGURATION_FILE\n");
   
   fclose(f);
   xv_set(SaveFileDialog,XV_SHOW,FALSE,NULL);
}

void ShowOpenFileDialog (void)
{
   xv_set(OpenFileDialog,XV_SHOW,TRUE,NULL);
}

void ShowSaveFileDialog (void)
{
   xv_set(SaveFileDialog,XV_SHOW,TRUE,NULL);
}

void CreateMainWindow (int argc, char **argv) 
{
   int i;
   char s[255];
   
   if (argc==2 && !strcmp(argv[1],"-mono")) {
      WhatScreen=MONOSCREEN;
   }
   
   xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);
   
   MainIcon = xv_create(XV_NULL,SERVER_IMAGE,
                        SERVER_IMAGE_DEPTH,1,
                        SERVER_IMAGE_X_BITS,main_bits,
                        XV_WIDTH,main_width,
                        XV_HEIGHT,main_height,
                        NULL);
   
   MainWindow = (Frame) xv_create ((Frame) NULL,FRAME, 
                                   XV_WIDTH,640, 
                                   XV_HEIGHT,480,
                                   XV_LABEL,"Iterated Prisoner's Dilemma",
                                   FRAME_SHOW_FOOTER,TRUE, 
                                   FRAME_SHOW_RESIZE_CORNER,FALSE,
                                   FRAME_ICON,xv_create(XV_NULL,ICON,ICON_IMAGE,MainIcon,NULL),
                                   NULL);
   
   MainControl = (Panel) xv_create (MainWindow, PANEL,
                                    XV_X, 0, XV_Y, 0, 
                                    XV_WIDTH, WIN_EXTEND_TO_EDGE, 
                                    XV_HEIGHT, WIN_EXTEND_TO_EDGE,
                                    WIN_BORDER, FALSE,
                                    NULL);
                                
   ListSelec = (Panel) xv_create (MainControl,PANEL_LIST,
                                  XV_X,410,XV_Y,10,
                                  PANEL_LIST_WIDTH,200, 
                                  PANEL_LIST_DISPLAY_ROWS,19, 
                                  PANEL_LIST_TITLE,"Selected Strategies",
                                  PANEL_LAYOUT,PANEL_HORIZONTAL,
                                  PANEL_READ_ONLY,FALSE,
                                  PANEL_CHOOSE_ONE,FALSE, 
                                  PANEL_CHOOSE_NONE,TRUE,
                                  NULL);
   
   ListDispo = (Panel) xv_create (MainControl,PANEL_LIST,
                                  XV_X,10,XV_Y,10,
                                  PANEL_LIST_WIDTH,200, 
                                  PANEL_LIST_DISPLAY_ROWS,19, 
                                  PANEL_LIST_TITLE,"Authorized Strategies",
                                  PANEL_LAYOUT,PANEL_HORIZONTAL,
                                  PANEL_READ_ONLY,FALSE,
                                  PANEL_CHOOSE_ONE,FALSE, 
                                  PANEL_CHOOSE_NONE,TRUE,
                                  NULL);
                                
   for (i=1 ; FORESEEN[i].adr != NULL; i++) {
      sprintf(s,"%5d   %s",FORESEEN[i].qute,FORESEEN[i].name);
      xv_set(ListDispo,
   	     PANEL_LIST_INSERT,(i-1),
   	     PANEL_LIST_STRING,(i-1),s,
   	     PANEL_LIST_CLIENT_DATA,(i-1),i,
   	     NULL);
   }
   
   (void) xv_create (MainControl,PANEL_MESSAGE,
                     XV_X,294,XV_Y,30,
                     PANEL_LABEL_STRING,"Add All",
                     PANEL_LABEL_BOLD,TRUE,
                     NULL);
                             
   (void) xv_create (MainControl,PANEL_BUTTON,
                     XV_X,290,XV_Y,50,
                     PANEL_LABEL_WIDTH,40, 
                     PANEL_LABEL_STRING,">>>>>",
                     PANEL_NOTIFY_PROC,AddAllDispoStrategies,
                     NULL);
                                  
   (void) xv_create (MainControl,PANEL_MESSAGE,
                     XV_X,305,XV_Y, 80,
                     PANEL_LABEL_STRING,"Add",
                     PANEL_LABEL_BOLD,TRUE,
                     NULL);
                     
   (void) xv_create (MainControl,PANEL_BUTTON,
                     XV_X,290,XV_Y,100,
                     PANEL_LABEL_WIDTH,40, 
                     PANEL_LABEL_STRING,"  >>>",
                     PANEL_NOTIFY_PROC,AddDispoStrategies,
                     NULL);
   
   (void) xv_create (MainControl,PANEL_MESSAGE,
                     XV_X,289,XV_Y,130,
                     PANEL_LABEL_STRING,"Remove",
                     PANEL_LABEL_BOLD,TRUE,
                     NULL);
                                                    
   (void) xv_create (MainControl,PANEL_BUTTON,
                     XV_X,290,XV_Y,150,
                     PANEL_LABEL_WIDTH,40,
                     PANEL_LABEL_STRING,"  <<<",
                     PANEL_NOTIFY_PROC,RemoveSelecStrategies,
                     NULL);
   
   (void) xv_create (MainControl,PANEL_MESSAGE,
                     XV_X,278,XV_Y,180,
                     PANEL_LABEL_STRING,"Remove All",
                     PANEL_LABEL_BOLD,TRUE,
                     NULL);
                     
   (void) xv_create (MainControl,PANEL_BUTTON,
                     XV_X,290,XV_Y,200,
                     PANEL_LABEL_WIDTH,40, 
                     PANEL_LABEL_STRING,"<<<<<",
                     PANEL_NOTIFY_PROC,RemoveAllSelecStrategies,
                     NULL);
                     
   (void) xv_create (MainControl,PANEL_BUTTON,
                     XV_X,255,XV_Y,230,
                     PANEL_LABEL_WIDTH,115,
                     PANEL_LABEL_STRING,"Random Selection",
                     PANEL_NOTIFY_PROC,SelectStrategiesRandomly,
                     NULL);
                     
   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,305,XV_Y,260, 
                    PANEL_LABEL_STRING,"With", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                    
   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,240,XV_Y,280, 
                    PANEL_LABEL_STRING,"Number of Strategies", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                     
   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,240,XV_Y,300, 
                    PANEL_LABEL_STRING,"Between", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);

   cStratBetween = (Panel) xv_create(MainControl,PANEL_NUMERIC_TEXT, 
                                     XV_X,320,XV_Y,300, 
                                     PANEL_VALUE_DISPLAY_LENGTH,2, 
                                     PANEL_LAYOUT,PANEL_HORIZONTAL,
                                     PANEL_MAX_VALUE,NBSTRATMAX, 
                                     PANEL_MIN_VALUE,1, 
                                     PANEL_VALUE,5,
                                     PANEL_READ_ONLY,FALSE,
                                     NULL);
                          
   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,240,XV_Y,320, 
                    PANEL_LABEL_STRING,"And", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);

   cStratAnd = (Panel) xv_create(MainControl,PANEL_NUMERIC_TEXT, 
                                 XV_X,320,XV_Y,320, 
                                 PANEL_VALUE_DISPLAY_LENGTH,2, 
                                 PANEL_LAYOUT,PANEL_HORIZONTAL,
                                 PANEL_MAX_VALUE,NBSTRATMAX, 
                                 PANEL_MIN_VALUE,1, 
                                 PANEL_VALUE,10,
                                 PANEL_READ_ONLY,FALSE,
                                 NULL);

   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,310,XV_Y,340, 
                    PANEL_LABEL_STRING,"And", 
                    PANEL_LABEL_BOLD,TRUE,
                    NULL);
                           
   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,240,XV_Y,360, 
                    PANEL_LABEL_STRING,"Population of Strategies", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                     
   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,240,XV_Y,380, 
                    PANEL_LABEL_STRING,"Between", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);

   cPopBetween = (Panel) xv_create(MainControl,PANEL_NUMERIC_TEXT, 
                                     XV_X,320,XV_Y,380, 
                                     PANEL_VALUE_DISPLAY_LENGTH,2, 
                                     PANEL_LAYOUT,PANEL_HORIZONTAL,
                                     PANEL_MAX_VALUE,999, 
                                     PANEL_MIN_VALUE,1, 
                                     PANEL_VALUE,100,
                                     PANEL_READ_ONLY,FALSE,
                                     NULL);
                          
   (void) xv_create(MainControl,PANEL_MESSAGE, 
                    XV_X,240,XV_Y,400, 
                    PANEL_LABEL_STRING,"And", 
                    PANEL_LABEL_BOLD,TRUE,
                    NULL);

   cPopAnd = (Panel) xv_create(MainControl,PANEL_NUMERIC_TEXT, 
                                 XV_X,320,XV_Y,400, 
                                 PANEL_VALUE_DISPLAY_LENGTH,2, 
                                 PANEL_LAYOUT,PANEL_HORIZONTAL,
                                 PANEL_MAX_VALUE,999, 
                                 PANEL_MIN_VALUE,1, 
                                 PANEL_VALUE,100,
                                 PANEL_READ_ONLY,FALSE,
                                 NULL);                        
   
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,10,XV_Y, 400,
                    PANEL_LABEL_WIDTH,71, 
                    PANEL_LABEL_STRING,"  Select All",
                    PANEL_NOTIFY_PROC,SelectAllDispoStrategies,
                    NULL);
   
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,120,XV_Y,400,
                    PANEL_LABEL_WIDTH,71, 
                    PANEL_LABEL_STRING,"Deselect All",
                    PANEL_NOTIFY_PROC,DeselectAllDispoStrategies,
                    NULL);
                    
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,410,XV_Y,430,
                    PANEL_LABEL_WIDTH,90, 
                    PANEL_LABEL_STRING,"Set Population",
                    PANEL_NOTIFY_PROC,SetPopulation,
                    NULL);
   
   cPopulation = (Panel) xv_create(MainControl,PANEL_NUMERIC_TEXT, 
                                   XV_X,530,XV_Y,430, 
                                   PANEL_VALUE_DISPLAY_LENGTH,4, 
                                   PANEL_VALUE_STORED_LENGTH,80, 
                                   PANEL_LAYOUT,PANEL_HORIZONTAL,
                                   PANEL_MAX_VALUE,1000, 
                                   PANEL_MIN_VALUE,1, 
                                   PANEL_VALUE,100,
                                   PANEL_READ_ONLY,FALSE,
                                   NULL);
   
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,410,XV_Y,400,
                    PANEL_LABEL_WIDTH,71, 
                    PANEL_LABEL_STRING,"  Select All",
                    PANEL_NOTIFY_PROC,SelectAllSelecStrategies,
                    NULL);
   
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,520,XV_Y,400,
                    PANEL_LABEL_WIDTH,71, 
                    PANEL_LABEL_STRING,"Deselect All",
                    PANEL_NOTIFY_PROC,DeselectAllSelecStrategies,
                    NULL);
   
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,10,XV_Y,460,
                    PANEL_LABEL_WIDTH,40, 
                    PANEL_LABEL_STRING,"  Run", 
                    PANEL_NOTIFY_PROC,LaunchGeneration,
                    NULL);
                    
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,140,XV_Y,460,
                    PANEL_LABEL_WIDTH,40, 
                    PANEL_LABEL_STRING," Load",
                    PANEL_NOTIFY_PROC, ShowOpenFileDialog,
                    NULL);
                                 
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,290,XV_Y,460,
                    PANEL_LABEL_WIDTH,40, 
                    PANEL_LABEL_STRING,"Setup",
                    PANEL_NOTIFY_PROC,CreateSetupWindow,
                    NULL);
                    
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,440,XV_Y,460,
                    PANEL_LABEL_WIDTH,40, 
                    PANEL_LABEL_STRING," Save",
                    PANEL_NOTIFY_PROC,ShowSaveFileDialog,
                    NULL);
   
   (void) xv_create(MainControl,PANEL_BUTTON,
                    XV_X,570,XV_Y,460,
                    PANEL_LABEL_WIDTH,40, 
                    PANEL_LABEL_STRING,"  Quit",
                    PANEL_NOTIFY_PROC,QuitMainWindow,
                    NULL);  
   
   OpenFileDialog = (File_chooser) xv_create(MainWindow,FILE_CHOOSER_OPEN_DIALOG,
                                             XV_SHOW,FALSE,
                                             FILE_CHOOSER_NOTIFY_FUNC,OpenFile,
                                             NULL);
                                             
   SaveFileDialog = (File_chooser) xv_create(MainWindow,FILE_CHOOSER_SAVE_DIALOG,
                                             XV_SHOW,FALSE,
                                             FILE_CHOOSER_NOTIFY_FUNC,SaveFile,
                                             NULL);
   
}
