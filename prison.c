
                 /*********************************************/
                 /*         Iterated Prisoner's Dilemma       */
                 /*                                           */
                 /*        Pour la Science, vol 181, Nov 92   */
                 /*        Pour la Science, vol 187, Mai 93   */
                 /*        IOS Press, Chaos and Society, 95   */
                 /*                                           */
                 /*        Authors: Philippe  MATHIEU         */
                 /*                 Jean-paul DELAHAYE        */
                 /*                                           */
                 /*        Date  : 06/06/92                   */
                 /*        maj   : 24/05/96                   */
                 /*                                           */
                 /*   e-mail : {mathieu, delahaye}@lifl.fr    */
                 /*********************************************/



/***************************************************************************
   This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
		Contact the authors for more details.
*****************************************************************************/



/*********************************************************************************/
/* Classical IPD      : R=3  , S=0  , T=5  , P=1                                 */
/*                      with S <= P <= R <= T and (S+T)/2 <= R                   */
/*                                                                               */
/* With Renouncement  : R=3  , S=0  , T=5  , P=1  , N=2                          */
/*                     As soon as one of the 2 players renounce, each of them    */
/*			obtains 2 points for each other play                     */
/*                                                                               */
/* Lift Dilemma       : R=3  , S=0  , T=8  , P=1                                 */
/*                      with S <= P <= R <= T and (S+T)/2 >  R                   */
/*                      Unlike the classical Dilemma this one encourage both     */
/*                      to have a mutual agreement to play in opposite phase.    */
/*                     We can see that random (non deterministic) strategies are */
/*                     the best in this variant                                  */
/*                                                                               */
/* Leader's Dilemma   : R=1 , S=3 , T=5 , P=0                                    */
/*                                                                               */
/* Other variants     : With noise  prison -N                                    */
/*                      With loss   prison -L                                    */
/*********************************************************************************/



/********************************************************************************/
/*    WARNING   WARNING   WARNING   WARNING   WARNING   WARNING   WARNING       */
/*                                                                              */
/*   If you change anything in this program, be sure to avoid an                */
/*   overful error in arithmetic computations                                   */
/*                                                                              */
/*   Sensitive variables are score_matrix, sum, ent_for_one and total           */
/*   Which are used in generations_loop                                         */
/*                                                                              */
/********************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>


#define MAX_PART 1000	/* You can't have a longer confrontation between 2 strats.    */
#define MAX_GEN  1000	/* You can't have more generations in ecological competition  */
#define NB_STRAT_MAX 75 /* You can't use more strategies                              */


/*         Declaration for global variables                            */
int TRACE1;         /* Trace of each confrontation                     */
int TRACE2;         /* Trace of ecological computation generations     */
int TRACE3;         /* Trace of scores after tournament or generations */
int TRACE4;         /* Trace of score matrix                           */
int GRAPH;          /* File for graphics output                        */
long BASE;          /* Total number of entities (constant total during generations)    */
int NB_STRATEGIES;  /* number of choosen strategies for competition                    */
char cmd[50];       /* command executed to compute graphic representation with gnuplot */
char name[50];      /* prefix name for all generated files                             */

char data_name_file[50]="",
cmd_plot_name_file[50]="",ps_name_file[50]="", bat_name_file[50]="";

FILE *trace_file,  *data_file , *cmd_plot_file, *bat_file ;


/*         Declaration for parameters used in command line                 */
static int R = -1;       	/* double cooperation value                */
static int P = -1;       	/* double defect value                     */
static int T = -1;       	/* defect against cooperate  value         */
static int S = -1;       	/* cooperate against defect value          */
static int N = -1;       	/* renounciation value                     */
static int GAME_LENGTH = -1;	/* confrontation length                    */
static int REPEAT = -1;   	/* number of random strategies repetition  */
static int NB_GEN = -1;  	/*  number of generations computed         */
static int SEED = -1;    	/* seed value : 0 if real random game      */
static int LOSS=-1;    		/* perturbation value in each generation   */
static int NOISE=-1;     	/* percentage noise perturbation           */


typedef enum {COOPERATE,DEFECT, RENOUNCE} card; /* all possible actions for a player */

card player1[MAX_PART+1], player2[MAX_PART+1];	/* the two player's history    */
card *MH, *RH;		       /* My_history and Rival's history               */
long score1, score2;	       /* scores of player 1 and player2               */
int nbD1,nbD2,nbC1,nbC2;       /* Cooperation an defect number for each player */
int last;   		       /* number of the last generation computed       */
typedef int counter[3];

int turn;		       /* number of the current game    (start to 1)   */



time_t t;                      /* for random generator initialization          */

typedef card (*ptr_fnct)(int); /* all defined strategies must return a card type */


/*      strategy description : Data must be in this format               */
typedef struct
{char name[20];	  /* public name of the strategy                         */
 ptr_fnct adr;    /* address of the strategy (private name)              */
 long qute;       /* number of entities                                  */
 long last_qute;  /* previous number of entities                         */
 int recompute;  /* boolean : strategy that must be computed many times */
 int stop;        /* stop date of this strategy                          */
} enreg;



/* allows each strategy to know its score during the game without computation */
long score(card *tab)
{       if (tab==player1) return score1;
	else return score2;
}

/* allows each strategy to know the number of DEFECT during the game without computation */
int nbD(card *tab)
{       if (tab==player1) return nbD1;
	else return nbD2;
}

/* allows each strategy to know the number of COOPERATE during the game without computation */
int nbC(card *tab)
{       if (tab==player1) return nbC1;
	else return nbC2;
}


/* score computation limited to the n last plays */
long score_lim(int n, card *tab)
{long score1=0, score2=0, i;
	if (turn-1<n) n=turn-1;
		for (i=turn-n; i<=turn-1; i++)
		{
			if (MH[i]==COOPERATE && RH[i]==COOPERATE)
			{       score1 +=R; score2 +=R; }
			else
			if (MH[i]==COOPERATE && RH[i]==DEFECT)
			{       score1 +=S; score2 +=T; }
			else
			if (MH[i]==DEFECT && RH[i]==COOPERATE)
			{       score1 +=T; score2 +=S; }
			else
			if (MH[i]==DEFECT && RH[i]==DEFECT)
			{       score1 +=P; score2 +=P; }
		}
	if (tab==MH) return score1;
	else return score2;
}

/* number of DEFECT limited to the n last plays  */
int defect_lim(int n, card *tab)
{int i,r;
	if (turn-1<n) n=turn-1;
	r=0;
	for (i=turn-n; i<=turn-1; i++)
		if (tab[i]==DEFECT) r++;
	return r;
}



/**************************************/
/*             STRATEGIES             */
/**************************************/

enreg strategies[NB_STRAT_MAX+2]=
	{"null",NULL,0,0,0,0,
	"null",NULL,0,0,0,0
	};


enreg to_test[NB_STRAT_MAX+2]= /* used only in best_of() */
	{"null",NULL,0,0,0,0,
	"null",NULL,0,0,0,0
	};

enreg sort_str[NB_STRAT_MAX+2]; /* used to sort strategies on their score for output */

#include "strategy.h" 


/* score matrix for 2 by 2 tournaments   */
long score_matrix[NB_STRAT_MAX+1][NB_STRAT_MAX+1];


typedef struct
{short index;  /* index number of the strategy     */
long value;    /* sum of its score               */
} enr_sum;

enr_sum sum[NB_STRAT_MAX+1];


/**************************************************/


void *my_alloc(long taille)
{char *p;
	p=(char *)malloc(taille);
	if (p != NULL) return(p);
	else
	{	printf("Allocation problem : Program aborted \n");
		exit(1);
	}
}



void trace()
{char mystring[10]; char trace_name[50];
	printf("  Trace of each confrontation                        (Y/N) ? ");
	scanf("%s",mystring);
	TRACE1=(toupper(mystring[0])=='Y' || toupper(mystring[0])=='O');
	printf("  Trace of scores after tournament or generations    (Y/N) ? ");
	scanf("%s",mystring);
	TRACE3=(toupper(mystring[0])=='Y' || toupper(mystring[0])=='O');
	printf("  Trace of ecological computation generations (1/10) (Y/N) ? ");
	scanf("%s",mystring);
	TRACE2=(toupper(mystring[0])=='Y' || toupper(mystring[0])=='O');
	sprintf(trace_name,"%s.tra",name);
	printf("  Trace of score matrix                              (Y/N) ? ");
	scanf("%s",mystring);
	TRACE4=(toupper(mystring[0])=='Y' || toupper(mystring[0])=='O');

	if (TRACE1 || TRACE2 || TRACE3 || TRACE4) trace_file=fopen(trace_name,"w");
}


void score_computation()
{
	if (player1[turn]==COOPERATE && player2[turn]==COOPERATE)
	{       score1 +=R; score2 +=R; }
	else
	if (player1[turn]==COOPERATE && player2[turn]==DEFECT)
	{       score1 +=S; score2 +=T; }
	else
	if (player1[turn]==DEFECT && player2[turn]==COOPERATE)
	{       score1 +=T; score2 +=S; }
	else
	if (player1[turn]==DEFECT && player2[turn]==DEFECT)
	{       score1 +=P; score2 +=P; }
	else
	if (player1[turn]==RENOUNCE || player2[turn]==RENOUNCE)
	{       score1 +=N; score2 +=N; }
}


/* run a tournament between two strategies  */
void play_a_game(int strat1, int strat2)
{int i;
	score1=0; score2=0; nbD1=0; nbD2=0; nbC1=0; nbC2=0;
	for (turn=1; turn <= GAME_LENGTH ; turn++)
	{
		MH=player1; RH=player2;
		player1[turn] = (strategies[strat1].adr)(1);

		MH=player2; RH=player1;
		player2[turn] = (strategies[strat2].adr)(2);


		if (NOISE)  /*  To play the Prisoner's dilemma with noise  */
		{	i=rand()%1000;
			if (i < NOISE*10) /* in thousandth of unit */
			{
				if (i%2==0)
				{
					if (player1[turn]==COOPERATE) player1[turn]=DEFECT;
					else  player1[turn]=COOPERATE;
				}
				else
				{
					if (player2[turn]==COOPERATE) player2[turn]=DEFECT;
					else player2[turn]=COOPERATE;
				}
			}
		}


		if (player1[turn]==RENOUNCE || player2[turn]==RENOUNCE)
		{       while (turn <= GAME_LENGTH)
			{       player1[turn]=RENOUNCE;
				player2[turn]=RENOUNCE;
				score_computation();
				turn++;
			}
		}
		else
		{
			if (player1[turn]==DEFECT) nbD1++; else nbC1++;
			if (player2[turn]==DEFECT) nbD2++; else nbC2++;
			score_computation();
		}
	}
}


/* comparison between 2 records for sorting the enr_sum array   */
/* used by sum_computation() and tournament_for_subclasses()    */

int comp_cum(enr_sum *a, enr_sum *b)
{       if (a->value < b->value) return +1;   /* a is after b  */
	else
	if (a->value > b->value) return -1;   /* a is before b */
	else return 0;
}


typedef int (*qsort_func)(const void *, const void *);

void sum_computation()
{int i,j;
	for(i=1; i<=NB_STRATEGIES; i++)
	{       sum[i].value=0;
		sum[i].index=i;
		for (j=1; j<=NB_STRATEGIES; j++)
			sum[i].value+=score_matrix[i][j];
	}
	/* At the end we sort the array */
	qsort(sum+1, NB_STRATEGIES, sizeof(enr_sum), (qsort_func)comp_cum);
}


void trace_scores(FILE *fich)
{int i;
	fprintf(fich,"\n Score list after tournament \n");
	for (i=1; i<=NB_STRATEGIES;i++)
		fprintf(fich,"%d\t%15s\t= %6ld\n",
			i,
			strategies[sum[i].index].name,
			sum[i].value);
	fprintf(fich,"\n");
}


void tournament_trace(int strat1, int strat2, int r1,int r2)
{int i,nbcar;
	if (GAME_LENGTH>30) nbcar=30; else nbcar=GAME_LENGTH;

	fprintf(trace_file,"%s against %s",strategies[strat1].name,strategies[strat2].name);
	fprintf(trace_file,"\ngame %15s : ",strategies[strat1].name);
	for (i=1;i<=nbcar;i++)
		if (player1[i]==DEFECT) fprintf(trace_file,"D ");
		else if (player1[i]==COOPERATE) fprintf(trace_file,"C ");
		else fprintf(trace_file,"R ");
	fprintf(trace_file,"\ngame %15s : ",strategies[strat2].name);
	for (i=1;i<=nbcar;i++)
		if (player2[i]==DEFECT) fprintf(trace_file,"D ");
		else if (player2[i]==COOPERATE) fprintf(trace_file,"C ");
		else fprintf(trace_file,"R ");
	fprintf(trace_file,"\npoints : %s=%d  %s=%d\n\n",strategies[strat1].name,r1,strategies[strat2].name,r2);
}

 
/*                   Meeting between two strategies                                   */
/* if one of them uses the random function, the meeting will be computed REPEAT times */
void one_meeting(int strat1, int strat2)
{int i;
long r1,r2,cum1,cum2;
	if (strategies[strat1].recompute || strategies[strat2].recompute )
	{       cum1=0; cum2=0;
		for (i=1;i<=REPEAT;i++)
		{       play_a_game(strat1,strat2);
			cum1+=score1; cum2+=score2;
		}
		r1=cum1/REPEAT; r2=cum2/REPEAT;
	}
	else
	{       play_a_game(strat1,strat2);
		r1=score1; r2=score2;
	}

	score_matrix[strat1][strat2]=r1;
	score_matrix[strat2][strat1]=r2;
	if (TRACE1) tournament_trace(strat1,strat2,r1,r2);

}


/*                tournament computation                  */
/* a strategy meets every other strategy including itself */
void tournament()
{int i,j;
	/* clear the score matrix before computation */
	for (i=1;i<=NB_STRATEGIES;i++)
		for (j=1;j<=NB_STRATEGIES;j++)
			score_matrix[i][j]=0;

	if (TRACE1) fprintf(trace_file,"\n\nTrace of the beginning of each confrontation \n\n");

	for (i=1; i<=NB_STRATEGIES; i++)
		for (j=i; j<=NB_STRATEGIES; j++)
			if (strategies[i].qute && strategies[j].qute)
				one_meeting(i,j); 

	sum_computation();

	if (TRACE3) trace_scores(trace_file);
}


/* analysis of the results after a round robin tournament */
void analysis()
{int i,j;
long best_gap, worst_gap, best_score, worst_score,strat;

	tournament();

	printf("\n\t\t\tAnalysis of round robin tournaments\n");

	for (i=1; i<=NB_STRATEGIES; i++)
	{	printf("%15s\t",strategies[i].name);
	
		/* search for the best gap */
		best_gap=-100000; strat=1;
		for (j=1;j<=NB_STRATEGIES;j++)
		{	if (score_matrix[i][j]-score_matrix[j][i] > best_gap)
			{
				best_gap=score_matrix[i][j]-score_matrix[j][i];
				strat=j;
			}
		}
		printf("best gap \t %8ld against %15s\n",best_gap,strategies[strat].name);

		/* search for the whorst gap */
		worst_gap=100000; strat=1;
		for (j=1;j<=NB_STRATEGIES;j++)
		{	if (score_matrix[i][j]-score_matrix[j][i] < worst_gap)
			{
				worst_gap=score_matrix[i][j]-score_matrix[j][i];
				strat=j;
			}
		}
		printf("\t\tworst gap \t %8ld against %15s\n",worst_gap,strategies[strat].name);

		/* search for the best score */
		best_score=-100000; strat=1;
		for (j=1;j<=NB_STRATEGIES;j++)
		{	if (score_matrix[i][j] > best_score)
			{
				best_score=score_matrix[i][j];
				strat=j;
			}
		}
		printf("\t\tbest score \t %8ld against %15s\n",best_score,strategies[strat].name);


		/* search for the worst score */
		worst_score=100000; strat=1;
		for (j=1;j<=NB_STRATEGIES;j++)
		{	if (score_matrix[i][j] < worst_score)
			{
				worst_score=score_matrix[i][j];
				strat=j;
			}
		}
		printf("\t\tworst score \t %8ld against %15s\n",worst_score,strategies[strat].name);

	}

	printf("\n");
}


/* comparison between 2 enregs to sort strategies array     */
/* 2 criteria : number of entities and if not on stop dates */
int comp_str(enreg *a, enreg *b)
{       if (a->stop==0 && b->stop!=0) return -1;
	if (a->stop!=0 && b->stop==0) return +1;
	if (a->stop==0 && b->stop==0) /* anyone stopped */
	{
		if (a->qute < b->qute) return +1;       /* a is after b  */
		else
		if (a->qute > b->qute) return -1;       /* a is before b */
		else return 0;
	}
	else /* both are stopped */
	{
		if (a->stop < b->stop) return +1;     /* a is after b  */
		else
		if (a->stop > b->stop) return -1;     /* a is before b */
		else return 0;
	}
}

void trace_population(int gen, FILE *fich)
{int i; long total;
	fprintf(fich,"------  Generation score in %d ------\n",gen);
	memcpy(sort_str,strategies,sizeof(strategies));
	qsort(sort_str+1, NB_STRATEGIES, sizeof(enreg), (qsort_func)comp_str);

	total=0;
	for (i=1;i<=NB_STRATEGIES;i++)
	{       if (sort_str[i].stop==0)
		{       fprintf(fich,"%d\t%15s\t= %6ld\n",i,sort_str[i].name,sort_str[i].qute);
			total+=sort_str[i].qute;
		}
		else
		   fprintf(fich,"%d\t%15s\t= %6ld stopped in %d\n",i,sort_str[i].name,sort_str[i].qute,sort_str[i].stop);

	}
	fprintf(fich,"total = %ld entities\n\n",total);
}


int stabilization()
{int i,stable;
	stable=1;
	for (i=1; stable && i<=NB_STRATEGIES; i++)
		if (strategies[i].last_qute != strategies[i].qute)
			stable=0;
	return(stable);
}


void perturbation()
{long total;
int i,j,k;
	for (i=1, total=0; i<=NB_STRATEGIES; i++)
		total+=strategies[i].qute;
	for (i=1; i<=LOSS; i++)
	{       j=(rand()%total)+1;
		k=1;
		while (j>strategies[k].qute)
		{       j-=strategies[k].qute; k++; }
		strategies[k].qute--;
		total--;
	}
}


/* add a new line in data file to use with Gnuplot or other plotter */
void line_in_data_file(int n)
{int i;
	fprintf(data_file,"%d\t",n);
	for (i=1; i<=NB_STRATEGIES; i++)
		fprintf(data_file,"%ld\t",strategies[i].qute);
	fprintf(data_file,"\n");
}


void creation_data_file()
{int i;
	/* creation of data_file  */
	data_file=fopen(data_name_file,"w");
	fprintf(data_file,"\t"); 
	for (i=1; i<=NB_STRATEGIES; i++)
		fprintf(data_file,"%s\t",strategies[i].name);
	fprintf(data_file,"\n");
	line_in_data_file(0);
}



void creation_plot_file()
{int i,last;
	/* plot_file creation for gnuplot  */
	cmd_plot_file=fopen(cmd_plot_name_file,"w");
	fprintf(cmd_plot_file,"set grid\n");
	fprintf(cmd_plot_file,"set data style lines \n");

	/* to obtain the index of the strategy after the qsort for gnuplot,
	   i use last_qute to store the index rank of a strategy in the data file.
	   By this way the obtain each curve name by its index in last_qute
	*/
	for (i=1; i<=NB_STRATEGIES; i++)
	  strategies[i].last_qute=i+1; 

	memcpy(sort_str,strategies,sizeof(strategies));
	qsort(sort_str+1, NB_STRATEGIES, sizeof(enreg), (qsort_func)comp_str);

	/* we only plot the 20 first best strategies */
	if (NB_STRATEGIES<20) last=NB_STRATEGIES; else last=20;

	/* the first line */
	fprintf(cmd_plot_file,"plot '%s' using 1:%ld  title \"%s\" , \\\n",
		data_name_file,sort_str[1].last_qute,sort_str[1].name);

	/* all the other but the last */
	for (i=2; i<last;i++)
		fprintf(cmd_plot_file,"     '%s' using 1:%ld  title \"%s\" , \\\n",
			data_name_file,sort_str[i].last_qute,sort_str[i].name);

	/* the last line */
	fprintf(cmd_plot_file,"     '%s' using 1:%ld  title \"%s\" \n",
		data_name_file,sort_str[last].last_qute,sort_str[last].name);

	fprintf(cmd_plot_file,"pause -1 \"Hit Return to continue\" \n");

	fprintf(cmd_plot_file,"set terminal postscript eps monochrome\n");
	fprintf(cmd_plot_file,"set output \"%s\" \n",ps_name_file);
	fprintf(cmd_plot_file,"replot\n");

	sprintf(cmd,"gnuplot %s",cmd_plot_name_file);

}



/* Computes ecological competion and return last generation number  */
int generations_loop()
{int i,j,k,alive;
 int STOP=0;
 double total, verif;
 double coeff;
 double *ent_for_one;


	/* clean the stop and last_qute fields */
	for (i=1;i<=NB_STRATEGIES;i++)
	{	strategies[i].last_qute=0; strategies[i].stop=0;}

	ent_for_one=my_alloc(sizeof(double)*(NB_STRATEGIES+1));
	for (i=0, BASE=0; i<=NB_STRATEGIES;i++) BASE+=strategies[i].qute;

	for (i=1; i<=MAX_GEN && !STOP ; i++)
	{       /* total entities computation for each strategy */
		for (j=1; j<=NB_STRATEGIES; j++)
		{       ent_for_one[j]=0;
			for (k=1; k<=NB_STRATEGIES; k++)
			{
				if (j==k)
		                        ent_for_one[j]+= (strategies[k].qute - 1) * score_matrix[j][k];
				else    ent_for_one[j]+= strategies[k].qute * score_matrix[j][k];

			}       
		}

		/* coeff computation to come back to constant population (BASE) */
		for (j=1, total=0; j<=NB_STRATEGIES; j++)
		{       
			total+= (double)strategies[j].qute * (double)ent_for_one[j];

			/* if you have too many strategies you may
			   get an overflow error on total
			   Remove comments if you want to check this
			*/
/*
			verif=total; verif++;
			if (total>=verif)
				printf("Overflow error on total : %lf\n",total);
*/
		}

		coeff= (double)BASE/total;
	
		/* New population scale with BASE */
		for (j=1; j<=NB_STRATEGIES;j++)
		{       strategies[j].last_qute = strategies[j].qute;
			if (total)
				strategies[j].qute = (long)(coeff*(double)ent_for_one[j]*(double)strategies[j].qute);
		}

		/* Count the number of alive strategies and note stopped date for dead ones */
		for (j=1, alive=0; j<=NB_STRATEGIES; j++)
			if (strategies[j].qute > 0) alive++;
			else
			if (strategies[j].stop==0) strategies[j].stop=i;

		if (TRACE2 && i%10==1) trace_population(i,trace_file);
		if (GRAPH==1) line_in_data_file(i);
		if ((alive<=1) || stabilization())
		{
			if (TRACE2)
			{       fprintf(trace_file," Population stabilization at the %d-th generation\n",i-1);
				trace_population(i,trace_file);
			}
			STOP=i;
		}
		else
		if (LOSS && alive>1)  /* is there any possible match */
		{       perturbation();
			if (TRACE2 && i%10==1)
			{       fprintf(trace_file,"   After perturbation, \n");
				trace_population(i,trace_file);
			}
		}

		if (i==NB_GEN) STOP=i;
	}
	free(ent_for_one);
	for (j=i+1;j<=NB_GEN;j++) line_in_data_file(j);

	if (STOP) 
	{	if (TRACE3) trace_population(STOP,trace_file);
		return STOP;
	}	
	else 
	{	if (TRACE3) trace_population(MAX_GEN,trace_file);
		return MAX_GEN;
	}
}




void generations()
{char mystring[5];
	printf("  File for graphics output                           (Y/N) ? ");
	scanf("%s",mystring);
	GRAPH=(toupper(mystring[0])=='Y' || toupper(mystring[0])=='O');
	if (GRAPH) creation_data_file();
	tournament();
	last=generations_loop();
	printf("\n  Population stabilization at the %d-th generation\n",last);
	if (GRAPH) {creation_plot_file();}
}


void trace_tableau()
{int i,j;
	fprintf(trace_file,"\n\nScore matrix (2 by 2)\n");
	fprintf(trace_file,"\t");
	for (i=1; i<=NB_STRATEGIES; i++)
		fprintf(trace_file,"%s\t",strategies[i].name);
	fprintf(trace_file,"\n");
	for (i=1; i<=NB_STRATEGIES; i++)
	{       fprintf(trace_file,"%s\t",strategies[i].name);
		for (j=1; j<=NB_STRATEGIES; j++)
			fprintf(trace_file,"%ld\t",score_matrix[i][j]);
		fprintf(trace_file,"\n");
	}
}



/* increment the i-th elmt of cpt count to obtain : n elmts in p */
/* used by combinations */

int inc(int *cpt, int i, int n, int p)
{	if (i==1 && cpt[i]==p-n+1) return 1;  /* it was the last */	
	if (cpt[i]==(p-n+i))
	{	if (inc(cpt,i-1,n,p)==1) return 1; 
		else cpt[i]=cpt[i-1]+1;
	}
	else cpt[i]++;
	return 0;
}



/* computation of comb(n,p) : p elmts in a total of n */
long comb(int n, int p)
{int i;
long result=1;
	if (p>(n-p)) p=(n-p); 
	for (i=1; i<=p;i++) result*=(n-i+1);
	for (i=1;i<=p; i++) result/=i;
	return result;
}


/* This function computes the results for all the confrontations      */
/* between all the choosen strategies except n.                       */
/* Round by round we take out n strategies and we note the            */
/* winner in a round robin tournament.                                */
/* At the end we write the number of victories for each strategy      */

void tournament_for_subclasses()
{int i,j, n, size;
int cpt[NB_STRAT_MAX+1]; /* used to generate all the subclasses */
long *save_scores;
enr_sum *result;  /* used to count the number of victories */
long exaeco;	/* used to count exaeco strategies after a controntation */

	printf("\t Size of subclasses ? ");
	scanf("%d",&size);

	n=NB_STRATEGIES-size; 

	printf("\t Note that %ld tournaments will be computed. It can take a long time \n\n",comb(NB_STRATEGIES,size));


	result=my_alloc(sizeof(enr_sum)*(NB_STRATEGIES+1));
	for (i=1; i<=NB_STRATEGIES; i++) {result[i].index=i; result[i].value=0;}
	exaeco=0;

	tournament();

	save_scores=my_alloc(sizeof(score_matrix));
	memcpy(save_scores,score_matrix,sizeof(score_matrix));

	
	/* init of the counter used to generate the strategies to take out */
	for (i=1;i<=n;i++) cpt[i]=i;
	cpt[n]--;	
	
	while (inc(cpt,n,n,NB_STRATEGIES)==0)
	{
		/* the actual set of strategies to take out is in cpt */
		
		for (i=1;i<=n;i++)
			for (j=1; j<=NB_STRATEGIES; j++)
			{	score_matrix[j][cpt[i]]=0;
				score_matrix[cpt[i]][j]=0;
			}

		sum_computation();

/* if you want to see all the results */
/*
for (i=1; i<=NB_STRATEGIES;i++)
	printf("%3d %15s \n",sum[i].value, strategies[sum[i].index].name);
printf("\n\n");
*/
		for (i=1; i<=NB_STRATEGIES; i++)
			if (sum[i].value==sum[1].value) 
			{	result[sum[i].index].value++;
				exaeco++;
			}
		exaeco--;  /*we have counted one too many */

		memcpy(score_matrix,save_scores,sizeof(score_matrix));
		if (TRACE3) trace_scores(trace_file);
	}

	/* At the end we sort the array */
	qsort(result+1, NB_STRATEGIES, sizeof(enr_sum), (qsort_func)comp_cum);

	printf("Number of victories \n");
	for (i=1;i<=NB_STRATEGIES;i++)
		printf("%15s \t: %3d\n", strategies[result[i].index].name, result[i].value);
	free(result);
	free(save_scores);

	printf("There was %ld exaeco strategies \n", exaeco);
	printf("Each strategy played in %d tournaments \n\n", comb(NB_STRATEGIES-1,size-1));

}


/* Still the same but for ecological competition  */
void evolution_for_subclasses()
{int i, n, size;
int val_max;
int cpt[NB_STRAT_MAX+1]; /* used to generate all the subclasses */
enreg *save_str;
enr_sum *result;  /* used to count the number of victories */
long exaeco;	/* used to count exaeco strategies after a controntation */

	printf("\t Size of subclasses ? ");
	scanf("%d",&size);

	n=NB_STRATEGIES-size; 

	printf("\t Note that %ld evolutions will be computed. It can take a long time \n\n",comb(NB_STRATEGIES,size));


	result=my_alloc(sizeof(enr_sum)*(NB_STRATEGIES+1)); 

	for (i=1;i<=NB_STRATEGIES; i++) 
	  {result[i].index=i; result[i].value=0;}

        exaeco=0;

	tournament();

	save_str=my_alloc(sizeof(strategies));
	memcpy(save_str,strategies,sizeof(strategies));

	
	/* init of the counter used to generate the strategies to take out */
	for (i=1;i<=n;i++) cpt[i]=i;
	cpt[n]--;	
	
	while (inc(cpt,n,n,NB_STRATEGIES)==0)
	{
		/* the actual set of strategies to take out is in cpt */
		/* the initial pop is 100 for each strategy           */
		for (i=1; i<=NB_STRATEGIES;i++) strategies[i].qute=100; 
		for (i=1;i<=n;i++)
			strategies[cpt[i]].qute=0;

		(void) generations_loop();

/* if you want to see all the results */
/*
for (i=1; i<=NB_STRATEGIES;i++)
	printf("%3d %15s \n",strategies[i].qute, strategies[i].name);
printf("\n\n");
*/
		/* we search for the first one */
		val_max=0;
		for (i=1;i<=NB_STRATEGIES;i++)
			if (strategies[i].qute>val_max) 
			   {val_max=strategies[i].qute;}

		for (i=1; i<=NB_STRATEGIES; i++)
			if (strategies[i].qute==val_max) 
			{	result[i].value++;
				exaeco++;
			}
		exaeco--;  /*we have counted one too many */

		memcpy(strategies,save_str,sizeof(strategies));
	}

	/* At the end we sort the array */
	qsort(result+1, NB_STRATEGIES, sizeof(enr_sum), (qsort_func)comp_cum);

	printf("Number of victories \n");
	for (i=1;i<=NB_STRATEGIES;i++)
		printf("%15s \t: %3d\n", strategies[result[i].index].name, result[i].value);
	free(result);
	free(save_str);

	printf("There was %ld exaeco strategies \n", exaeco);
	printf("Each strategy played in %d evolutions \n\n", comb(NB_STRATEGIES-1,size-1));

}


/* Main function to run autotest on all subclasses of strategies */
void autotest()
{char mystring[5];
	printf("\t Autotest of strategy subclasses\n");
	printf("\t Tournament or Generations ? ");
	scanf("%s",mystring);
	     if (toupper(mystring[0])=='T')  tournament_for_subclasses();
	else if (toupper(mystring[0])=='G')  evolution_for_subclasses();

}



/* The first set of strategies is fixed. We want to know which
strategy of the second set is the best is this fixed set */

void bestof_computation()
{int i,j, NB_TEST, NB_FIXED;
char mystring[5];
long result;  /* used to count the score of each strategy */

	printf("\t Tournament or Generations ? ");
	scanf("%s",mystring);

	NB_FIXED=NB_STRATEGIES;
	/* size of best_of set of strategies */
	for (i=1, NB_TEST=0 ; to_test[i].adr != NULL; i++) NB_TEST++;
	NB_STRATEGIES+=NB_TEST;
	if (NB_TEST<1)
	{	printf("Use the -b option to define to set to study \n");
		exit(1);
	}
	if (NB_STRATEGIES > NB_STRAT_MAX)
	{	printf("Total number of strategies too big \n");
		printf("Increase NB_STRAT_MAX and recompile \n");
		exit(1);
	}

	/* add the two sets of strategies before the tournament computation */
	for (i=1;i<=NB_TEST;i++)
		memcpy(strategies+NB_FIXED+i,to_test+i,sizeof(enreg));
	memcpy(strategies+NB_STRATEGIES+1,strategies,sizeof(enreg));

	tournament();

	
			
	if (toupper(mystring[0])=='T') /* tournament */
	{	printf("\nResults of the best_of tournament (not sorted)\n");
		for (i=1;i<=NB_TEST;i++)
		{	result=0;
			for (j=1;j<=NB_FIXED;j++)
				result+=score_matrix[NB_FIXED+i][j];
			result+=score_matrix[NB_FIXED+i][NB_FIXED+i];
		}
		printf("%15s \t: %3d \n",strategies[NB_FIXED+i],result);
	}
	else /* generations */
	{	printf("\nResults of the best_of generations (not sorted)\n");
		for (i=1;i<=NB_TEST;i++)
		{	for (j=1;j<=NB_FIXED;j++) strategies[j].qute=100;
			for (j=1;j<=NB_TEST;j++) strategies[NB_FIXED+j].qute=0;
			strategies[NB_FIXED+i].qute=100;

			(void) generations_loop();
			
			memcpy(sort_str,strategies,sizeof(strategies));
			qsort(sort_str+1, NB_STRATEGIES, sizeof(enreg), (qsort_func)comp_str);
			
			/* I search for my strategy i */
			for (j=1;j<=NB_STRATEGIES && strcmp(strategies[NB_FIXED+i].name,sort_str[j].name)!=0;j++);
			
			printf("%15s : %3d \t rank : %2d",
				strategies[NB_FIXED+i].name, strategies[NB_FIXED+i].qute,j);

			if (strategies[NB_FIXED+i].qute < sort_str[1].qute)
				printf(" (winner : %15s with %3d)",sort_str[1].name, sort_str[1].qute);

			printf("\n");

		}
	}
}





/* Computation of dominant strategies is a 2 by 2 competition :
  A is strong dominant if AA+AB >= BB+BA  for each B
  A is weak dominant if AB >= BA  for each B
*/
void dominant()
{int i,j,found,lost,strength;
long score_i, score_j;
float ecart;
	if (NB_STRATEGIES < 2) exit(1);
	printf("\t Dominant search in 2 by 2 tournaments\n");
	printf("\t Weak or Strong domination (1/2) :");
	scanf("%d",&strength);
	tournament();
	found=0;
	for (i=1; i<=NB_STRATEGIES ;i++)
	{	lost=0; /* initiallly strat i has never lost a meeting */
		ecart=0.0;
		for (j=1; j<=NB_STRATEGIES && !lost; j++)
		{	if (strength==1) score_i=score_matrix[i][j];
			else             score_i=score_matrix[i][i]+score_matrix[i][j];

			if (strength==1) score_j=score_matrix[j][i];
			else             score_j=score_matrix[j][j]+score_matrix[j][i];

			/* printf("%s against %s : %ld %ld \n",strategies[i].name,strategies[j].name,score_i, score_j);  */

			if (score_i < score_j)
				lost=1; /* strat i has lost one time */
			ecart+=(score_i - score_j);
		}
		if (!lost)
		{	printf("\n\t The %s strategy is %s dominant in the chosen panel\n",
				 strategies[i].name, (strength==1)?"weak":"strong");
			printf("\t with an average difference of %2.3f points\n", ecart/(float)(NB_STRATEGIES-1));
			found++;
		}
	}

	if (found==0) printf("\t There is no %s dominant strategy\n ", (strength==1)?"weak":"strong");
}



/* return 1 if i wins against j with weak or strong comparison */
int beat(int i, int j, int strength)
{	if (strength==1) /* weak comparison */
	{	if (score_matrix[i][j] > score_matrix[j][i]) return 1;}

	else /* strong comparison  */
	{	if (score_matrix[i][j]+ score_matrix[i][i] > score_matrix[j][i]+score_matrix[j][j]) return 1;}

	return 0;
}



/* search for a cycle of 3 winning strategies               */
/* i wins against j , j wins against k and k wins against i */
void cycle()
{int strength,i,j,k;
	if (NB_STRATEGIES < 3) exit(1);
	printf("\t Search for cycles of length 3 in 2 by 2 tournaments \n");
	printf("\t Weak or Strong cycle (1/2) :");
	scanf("%d",&strength);
	tournament();

	for (i=1; i<=NB_STRATEGIES;i++)
	for (j=1; j<=NB_STRATEGIES;j++)
		if (beat(i,j,strength))
		for (k=1; k<=NB_STRATEGIES;k++)
		{	if (beat(j,k,strength) & beat(k,i,strength))
			{	printf("\t cycle obtained with %s, %s and %s \n",
					strategies[i].name, strategies[j].name,strategies[k].name);
			}
		}
}




void recop_str(int ind_source, enreg *dest, int ind_dest, int max)
{
	if (ind_source>0 && ind_source<=max && ind_dest<=NB_STRAT_MAX)
	       memcpy(dest+ind_dest,foreseen+ind_source,sizeof(enreg));
}


/* 
Allows the user to init the strategies array which will be used by
the program with a subclass of the strategies allowed in the
'foreseen' array Choice must contain a (y|n)* mystring, strategies
which index correspond to a 'y' in the string are choosen.

Choice can also contain a list of strategies indices, separated by
commas, which gives the index of the strategies choosen for the
program.  
*/
void choosen_strategies(char *choice, enreg *dest)
{int i,j,number;
int nb_foreseen=0;

	for (nb_foreseen=1;foreseen[nb_foreseen].adr!=NULL;nb_foreseen++);

	i=0; number=0; j=1;
	if (isdigit(choice[0])) /* strategies choosen by their index */
	{	while (choice[i] !='\0')
		{	
			if (isdigit(choice[i])) number=number*10+(int)choice[i]-(int)'0';
			if (choice[i]=='.' || choice[i]==',')
			{	recop_str(number,dest,j,nb_foreseen);
				number=0; j++;
			}
			i++;
		}
		recop_str(number,dest,j,nb_foreseen);

	}
	else /* (y|n)* mystring */
	{	i=0; j=1;
		while (choice[i] != '\0')
		{       if (choice[i]=='y' || choice[i]=='Y' || choice[i]=='o' || choice[i]=='O')
			{	recop_str(i+1,dest,j,nb_foreseen);
				j++;
			}
			i++;
		}
	}
	recop_str(nb_foreseen,dest,j+1,nb_foreseen);
}


/* Warning : If you put this include before the qsort calls in the program
you will obtain a compilation error.
If you want to avoid this you must have to take out these 3 lines
at the beginning of getopt.c
	#ifndef const
	#define const
	#endif
But then, it does not agree with the GNU Licence ! :-( 
*/

#include "getopt.c"


void params_treatment(int argc, char **argv)
{
int c,i;
extern char *optarg;
extern int optind;
int errflg = 0;
char examples[200];

	while ((c = getopt(argc, argv, "r:p:t:n:s:l:R:g:S:L:N:a:b:oh")) != EOF)
	switch (c) {
	case 'r': /* double cooperation value (R)eward */
		sscanf(optarg,"%d",&R);
		break;
	case 'p': /* double defect value (P)unishment */
		sscanf(optarg,"%d",&P);
		break;
	case 't': /* defect against cooperate (T)emptation */
		sscanf(optarg,"%d",&T);
		break;
	case 'n': /* renouncement */
		sscanf(optarg,"%d",&N);
		break;
	case 's': /* cooperate against defect (S)ucker's score */
		sscanf(optarg,"%d",&S);
		break;
	case 'l': /* confrontation length */
		sscanf(optarg,"%d",&GAME_LENGTH);
		if (GAME_LENGTH > MAX_PART) GAME_LENGTH = MAX_PART;
		break;
	case 'R': /* number of random strategies (R)epetition */
		sscanf(optarg,"%d",&REPEAT);
		break;
	case 'g': /* number of generations computed (else until stability) */
		sscanf(optarg,"%d",&NB_GEN);
		if (NB_GEN > MAX_GEN) NB_GEN= MAX_GEN;
		break;
	case 'S': /* (S)eed value : 0 if real random game  */
		sscanf(optarg,"%d",&SEED);
		break;
	case 'L': /* Value of (Lo)ss in each generation (else no perturbation)  */
		sscanf(optarg,"%d",&LOSS);
		break;
	case 'N': /*  percentage (N)oise value in \% */
		sscanf(optarg,"%d",&NOISE);
		break;
	case 'a': /* strategies subclass taken into (A)ccount  */
		sscanf(optarg,"%s",examples);
		choosen_strategies(examples,strategies);
		break;
	case 'b': /* strategies used to compute the (b)est_of  */
		sscanf(optarg,"%s",examples);
		choosen_strategies(examples,to_test);
		break;
	case 'o': /* (O)utput usable strategies list */
		i=1;
		while (foreseen[i].adr != NULL)
		{       printf("%2d\t%20s",i,foreseen[i].name); 
		        if (i%2==0) printf("\n"); else printf("\t|\t");
		        i++;
		}
		exit(1);
		break;
		
	case 'h':
		    printf("---------  help -------\n");
		    printf("-r <int>	double cooperation value (R)eward \t\t(default 3)\n");
		    printf("-p <int>	double defect value (P)unishment  \t\t(default 1)\n");
		    printf("-t <int>	defect against cooperate (T)emptation  \t\t(default 5)\n");
		    printf("-s <int>	cooperate against defect (S)ucker's score  \t(default 0)\n");
		    printf("-n <int>	Renouncement  \t\t\t\t\t(default 2)\n");
		    printf("-l <int>	confrontation (l)ength  \t\t\t(default 1000)\n");
		    printf("-R <int>	number of random strategies (R)epetition  \t(default 8)\n");
		    printf("-g <int>	number of (g)enerations computed  \t\t(default until stab.)\n");
		    printf("-S <int>	(S)eed value : 0 if real random game  \t\t(default 1)\n");
		    printf("-L <int>	(L)oss in each generation  \t\t\t(default 0)\n");
		    printf("-N <int>	percentage (N)oise value  \t\t\t(default 0)\n");
		    printf("-a <string>	strategies subclass taken into (a)ccount  \t(default all)\n");
		    printf("		(y|n)* or list of strategy number separated by commas \n");
		    printf("-b <string>	strategies used for best_of computation \t(default null)\n");
		    printf("-o 		(o)utput usable strategies list \n");
		    printf("-h 		this help message \n");
		    exit(1);
		    break;
	case '?':
		errflg++;
	}
	if (errflg) 
	{
		(void)fprintf(stderr,
		"usage: prison [-r <int>|-p <int>|-t <int>|-n <int>|-s <int>|-l <int>|-R <int>|-g <int>|-S <int>|-L <int>|-N <int>|-a <string>|-o|-h]\n");
	       exit (2);
	}


	/**********************/
	/*   DEFAULT VALUES   */
	/**********************/

	if (R<0) R=3;
	if (S<0) S=0;
	if (T<0) T=5;
	if (P<0) P=1;
	if (N<0) N=2;
	if (GAME_LENGTH<0) GAME_LENGTH=MAX_PART;
	if (NB_GEN>MAX_GEN) NB_GEN=MAX_GEN;
	if (REPEAT<0) REPEAT=8;
	if (NB_GEN<0) NB_GEN=0;  /* stop at stabilization */
	if (SEED<0) SEED=1;
	if (LOSS<0) LOSS=0;
	if (NOISE<0) NOISE=0;
	if (strategies[1].adr==NULL) memcpy(strategies,foreseen,(NB_STRAT_MAX+2)*sizeof(enreg));

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


int main(int argc,char *argv[])
{char mystring[5];
 int i;
 time_t elapsed;
	params_treatment(argc,argv);

	/* strategies number computation */
	for (i=1, NB_STRATEGIES=0 ; strategies[i].adr != NULL; i++) NB_STRATEGIES++;

	printf("%d Strategies taken into account \n",NB_STRATEGIES);
	printf("____________________________________________________\n");


	if (SEED) srand((unsigned) SEED);               /* fixed seed        */
	else srand((unsigned) time (&t));               /* real random game  */

	printf("  Tournament, Generations, Subclass, Domination, Cycles, Best-of (T/G/S/D/C/B) ? ");
	scanf("%s",mystring);
	printf("  Output files prefix name ? ");
	scanf("%s",name);
	sprintf(data_name_file,"%s.dat",name);
	sprintf(cmd_plot_name_file,"%s.cmd",name);
	sprintf(ps_name_file,"%s.ps",name);
	sprintf(bat_name_file,"%s.bat",name);

	/* copy of the command line in a .bat file */
	bat_file=fopen(bat_name_file,"w");
	for (i=0;i<argc;i++) fprintf(bat_file,"%s ",argv[i]);
	fclose(bat_file);

	trace();
	elapsed = time(&t);
	switch  (toupper(mystring[0])) 
	{
		case 'T' : tournament(); analysis(); trace_scores(stdout); break;
		case 'G' : generations(); trace_population(last,stdout);   break;
		case 'S' : autotest(); break;
		case 'D' : dominant(); break;
		case 'C' : cycle(); break;
		case 'B' : bestof_computation();
	}

	printf("  Problem solved in %d seconds \n\n",time(&t)-elapsed);

	if (TRACE4) trace_tableau();

	if (TRACE1 || TRACE2 || TRACE3 || TRACE4) fclose(trace_file);
	if (GRAPH) 
	{       fclose(data_file); fclose(cmd_plot_file);
		system(cmd);  /* for GNUPLOT   */
	}


	return(0);
}
