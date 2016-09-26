
/*---------------------------------------------------------------------*/
void DefaultParams (void)
{
   printf("Used Parameters\n");
   printf("R=%d  ",R);
   printf("P=%d  ",P);
   printf("T=%d  ",T);
   printf("S=%d  ",S);
   printf("N=%d  \n",N);
   printf("GAME_LENGTH = %d\n",GAME_LENGTH);
   printf("REPEAT      = %d\n",REPEAT);
   printf("NB_GEN      = %d\n",NB_GEN);
   printf("SEED        = %d\n",SEED);
   printf("LOSS        = %d\n",LOSS);
   printf("NOISE       = %d\n", NOISE);
}

void Generation (void)
{
   char mystring[5];
   int i;
   time_t elapsed;
   
   
   DefaultParams ();
   
   /* strategies number computation */
   for (i=1, NB_STRATEGIES=0 ; strategies[i].adr != NULL; i++) {
   	NB_STRATEGIES++;
   	printf("%s\n",strategies[i].name);
   }

   printf("%d Strategies taken into account \n",NB_STRATEGIES);
   printf("____________________________________________________\n");

   if (SEED) srand((unsigned) SEED);	/* fixed seed        */
   else srand((unsigned) time (&t));    /* real random game  */

   for (i=0, BASE=0; i<=NB_STRATEGIES;i++) {
   	BASE+=strategies[i].qute;
   }

   elapsed = time(&t);

   creation_data_file();
   generations();


   printf("  Problem solved in %d seconds \n\n",time(&t)-elapsed);

   fclose(data_file);
   fclose(cmd_plot_file);
   /*
   system(cmd);
   */
}

#include "interface.c"

int main(int argc,char *argv[])
{
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

   if (SEED) srand((unsigned) SEED);	/* fixed seed        */
   else srand((unsigned) time (&t));    /* real random game  */

   CreateMainWindow(argc,argv);
   xv_main_loop(MainWindow);
   exit(0);
}
/*---------------------------------------------------------------------*/

