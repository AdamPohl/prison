void ReadData (void)
{
   FILE *file;
   int i, j, integer, pos=0;
   char s[255];
      
   MinVal=100;
   MaxVal=100;
   
   file=fopen(RESULT_DAT,"r");
   if (file==NULL) {
      notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "ERROR !!!",
                    "Impossible to open",
                    "data file",
                    RESULT_DAT,
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      return;
   }
   
   if (tab!=NULL) free(tab);
   tab=(int *) malloc (sizeof(int) * (NBSTRAT+1) * (LAST+1));
   if (tab==NULL) {
      notice_prompt(MainControl,NULL,
                    NOTICE_MESSAGE_STRINGS,
                    "ERROR !!!",
                    "Impossible to allocate",
                    "enough memory to read data",
                    NULL,
                    NOTICE_BUTTON_YES,"OK",
                    NULL);
      fclose(file);
      return;
   }

   /* On passe les noms des strategies */   
   for (i=0 ; i<NBSTRAT ; i++) {
      fscanf(file,"%s",&s);
   }
   
   /* On lit les donnees, le premier fscanf elimine l indice */
   for (i=0 ; i<=LAST ; i++) {
      fscanf(file,"%d",&integer);
      for (j=0 ; j<(NBSTRAT + 1) ; j++) {
         fscanf(file,"%d",&integer);
         if ((integer>MaxVal) && (j!=NBSTRAT)) MaxVal=integer;
         if ((integer<MinVal) && (j!=NBSTRAT)) MinVal=integer;
         tab[pos]=integer;
         pos++;
      }
   }
      
   fclose(file);
}

void MyEventProc (window, event, arg)
Xv_Window window;
Event *event;
Notify_arg arg;
{
   int i, deltaX, deltaYS, deltaYC, mouseX, mouseY, tabX, tabY;
   char s[255];
   
   if (TracingMethod == GRID) {
      switch (event_action(event)) {
         case LOC_MOVE:
            deltaX=xv_get(TraceCanvas,XV_WIDTH);
            deltaYC=xv_get(TraceCanvas,XV_HEIGHT);
            deltaYS= 2 * deltaYC / 3;
            mouseX=event_x(event);
            if (mouseX>=30 && mouseX<=(deltaX-100) && deltaX>130) {
               mouseY=event_y(event);
               tabX=(mouseX - 30) * LAST / (deltaX - 130);
               sprintf(s,"Generation: %d",tabX);
               xv_set(mGeneration,PANEL_LABEL_STRING,s,NULL);
               if (mouseY>=10 && mouseY<=(deltaYS-25) && deltaYS>50) {
                  tabY=(deltaYS - mouseY - 25) * (MaxVal - MinVal) / (deltaYS - 50);
                  sprintf(s,"Population: %d",tabY);
                  xv_set(mPopulation,PANEL_LABEL_STRING,s,NULL);
               }
            }
            break;
         default:break;
      }
   }
}

Notify_value TraceGraph (void)
{
   int i, j, indice, colorindice, popTotale=0, lastAngle=0;   

   if (LineWidth != xv_get(SliderWidth,PANEL_VALUE)) {
      LineWidth = xv_get(SliderWidth,PANEL_VALUE);
      XSetLineAttributes(dpy,gcblack,LineWidth,LineSolid,CapRound,JoinRound);
      for (i=0 ; i<15 ; i++) {
         XSetLineAttributes(dpy,gccolors[i],LineWidth,LineSolid,CapRound,JoinRound);
      }
   }
   if (vitesse != xv_get(SliderSpeed,PANEL_VALUE)) {
      vitesse = xv_get(SliderSpeed,PANEL_VALUE);
      timer.it_value.tv_usec = 500000/vitesse;
      timer.it_interval.tv_usec = 500000/vitesse;
      notify_set_itimer_func(TraceWindow,TraceGraph,ITIMER_REAL,&timer,NULL);
   }

   if (CurrentIndice>LAST) {
      notify_set_itimer_func(TraceWindow,NOTIFY_FUNC_NULL,ITIMER_REAL,&timer,NULL);
      return;
   }
      
   currentX = ((deltaX - 130) * CurrentIndice) / LAST + 30;
   lastX = ((deltaX - 130) * (CurrentIndice-1)) / LAST + 30;
   colorindice=0;
   if (TracingMethod != GRID) {
      for (j=0 ; j<NBSTRAT ; j++) popTotale+=tab[CurrentIndice*(NBSTRAT+1)+j];
      XFillArc(dpy,TraceXWindow,gcwhite,10,10,deltaX-130,deltaYS-25,0,360*64);
      XDrawArc(dpy,TraceXWindow,gcwhite,10,10,deltaX-130,deltaYS-25,0,360*64);
   }
   for (j=0 ; j<NBSTRAT ; j++) {
      if (xv_get(ListTrace,PANEL_LIST_SELECTED,j)) {
         indice=xv_get(ListTrace,PANEL_LIST_CLIENT_DATA,j);
         switch (TracingMethod) {
            case GRID:
               currentY=deltaYS-((deltaYS - 50)*(tab[CurrentIndice*(NBSTRAT+1)+indice]-MinVal))/(MaxVal-MinVal)-25; 
               lastY=deltaYS-((deltaYS-50)*(tab[(CurrentIndice-1)*(NBSTRAT+1)+indice]-MinVal))/(MaxVal-MinVal)-25;
               XDrawLine(dpy,TraceXWindow,gccolors[colorindice],currentX,currentY,lastX,lastY);
               break;
            case PIE:
               XFillArc(dpy,TraceXWindow,gccolors[colorindice],10,10,deltaX-130,deltaYS-25,
                        lastAngle,(23040*tab[(CurrentIndice-1)*(NBSTRAT+1)+indice])/popTotale);
               XDrawArc(dpy,TraceXWindow,gcblack,10,10,deltaX-130,deltaYS-25,
                        lastAngle,(23040*tab[(CurrentIndice-1)*(NBSTRAT+1)+indice])/popTotale);
               lastAngle+=(23040*tab[(CurrentIndice-1)*(NBSTRAT+1)+indice])/popTotale;
               break;
            default:
               break;
         }
      }      
      colorindice=(colorindice + 1) % 15;
   }
   if (CurrentIndice>1) {
      currentY = deltaYC - ((deltaYC - deltaYS - 50) * tab[CurrentIndice*(NBSTRAT+1)+NBSTRAT]) / 100 - 25; 
      lastY = deltaYC - ((deltaYC - deltaYS - 50) * tab[(CurrentIndice-1)*(NBSTRAT+1)+NBSTRAT]) / 100 - 25;
      XDrawLine(dpy,TraceXWindow,gcblack,currentX,currentY,lastX,lastY);
   }
   XFlush(dpy);
   CurrentIndice++;
}

void StopTrace (void)
{
   notify_set_itimer_func(TraceWindow,NOTIFY_FUNC_NULL,ITIMER_REAL,&timer,NULL);
}

void ContinueTrace (void)
{  
   int i;

   if (LineWidth != xv_get(SliderWidth,PANEL_VALUE)) {
      LineWidth = xv_get(SliderWidth,PANEL_VALUE);
      XSetLineAttributes(dpy,gcblack,LineWidth,LineSolid,CapRound,JoinRound);
      for (i=0 ; i<15 ; i++) {
         XSetLineAttributes(dpy,gccolors[i],LineWidth,LineSolid,CapRound,JoinRound);
      }
   }
   
   vitesse = xv_get(SliderSpeed,PANEL_VALUE);
   timer.it_value.tv_usec = 500000/vitesse;
   timer.it_interval.tv_usec = 500000/vitesse;
   
   notify_set_itimer_func(TraceWindow,TraceGraph,ITIMER_REAL,&timer,NULL);
}

void RepaintGraph (void)
{
   int i, j, k1, k2, indice, colorindice;
   char s[255];
   
   TraceXWindow=xv_get(canvas_paint_window(TraceCanvas),XV_XID);
   XClearWindow(dpy,TraceXWindow);
   XFlush(dpy);
      
   deltaX=xv_get(TraceCanvas,XV_WIDTH);
   deltaYC=xv_get(TraceCanvas,XV_HEIGHT);
   deltaYS= 2 * deltaYC / 3;
   
   if (MinVal==MaxVal) MinVal=0;
   
   /* Noms des strategies */
   colorindice=0;
   delta=0;
   for (i=0 ; i<NBSTRAT ; i++) {
      if (xv_get(ListTrace,PANEL_LIST_SELECTED,i)) {
         delta+=10;
         sprintf(s,"%s",xv_get(ListTrace,PANEL_LIST_STRING,i));
         XDrawString(dpy,TraceXWindow,gcblack,deltaX-95,delta,s,strlen(s));
         XSetLineAttributes(dpy,gccolors[colorindice],2,LineSolid,CapNotLast,JoinMiter);
         XDrawLine(dpy,TraceXWindow,gccolors[colorindice],deltaX-15,delta-5,deltaX-5,delta-5);
         XSetLineAttributes(dpy,gccolors[colorindice],1,LineSolid,CapNotLast,JoinMiter);
      }
      colorindice=(colorindice + 1) % 15;
   }
   sprintf(s,"Cooperation");
   XDrawString(dpy,TraceXWindow,gcblack,deltaX-95,(deltaYC-(deltaYC-deltaYS)/2),s,strlen(s));
   
   /* Largeur des courbes */
   XSetLineAttributes(dpy,gcblack,xv_get(SliderWidth,PANEL_VALUE),LineSolid,CapRound,JoinRound);
   for (i=0 ; i<15 ; i++) {
         XSetLineAttributes(dpy,gccolors[i],xv_get(SliderWidth,PANEL_VALUE),LineSolid,CapRound,JoinRound);
   }
      
   /* Axe des Abscisses */
   delta=LAST/10;
   if (delta==0) delta=1;
   for (i=0 ; i<=LAST ; i+=delta) {
      currentX = ((deltaX - 130) * i) / LAST + 30;
      /* Graphe de la courbe des strategies */
      if (TracingMethod == GRID) {   
         XDrawLine(dpy,TraceXWindow,gcblack,currentX,deltaYS-25,currentX,deltaYS-22);
         sprintf(s,"%d",i);
         XDrawString(dpy,TraceXWindow,gcblack,currentX,deltaYS-5,s,strlen(s));
      }
      
      /* Graphe de la courbe de cooperation */
      XDrawLine(dpy,TraceXWindow,gcblack,currentX,deltaYC-25,currentX,deltaYC-22);
      sprintf(s,"%d",i);
      XDrawString(dpy,TraceXWindow,gcblack,currentX,deltaYC-5,s,strlen(s));
      
      /* Lignes pointillees */
      if (GridOn==TRUE) {
         if (TracingMethod == GRID) XDrawLine(dpy,TraceXWindow,gcgray,currentX,10,currentX,deltaYS-25);
         XDrawLine(dpy,TraceXWindow,gcgray,currentX,deltaYS+10,currentX,deltaYC-25);
      }
   }
         
   /* Axe des Ordonnees (Courbe des strategies) */
   if (TracingMethod == GRID) {
      delta=(MaxVal - MinVal)/10;
      if (delta==0) delta=1;
      for (i=0 ; i<=(MaxVal - MinVal) ; i+=delta) {
         currentY = deltaYS - ((deltaYS - 50) * i) / (MaxVal - MinVal) - 25;
         XDrawLine(dpy,TraceXWindow,gcblack,30,currentY,27,currentY);
         sprintf(s,"%d",i+MinVal);
         XDrawString(dpy,TraceXWindow,gcblack,5,currentY,s,strlen(s));
         /* Lignes pointillees */
         if (GridOn==TRUE) XDrawLine(dpy,TraceXWindow,gcgray,30,currentY,deltaX-100,currentY);
      }
   }
   
   /* Axe des Ordonnees (Courbe de la cooperation)*/
   delta=10;
   for (i=0 ; i<=100 ; i+=delta) {
      currentY = deltaYC - ((deltaYC - deltaYS - 50) * i) / 100 - 25;
      XDrawLine(dpy,TraceXWindow,gcblack,30,currentY,27,currentY);
      sprintf(s,"%d",i);
      XDrawString(dpy,TraceXWindow,gcblack,5,currentY,s,strlen(s));
      /* Lignes pointillees */
      if (GridOn==TRUE) XDrawLine(dpy,TraceXWindow,gcgray,30,currentY,deltaX-100,currentY);
   }
   
   if (TracingMethod == GRID) {
      XDrawLine(dpy,TraceXWindow,gcblack,30,deltaYS-25,deltaX-100,deltaYS-25);
      XDrawLine(dpy,TraceXWindow,gcblack,30,10,deltaX-100,10);
      XDrawLine(dpy,TraceXWindow,gcblack,30,10,30,deltaYS-25);
      XDrawLine(dpy,TraceXWindow,gcblack,deltaX-100,10,deltaX-100,deltaYS-25);
   }

   XDrawLine(dpy,TraceXWindow,gcblack,30,deltaYC-25,deltaX-100,deltaYC-25);
   XDrawLine(dpy,TraceXWindow,gcblack,30,deltaYS+10,deltaX-100,deltaYS+10);
   XDrawLine(dpy,TraceXWindow,gcblack,30,deltaYS+10,30,deltaYC-25);
   XDrawLine(dpy,TraceXWindow,gcblack,deltaX-100,deltaYS+10,deltaX-100,deltaYC-25);
      
   CurrentIndice = 1;
   
   vitesse = xv_get(SliderSpeed,PANEL_VALUE);
   timer.it_value.tv_usec = 500000/vitesse;
   timer.it_interval.tv_usec = 500000/vitesse;
   notify_set_itimer_func(TraceWindow,TraceGraph,ITIMER_REAL,&timer,NULL);
      
   XFlush(dpy);
}

void ResizeGraph (void)
{
   newHeight = (int) xv_get(TraceWindow,XV_HEIGHT);
   newWidth = (int) xv_get(TraceWindow,XV_WIDTH);
   
   if ((newHeight<=maxHeight) && (newWidth<=maxWidth)) {
      RepaintGraph();
   } else {
      if (newHeight>maxHeight) maxHeight=newHeight;
      if (newWidth>maxWidth) maxWidth=newWidth;
   }
}

void SelectAllTraceStrategies (void)
{
   int i, nbrows;
   
   nbrows = xv_get(ListTrace,PANEL_LIST_NROWS);
   
   for (i=0 ; i<nbrows ; i++) {
      xv_set(ListTrace,PANEL_LIST_SELECT,i,TRUE,NULL);
   }
}

void DeselectAllTraceStrategies (void)
{
   int i, nbrows;
   
   nbrows = xv_get(ListTrace,PANEL_LIST_NROWS);
   
   for (i=nbrows-1 ; i>=0 ; i--) {
      if (xv_get(ListTrace,PANEL_LIST_SELECTED,i)) {
         xv_set(ListTrace,PANEL_LIST_SELECT,i,FALSE,NULL);
      }
   }
}

void ShowGrid (void)
{
   GridOn = !GridOn;
}

void ChangeTracingMethod (void)
{
   TracingMethod=xv_get(ChoiceMethod,PANEL_VALUE);
   RepaintGraph();
}

void QuitTrace (void)
{
   xv_set(TraceWindow,XV_SHOW,FALSE,NULL);
   xv_set(MainWindow,FRAME_CLOSED,FALSE,NULL);
}

void InitContextAndColor (void)
{
   int i;
   Colormap cmap;
   XColor color;

   dpy=(Display *) xv_get(TraceCanvas,XV_DISPLAY);
   screen=DefaultScreen(dpy);
   cmap=XDefaultColormap(dpy,screen);
   
   black=BlackPixel(dpy,screen);
   white=WhitePixel(dpy,screen);
   
   gcblack=XCreateGC(dpy,RootWindow(dpy,screen),0,0);
   XSetForeground(dpy,gcblack,black);
   XSetBackground(dpy,gcblack,white);
   XSetLineAttributes(dpy,gcblack,1,LineSolid,CapRound,JoinRound);
   
   gcwhite=XCreateGC(dpy,RootWindow(dpy,screen),0,0);
   XSetForeground(dpy,gcwhite,white);
   XSetBackground(dpy,gcwhite,white);
   XSetLineAttributes(dpy,gcwhite,1,LineSolid,CapRound,JoinRound);
     
   if (WhatScreen==COLORSCREEN) {
      color.red = 256 * 150;
      color.green = 256 * 150;
      color.blue = 256 * 150;
      XAllocColor(dpy,cmap,&color);
      gcgray=XCreateGC(dpy,RootWindow(dpy,screen),0,0);
      XSetForeground(dpy,gcgray,color.pixel);
      XSetBackground(dpy,gcgray,white);
      XSetLineAttributes(dpy,gcgray,1,LineOnOffDash,CapRound,JoinRound);

      /* Allocation des couleurs */
      for (i=0 ; i<15 ; i++) {
         color.red = 256 * colors[i].red;
         color.green = 256 * colors[i].green;
         color.blue = 256 * colors[i].blue;
         XAllocColor(dpy,cmap,&color);
         gccolors[i]=XCreateGC(dpy,RootWindow(dpy,screen),0,0);
         XSetForeground(dpy,gccolors[i],color.pixel);
         XSetBackground(dpy,gccolors[i],white);
         XSetLineAttributes(dpy,gccolors[i],1,LineSolid,CapRound,JoinRound);
      }
   } else {
      /* WhatScreen==MONOSCREEN */
      
      gcgray=XCreateGC(dpy,RootWindow(dpy,screen),0,0);
      XSetForeground(dpy,gcgray,black);
      XSetBackground(dpy,gcgray,white);
      XSetLineAttributes(dpy,gcgray,1,LineOnOffDash,CapRound,JoinRound);

      for (i=0 ; i<15 ; i++) {
         gccolors[i]=XCreateGC(dpy,RootWindow(dpy,screen),0,0);
         XSetForeground(dpy,gccolors[i],black);
         XSetBackground(dpy,gccolors[i],white);
         XSetLineAttributes(dpy,gccolors[i],1,LineSolid,CapRound,JoinRound);
      }
   }
}

void CreateTraceWindow (void)
{
   int i, nbrows;

   if (!TraceWindow) {
   
      TraceIcon = xv_create(XV_NULL,SERVER_IMAGE,
                            SERVER_IMAGE_DEPTH,1,
                            SERVER_IMAGE_X_BITS,trace_bits,
                            XV_WIDTH,trace_width,
                            XV_HEIGHT,trace_height,
                            NULL);
    
      TraceWindow = (Frame) xv_create(MainWindow,FRAME,
                                      XV_WIDTH,640,XV_HEIGHT,510, 
  		                      XV_LABEL,"Trace Window", 
  		                      XV_SHOW,TRUE,
		                      FRAME_SHOW_FOOTER, TRUE, FRAME_SHOW_RESIZE_CORNER, TRUE,
		                      FRAME_ICON,xv_create(XV_NULL,ICON,ICON_IMAGE,TraceIcon,NULL),
		                      NULL);

      TraceControl = (Panel) xv_create(TraceWindow,PANEL,
                                       XV_X,0, XV_Y,0, 
                                       XV_WIDTH,230, 
                                       XV_HEIGHT,WIN_EXTEND_TO_EDGE,
                                       WIN_BORDER,FALSE,
                                       NULL);
                                       
      ListTrace = (Panel) xv_create (TraceControl,PANEL_LIST,
                                     XV_X,10,XV_Y,10,
                                     PANEL_LIST_WIDTH,200, 
                                     PANEL_LIST_DISPLAY_ROWS,11, 
                                     PANEL_LIST_TITLE,"Computed Strategies",
                                     PANEL_LAYOUT,PANEL_HORIZONTAL,
                                     PANEL_READ_ONLY,FALSE,
                                     PANEL_CHOOSE_ONE,FALSE, 
                                     PANEL_CHOOSE_NONE,TRUE,
                                     NULL);
   
      (void) xv_create(TraceControl,PANEL_BUTTON,
                       XV_X,10,XV_Y,260,
                       PANEL_LABEL_WIDTH,71, 
                       PANEL_LABEL_STRING,"  Select All",
                       PANEL_NOTIFY_PROC,SelectAllTraceStrategies,
                       NULL);
                     
      (void) xv_create(TraceControl,PANEL_BUTTON,
                       XV_X,120,XV_Y,260,
                       PANEL_LABEL_WIDTH,71, 
                       PANEL_LABEL_STRING,"Deselect All",
                       PANEL_NOTIFY_PROC,DeselectAllTraceStrategies,
                       NULL);

      (void) xv_create(TraceControl,PANEL_BUTTON,
                       XV_X,10,XV_Y,290,
                       PANEL_LABEL_WIDTH,71, 
                       PANEL_LABEL_STRING,"   Trace", 
                       PANEL_NOTIFY_PROC,RepaintGraph,
                       NULL);
                       
      (void) xv_create(TraceControl,PANEL_BUTTON,
                       XV_X,120,XV_Y,290,
                       PANEL_LABEL_WIDTH,71, 
                       PANEL_LABEL_STRING,"    Stop", 
                       PANEL_NOTIFY_PROC,StopTrace,
                       NULL);
      
      (void) xv_create(TraceControl,PANEL_BUTTON,
                       XV_X,10,XV_Y,320,
                       PANEL_LABEL_WIDTH,71, 
                       PANEL_LABEL_STRING," Continue", 
                       PANEL_NOTIFY_PROC,ContinueTrace,
                       NULL);
      
      (void) xv_create(TraceControl,PANEL_BUTTON,
                       XV_X,120,XV_Y,320,
                       PANEL_LABEL_WIDTH,71, 
                       PANEL_LABEL_STRING," One Step", 
                       PANEL_NOTIFY_PROC,TraceGraph,
                       NULL);
      
      (void) xv_create(TraceControl,PANEL_BUTTON,
                       XV_X,10,XV_Y,350,
                       PANEL_LABEL_WIDTH,182, 
                       PANEL_LABEL_STRING,"        Close Trace Window", 
                       PANEL_NOTIFY_PROC,QuitTrace,
                       NULL);
                  
      SliderSpeed = (Panel) xv_create(TraceControl,PANEL_SLIDER,
                                      XV_X,10,XV_Y,380,
                                      PANEL_LABEL_STRING,"Speed",
                                      PANEL_VALUE,30,
                                      PANEL_MIN_VALUE,1,
                                      PANEL_MAX_VALUE,50,
                                      PANEL_SLIDER_WIDTH,80,
                                      PANEL_TICKS,10,
                                      PANEL_LAYOUT,PANEL_HORIZONTAL,
                                      NULL);
                                      
      SliderWidth = (Panel) xv_create(TraceControl,PANEL_SLIDER,
                                      XV_X,10,XV_Y,410,
                                      PANEL_LABEL_STRING,"Width",
                                      PANEL_VALUE,LineWidth,
                                      PANEL_MIN_VALUE,0,
                                      PANEL_MAX_VALUE,5,
                                      PANEL_SLIDER_WIDTH,80,
                                      PANEL_TICKS,6,
                                      PANEL_LAYOUT,PANEL_HORIZONTAL,
                                      NULL);
                                      
      ChoiceMethod = (Panel) xv_create(TraceControl, PANEL_CHOICE,
		                       XV_X,10,XV_Y,440,
		                       PANEL_CHOICE_NCOLS,3,
		                       PANEL_LAYOUT,PANEL_HORIZONTAL,
		                       PANEL_CHOOSE_NONE,FALSE,
		                       PANEL_CHOICE_STRINGS,"Grid"," Pie ",NULL,
		                       PANEL_NOTIFY_PROC,ChangeTracingMethod,
		                       NULL);
                                      
      (void) xv_create(TraceControl,PANEL_TOGGLE,PANEL_FEEDBACK,PANEL_MARKED,
                       XV_X,140,XV_Y,440,
                       PANEL_CHOICE_NCOLS,1,
                       PANEL_LAYOUT,PANEL_HORIZONTAL,
                       PANEL_LABEL_STRING,"Grid Y/N",
                       PANEL_CHOICE_STRING,0, "",
                       PANEL_VALUE,1,
                       PANEL_NOTIFY_PROC, ShowGrid,
                       NULL);
                            
      mGeneration = (Panel) xv_create(TraceControl,PANEL_MESSAGE, 
                                      XV_X,10,XV_Y,470, 
                                      PANEL_LABEL_BOLD,TRUE, 
                                      NULL);
      
      mPopulation = (Panel) xv_create(TraceControl,PANEL_MESSAGE, 
                                      XV_X,10,XV_Y,490, 
                                      PANEL_LABEL_BOLD,TRUE, 
                                      NULL);
                                      
      TraceCanvas = (Canvas) xv_create(TraceWindow,CANVAS,
                                       XV_X,230,XV_Y,0, 
                                       XV_WIDTH,WIN_EXTEND_TO_EDGE, 
                                       XV_HEIGHT,WIN_EXTEND_TO_EDGE,
                                       CANVAS_AUTO_EXPAND,TRUE,
                                       CANVAS_AUTO_SHRINK,TRUE,
                                       CANVAS_REPAINT_PROC,RepaintGraph,
                                       CANVAS_RESIZE_PROC,ResizeGraph,
                                       NULL);
                                       
      xv_set(canvas_paint_window(TraceCanvas),
             WIN_EVENT_PROC,MyEventProc,
             WIN_CONSUME_EVENTS,LOC_MOVE,
             NULL);
                                    
      InitContextAndColor();
      for (i=0 ; i<NBSTRAT ; i++) {
         xv_set(ListTrace,
   	        PANEL_LIST_INSERT,i,
   	        PANEL_LIST_STRING,i,SORTSTRAT[i+1].name,
   	        PANEL_LIST_CLIENT_DATA,i,SORTSTRAT[i+1].last_qute-2,
   	        PANEL_LIST_SELECT,i,TRUE,
   	        NULL);
      }
   } else {
      nbrows = xv_get(ListTrace,PANEL_LIST_NROWS);
      for (i=nbrows-1 ; i>=0 ; i--) {
         xv_set(ListTrace,PANEL_LIST_DELETE,i,NULL);
      }
      for (i=0 ; i<NBSTRAT ; i++) {
   	xv_set(ListTrace,
   	       PANEL_LIST_INSERT,i,
   	       PANEL_LIST_STRING,i,SORTSTRAT[i+1].name,
   	       PANEL_LIST_CLIENT_DATA,i,SORTSTRAT[i+1].last_qute-2,
   	       PANEL_LIST_SELECT,i,TRUE,
   	       NULL);
     }
     xv_set(TraceWindow,XV_SHOW,TRUE,FRAME_CLOSED,FALSE,NULL);
     RepaintGraph();
   } 
}
