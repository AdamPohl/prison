
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


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>


#define MAX_PART 1000	/* You can't have a longer confrontation between 2 strats.    */
#define MAX_GEN  1000	/* You can't have more generations in ecological competition  */
#define NB_STRAT_MAX 75	/* You can't use more strategies                              */


/*         Declaration for global variables                            */

long BASE;          /* Total number of entities (constant total during generations)    */
int NB_STRATEGIES;  /* number of choosen strategies for competition                    */
char cmd[50];       /* command executed to compute graphic representation with gnuplot */

FILE *data_file , *cmd_plot_file ;


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



time_t t;     		       /* for random generator initialization          */

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

enreg sort_str[NB_STRAT_MAX+2]; /* used to sort strategies on their score for output */

#include "strategy.h"


/* score matrix for 2 by 2 tournaments   */
long score_matrix[NB_STRAT_MAX+1][NB_STRAT_MAX+1];


typedef struct
{short index;  /* index number of the strategy     */
long value;    /* sum of its score               */
} enr_sum;

enr_sum sum[NB_STRAT_MAX+1];


/* used to compute percent cooperation */
int coop[NB_STRAT_MAX+2][NB_STRAT_MAX+2];



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
	coop[strat1][strat2]=nbC1;
	coop[strat2][strat1]=nbC2;
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

}


/*                tournament computation                  */
/* a strategy meets every other strategy including itself */
void tournament()
{int i,j;
	/* clear the score matrix before computation */
	for (i=1;i<=NB_STRATEGIES;i++)
		for (j=1;j<=NB_STRATEGIES;j++)
			score_matrix[i][j]=0;


	for (i=1; i<=NB_STRATEGIES; i++)
		for (j=i; j<=NB_STRATEGIES; j++)
			if (strategies[i].qute && strategies[j].qute)
				one_meeting(i,j);
	sum_computation();

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



/* calcul du pourcentage de cooperations */
void compute_coop(int *a)
{int i,j;
long base;
double total_coups,total_coop;
	for (i=1, base=0; i<=NB_STRATEGIES; i++) base+=strategies[i].qute;
	total_coups=(double)base*(base-1)*GAME_LENGTH;

	for (i=1, total_coop=0;i<=NB_STRATEGIES;i++)
	{	
		for (j=1;j<=NB_STRATEGIES;j++)
			
			if (i==j) total_coop+=(double)(strategies[i].qute*(strategies[i].qute-1))*coop[i][i];

			else	  total_coop+=(double)(strategies[i].qute*strategies[j].qute*coop[i][j]);
	}
	*a=(int)(total_coop/(total_coups/100)); 
}


/* add a new line in data file to use with Gnuplot or other plotter */
void line_in_data_file(int n)
{int i,a;
	compute_coop(&a);
	fprintf(data_file,"%d\t",n);
	for (i=1; i<=NB_STRATEGIES; i++)
	{
		fprintf(data_file,"%ld\t",strategies[i].qute);
	}

	fprintf(data_file,"%d\t ",a);  /* pour des chiffres plus humains */
	fprintf(data_file,"\n");
}


void creation_data_file()
{int i;
	/* creation of data_file  */
	data_file=fopen("result.dat","w");
	fprintf(data_file,"\t");
	for (i=1; i<=NB_STRATEGIES; i++)
		fprintf(data_file,"%s\t",strategies[i].name);
	fprintf(data_file,"\n");
	line_in_data_file(0);
}




void creation_plot_file()
{int i,last;
	/* plot_file creation for gnuplot  */
	cmd_plot_file=fopen("result.cmd","w");
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
	fprintf(cmd_plot_file,"plot 'result.dat' using 1:%ld  title \"%s\" , \\\n",
		sort_str[1].last_qute,sort_str[1].name);

	/* all the other but the last */
	for (i=2; i<last;i++)
		fprintf(cmd_plot_file,"     'result.dat' using 1:%ld  title \"%s\" , \\\n", 
			sort_str[i].last_qute,sort_str[i].name);

	/* the last line */
	fprintf(cmd_plot_file,"     'result.dat' using 1:%ld  title \"%s\" \n", 
		sort_str[last].last_qute,sort_str[last].name);

	fprintf(cmd_plot_file,"pause -1 \"Hit Return to continue\" \n");

	fprintf(cmd_plot_file,"set terminal postscript eps monochrome\n");
	fprintf(cmd_plot_file,"set output 'result.ps' \n");
	fprintf(cmd_plot_file,"replot\n");

	sprintf(cmd,"gnuplot result.cmd");


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

		line_in_data_file(i);

		if ((alive<=1) || stabilization())
			STOP=i;
		else
		if (LOSS && alive>1)  /* is there any possible match */
		{       perturbation();
		}

		if (i==NB_GEN) STOP=i;
	}
	free(ent_for_one);
	for (j=i+1;j<=NB_GEN;j++) line_in_data_file(j);
	if (STOP) return STOP; else return MAX_GEN;
}




void generations()
{	tournament();
	last=generations_loop();
	printf("\n  Population stabilization at the %d-th generation\n",last);
	creation_plot_file();
}




void recop_str(int source, int dest, int max)
{
	if (source>0 && source<=max && dest<=NB_STRAT_MAX)
	       memcpy(strategies+dest,foreseen+source,sizeof(enreg));
}


/* Allows the user to init the strategies array which will be used by the program
with a subclass of the strategies allowed in the 'foreseen' array
Choice must contain a (y|n)* mystring, strategies which index correspond to
a 'y' in the string are choosen.
Choice can also contain a list of strategies indices, separated by commas, which gives
the index of the strategies choosen for the program.
*/
void reaffect_strategies(char *choice)
{int i,j,number;
int nb_foreseen=0;

	for (nb_foreseen=1;foreseen[nb_foreseen].adr!=NULL;nb_foreseen++);

	i=0; number=0; j=1;
	if (isdigit(choice[0])) /* strategies choosen by their index */
	{	while (choice[i] !='\0')
		{	
			if (isdigit(choice[i])) number=number*10+(int)choice[i]-(int)'0';
			if (choice[i]=='.' || choice[i]==',')
			{	recop_str(number,j,nb_foreseen);
				number=0; j++;
			}
			i++;
		}
		recop_str(number,j,nb_foreseen);
					
	}
	else /* (y|n)* mystring */
	{	i=0; j=1;
		while (choice[i] != '\0')
		{       if (choice[i]=='y' || choice[i]=='Y' || choice[i]=='o' || choice[i]=='O')
			{	recop_str(i+1,j,nb_foreseen);
				j++;
			}
			i++;
		}
	}
	recop_str(nb_foreseen,j+1,nb_foreseen);
}


/* Warning : If you put this include before the qsort calls in the program
you will obtain a compilation error.
If you want to avoid this you must have to take out these 3 lines
at the beginning of getopt.c
	#ifndef const
	#define const
	#endif
But then, it does not agree with the GNU Licence ! :-( 

You can remove it under Unix
*/

#include "getopt.c"


void params_treatment(int argc, char **argv)
{
int c,i;
extern char *optarg;
extern int optind;
int errflg = 0;
char examples[200];

	while ((c = getopt(argc, argv, "r:p:t:n:s:l:R:g:S:L:N:a:oh")) != EOF)
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
		reaffect_strategies(examples);
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




#ifdef XPRISON
#include "xmain.c"  /* used to build xprison under Xview */
#else
 

int main(int argc,char *argv[])
{ int i;
 time_t elapsed;
	params_treatment(argc,argv);

	/* strategies number computation */
	for (i=1, NB_STRATEGIES=0 ; strategies[i].adr != NULL; i++) NB_STRATEGIES++;

	printf("%d Strategies taken into account \n",NB_STRATEGIES);
	printf("____________________________________________________\n");

	if (SEED) srand((unsigned) SEED);               /* fixed seed        */
	else srand((unsigned) time (&t));               /* real random game  */

	for (i=0, BASE=0; i<=NB_STRATEGIES;i++) BASE+=strategies[i].qute;

	elapsed = time(&t);

	creation_data_file();
	generations(); 


	printf("  Problem solved in %d seconds \n\n",time(&t)-elapsed);

	fclose(data_file); fclose(cmd_plot_file);
	system(cmd);  /* for GNUPLOT   */

	return(0);
}



#endif


