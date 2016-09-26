void SetParameters (void)
{
   int ok;
   
   R = (int) xv_get(cR,PANEL_VALUE);
   P = (int) xv_get(cP,PANEL_VALUE);
   T = (int) xv_get(cT,PANEL_VALUE);
   N = (int) xv_get(cN,PANEL_VALUE);
   S = (int) xv_get(cS,PANEL_VALUE);
   GAME_LENGTH = (int) xv_get(cGameLength,PANEL_VALUE);
   REPEAT = (int) xv_get(cRepeat,PANEL_VALUE);
   NB_GEN = (int) xv_get(cNbGen,PANEL_VALUE);
   SEED = (int) xv_get(cSeed,PANEL_VALUE);
   LOSS = (int) xv_get(cLoss,PANEL_VALUE);
   NOISE = (int) xv_get(cNoise,PANEL_VALUE);
   
   if (!(T>R && R>P && P>S)) {
      ok=notice_prompt(SetupControl,NULL,
                             NOTICE_MESSAGE_STRINGS,
                             "WARNING !!!",
                             "The rule T>R>P>S",
                             "is not respected",
                             "Continue anywhere ?",
                             NULL,
                             NOTICE_BUTTON_YES,"Yes",
                             NOTICE_BUTTON_NO,"No",
                             NULL);
   }
      
   if (!(2*R>T+S)) {
      ok=notice_prompt(SetupControl,NULL,
                             NOTICE_MESSAGE_STRINGS,
                             "WARNING !!!",
                             "The rule 2R>T+S",
                             "is not respected",
                             "Continue anywhere ?",
                             NULL,
                             NOTICE_BUTTON_YES,"Yes",
                             NOTICE_BUTTON_NO,"No",
                             NULL);
   }
   
   if (ok) {
      xv_set(SetupWindow, XV_SHOW, FALSE, NULL);
   }
}

void ResetParameters (void)
{
   xv_set(cR,PANEL_VALUE,3,NULL);
   xv_set(cP,PANEL_VALUE,1,NULL);
   xv_set(cT,PANEL_VALUE,5,NULL);
   xv_set(cN,PANEL_VALUE,2,NULL);
   xv_set(cS,PANEL_VALUE,0,NULL);
   xv_set(cGameLength,PANEL_VALUE,1000,NULL);
   xv_set(cRepeat,PANEL_VALUE,8,NULL);
   xv_set(cNbGen,PANEL_VALUE,0,NULL);
   xv_set(cSeed,PANEL_VALUE,0,NULL);
   xv_set(cLoss,PANEL_VALUE,0,NULL);
   xv_set(cNoise,PANEL_VALUE,0,NULL);
}

void CreateSetupWindow (void)
{
   if (SetupWindow) {
      xv_set(SetupWindow, XV_SHOW, TRUE, NULL);
      return;
   }

   SetupIcon = xv_create(XV_NULL,SERVER_IMAGE,
                         SERVER_IMAGE_DEPTH,1,
                         SERVER_IMAGE_X_BITS,setup_bits,
                         XV_WIDTH,setup_width,
                         XV_HEIGHT,setup_height,
                         NULL);
  
   SetupWindow = (Frame) xv_create(MainWindow, FRAME,
  		                  XV_WIDTH,320, 
  		                  XV_HEIGHT,380,
  		                  XV_LABEL, "Setup Window",
  		                  XV_SHOW,TRUE,
		                  FRAME_SHOW_FOOTER,TRUE, 
		                  FRAME_SHOW_RESIZE_CORNER,FALSE,
                                  FRAME_ICON,xv_create(XV_NULL,ICON,ICON_IMAGE,SetupIcon,NULL),
		                  NULL);
   
   SetupControl = (Panel) xv_create(SetupWindow, PANEL,
                                    XV_X,0,XV_Y,0, 
                                    XV_WIDTH,WIN_EXTEND_TO_EDGE, 
                                    XV_HEIGHT,WIN_EXTEND_TO_EDGE,
                                    WIN_BORDER,FALSE,
                                    NULL);
   
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,10, 
                    PANEL_LABEL_STRING,"Reward", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                    
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,40, 
                    PANEL_LABEL_STRING,"Punishement",
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                    
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,70, 
                    PANEL_LABEL_STRING,"Temptation", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                    
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,100, 
                    PANEL_LABEL_STRING,"Renoucement", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                    
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,130, 
                    PANEL_LABEL_STRING,"Sucker's Score", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
                    
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,160, 
                    PANEL_LABEL_STRING,"Confrontation Length", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
   
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,190, 
                    PANEL_LABEL_STRING,"Random Strategies Repetition",
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
   
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,220, 
                    PANEL_LABEL_STRING,"Number of Generations", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
   
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,250, 
                    PANEL_LABEL_STRING,"Seed Value",
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
   
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,280, 
                    PANEL_LABEL_STRING,"Loss Value", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);
   
   (void) xv_create(SetupControl,PANEL_MESSAGE, 
                    XV_X,10,XV_Y,310, 
                    PANEL_LABEL_STRING,"Noise Value", 
                    PANEL_LABEL_BOLD,TRUE, 
                    NULL);

   cR = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT, 
                          XV_X,230,XV_Y,10, 
                          PANEL_VALUE_DISPLAY_LENGTH,4, 
                          PANEL_VALUE_STORED_LENGTH,80, 
                          PANEL_LAYOUT,PANEL_HORIZONTAL,
                          PANEL_MAX_VALUE,100, 
                          PANEL_MIN_VALUE,0, 
                          PANEL_VALUE,R,
                          PANEL_READ_ONLY,FALSE,
                          NULL);
                          
   cP = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                          XV_X,230,XV_Y,40,
                          PANEL_VALUE_DISPLAY_LENGTH,4, 
                          PANEL_VALUE_STORED_LENGTH,80,
                          PANEL_LAYOUT,PANEL_HORIZONTAL,
                          PANEL_MAX_VALUE,100,
                          PANEL_MIN_VALUE,0,
                          PANEL_VALUE,P,
                          PANEL_READ_ONLY,FALSE,
                          NULL);
   
   cT = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                          XV_X,230,XV_Y,70,
                          PANEL_VALUE_DISPLAY_LENGTH,4, 
                          PANEL_VALUE_STORED_LENGTH,80,
                          PANEL_LAYOUT,PANEL_HORIZONTAL,
                          PANEL_MAX_VALUE,100, 
                          PANEL_MIN_VALUE,0, 
                          PANEL_VALUE,T,
                          PANEL_READ_ONLY,FALSE,
                          NULL);
   
   cN = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                          XV_X,230,XV_Y,100,
                          PANEL_VALUE_DISPLAY_LENGTH,4, 
                          PANEL_VALUE_STORED_LENGTH,80,
                          PANEL_LAYOUT,PANEL_HORIZONTAL,
                          PANEL_MAX_VALUE,100, 
                          PANEL_MIN_VALUE,0, 
                          PANEL_VALUE,N,
                          PANEL_READ_ONLY,FALSE,
                          NULL);
   
   cS = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                          XV_X,230,XV_Y,130,
                          PANEL_VALUE_DISPLAY_LENGTH,4, 
                          PANEL_VALUE_STORED_LENGTH,80,
                          PANEL_LAYOUT,PANEL_HORIZONTAL,
                          PANEL_MAX_VALUE,100, 
                          PANEL_MIN_VALUE,0, 
                          PANEL_VALUE,S,
                          PANEL_READ_ONLY,FALSE,
                          NULL);
   
   cGameLength = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                                   XV_X,230,XV_Y,160,
                                   PANEL_VALUE_DISPLAY_LENGTH,4,
                                   PANEL_VALUE_STORED_LENGTH,80,
                                   PANEL_LAYOUT,PANEL_HORIZONTAL,
                                   PANEL_MAX_VALUE,1000, 
                                   PANEL_MIN_VALUE,0,
                                   PANEL_VALUE,GAME_LENGTH,
                                   PANEL_READ_ONLY,FALSE,
                                   NULL);
   
   cRepeat = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                               XV_X,230,XV_Y,190,
                               PANEL_VALUE_DISPLAY_LENGTH,4, 
                               PANEL_VALUE_STORED_LENGTH,80,
                               PANEL_LAYOUT,PANEL_HORIZONTAL,
                               PANEL_MAX_VALUE,100,
                               PANEL_MIN_VALUE,0,
                               PANEL_VALUE,REPEAT,
                               PANEL_READ_ONLY,FALSE,
                               NULL);   
   
   cNbGen = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                              XV_X,230,XV_Y,220,
                              PANEL_VALUE_DISPLAY_LENGTH,4, 
                              PANEL_VALUE_STORED_LENGTH,80,
                              PANEL_LAYOUT,PANEL_HORIZONTAL,
                              PANEL_MAX_VALUE,1000,
                              PANEL_MIN_VALUE,0, 
                              PANEL_VALUE,NB_GEN,
                              PANEL_READ_ONLY,FALSE,
                              NULL);
   
   cSeed = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                             XV_X,230,XV_Y,250,
                             PANEL_VALUE_DISPLAY_LENGTH,3,
                             PANEL_VALUE_STORED_LENGTH,80,
                             PANEL_LAYOUT,PANEL_HORIZONTAL,
                             PANEL_MAX_VALUE,100, 
                             PANEL_MIN_VALUE,0, 
                             PANEL_VALUE,SEED,
                             PANEL_READ_ONLY,FALSE,
                             NULL);
   
   cLoss = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                             XV_X,230,XV_Y,280,
                             PANEL_VALUE_DISPLAY_LENGTH,3, 
                             PANEL_VALUE_STORED_LENGTH,80,
                             PANEL_LAYOUT,PANEL_HORIZONTAL,
                             PANEL_MAX_VALUE,100, 
                             PANEL_MIN_VALUE,0, 
                             PANEL_VALUE,LOSS,
                             PANEL_READ_ONLY,FALSE,
                             NULL);
    
   cNoise = (Panel) xv_create(SetupControl,PANEL_NUMERIC_TEXT,
                              XV_X,230,XV_Y,310,
                              PANEL_VALUE_DISPLAY_LENGTH,3, 
                              PANEL_VALUE_STORED_LENGTH,80,
                              PANEL_LAYOUT,PANEL_HORIZONTAL,
                              PANEL_MAX_VALUE,100, 
                              PANEL_MIN_VALUE,0, 
                              PANEL_VALUE,NOISE,
                              PANEL_READ_ONLY,FALSE,
                              NULL);

   (void) xv_create(SetupControl,PANEL_BUTTON,
                    XV_X,50,XV_Y,360,
                    PANEL_LABEL_WIDTH,40, 
                    PANEL_LABEL_STRING,"Apply",
                    PANEL_NOTIFY_PROC,SetParameters,
                    NULL);
   
   (void) xv_create(SetupControl,PANEL_BUTTON,
                    XV_X,200,XV_Y,360,
                    PANEL_LABEL_WIDTH,40, 
                    PANEL_LABEL_STRING,"Reset",
                    PANEL_NOTIFY_PROC,ResetParameters,
                    NULL);
}

