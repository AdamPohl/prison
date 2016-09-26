             /**************************************************/
             /*           Iterated Prisoner's Dilemma          */ 
             /*                                                */ 
             /*         Pour la Science, vol 181, Nov 92       */ 
             /*         Pour la Science, vol 187, Mai 93       */ 
             /*         IOS Press, Chaos and Society, 95       */ 
             /*                                                */ 
             /*           Authors: Philippe  MATHIEU           */ 
             /*                    Jean-paul DELAHAYE          */ 
             /*                    Bruno     BEAUFILS          */ 
             /*                                                */ 
             /*               Date     : 06/06/92              */ 
             /*               Update   : 16/04/96              */ 
             /*                                                */ 
             /* e-mail : {mathieu, delahaye, beaufils}@lifl.fr */
             /**************************************************/


/*****************************************************************************
       This program is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
                    Contact the authors for more details.
*****************************************************************************/

#include "classics.str"           /* classical strategies */

/* #include "renounce.str" */     /* 6 strats from IT-233 with renouncement */
/* #include "pls.str"      */     /* 95 strats from PLS tournament & IT-249 */
/* #include "misc.str"     */     /* miscellaneous strategies */
/* #include "lift.str"     */     /* strategies for lift dilemma */
/* #include "slaves.str"   */     /* Master/slaves model to win tournament */

enreg foreseen[NB_STRAT_MAX+2]=
{
  "null",NULL,0,0,0,0,

/*****************************************************************************
                     WRITE THE LIST OF USED STRATEGY HERE

             the syntax of each line must be the following one :

                    "extern_name",internal_name,100,0,X,0,

   . extern_name is the name that you want to see in the results 
     and graphics.   
   . internal_name is the name of the procedure coding your strategy
   . The third parameter is the size of initial population for 
     ecological competitions. Usually we put 100 for all of them.
   . 0 is required at 4th argument
   . X must be replaced by 0 or 1. 0 if your strategy does not make 
     use of random and 1 if there is any rand in it.
   . 0 is required at last argument

 *****************************************************************************/

  "all_c",all_c,100,0,0,0,
  "all_d",all_d,100,0,0,0,
  "ipd_random",ipd_random,100,0,1,0,
  "tit_for_tat",tit_for_tat,100,0,0,0,
  "spiteful",spiteful,100,0,0,0,
  "per_ddc",per_ddc,100,0,0,0,
  "per_ccd",per_ccd,100,0,0,0,
  "soft_majo",soft_majo,100,0,0,0,
  "prober",prober,100,0,0,0,
  "mistrust",mistrust,100,0,0,0,
  "hard_majo",hard_majo,100,0,0,0,
  "hard_tft",hard_tft,100,0,0,0,

  "gradual",gradual,100,0,0,0,
  "hard_tf2t",hard_tf2t,100,0,0,0,
  "soft_tf2t",soft_tf2t,100,0,0,0,
  "hard_joss",hard_joss,100,0,1,0,
  "soft_joss",soft_joss,100,0,1,0,
  "c_then_per_dc",c_then_per_dc,100,0,0,0,
  "per_cd",per_cd,100,0,0,0,
  "per_cccdcd",per_cccdcd,100,0,0,0,
  "prob_c_4_on_5",prob_c_4_on_5,100,0,1,0,
  "per_ccccd",per_ccccd,100,0,0,0,
  "calculator",calculator,100,0,1,0,
  "prober2",prober2,100,0,0,0,
  "prober3",prober3,100,0,0,0,
  "prober4",prober4,100,0,0,0,
  "hard_prober",hard_prober,100,0,0,0,
  "better_and_better",better_and_better,100,0,1,0,
  "worse_and_worse",worse_and_worse,100,0,1,0,
  "worse_and_worse2",worse_and_worse2,100,0,1,0,
  "worse_and_worse3",worse_and_worse3,100,0,1,0,
  "doubler",doubler,100,0,1,0,
  "soft_spiteful",soft_spiteful,100,0,0,0,

  "pavlov",pavlov,100,0,0,0,
  "gradual_killer",gradual_killer,100,0,0,0,
  "slow_tft",slow_tft,100,0,0,0,
  "easy_go",easy_go,100,0,0,0,

/*****************************************************************************
 ***                            END OF LIST                                ***
 *****************************************************************************/

  "null",NULL,0,0,0,0
};


/* 6 stratsegies from the PLS paper with renouncement.
   If you want to use one of them, remove comments at the beginning of file
   at the line  #include "renounce.str".  */
/*
  "prudente",prudente,100,0,0,0,
  "susceptible",susceptible,100,0,0,0,
  "indulgente",indulgente,100,0,0,0,
  "dure",dure,100,0,0,0,
  "rand_echap",rand_echap,100,0,1,0,
  "tft_echap",tft_echap,100,0,0,0,
*/


/*  23 STRATEGIES for Lift Dilemma   
   If you want to use one of them, remove comments at the beginning of file
   at the line   #include "lift.str".  */
/*
  "raison56",raison56,100,0,1,0,
  "raison43",raison43,100,0,1,0,
  "raison10",raison10,100,0,1,0,
  "raison20",raison20,100,0,1,0,
  "raison30",raison30,100,0,1,0,
  "raison40",raison40,100,0,1,0,
  "raison50",raison50,100,0,1,0,
  "raison60",raison60,100,0,1,0,
  "raison70",raison70,100,0,1,0,
  "raison80",raison80,100,0,1,0,
  "raison90",raison90,100,0,1,0,
  "raison_naif_56",raison_naif_56,100,0,1,0,
  "raison_naif_50",raison_naif_50,100,0,1,0,
  "raison_douce",raison_douce,100,0,1,0,
  "cooperate_raison",cooperate_raison,100,0,1,0,
  "raison_prudente",raison_prudente,100,0,1,0,
  "raison_dephasage",raison_dephasage_mult,100,0,1,0,
  "raison_tft_50",raison_tft_50,100,0,1,0,
  "raison_tft_10",raison_tft_10,100,0,1,0,
  "raison_tft_90",raison_tft_90,100,0,1,0,
  "jp_concours",jp_concours,100,0,1,0,
  "phil_concours",phil_concours,100,0,1,0,
  "nous_concours",nous_concours,100,0,1,0,
*/


/*  strategies from the PLS tournament with renouncement
    39 56 65 71 72 90 95 97 103 are not programmed
   If you want to use one of them, remove comments at the beginning of file
   at the line  #include "pls.str".  */
/*
  "strat1",strat1,100,0,0,0,
  "strat2",strat2,100,0,0,0,
  "strat3",strat3,100,0,0,0,
  "strat4",strat4,100,0,0,0,
  "strat5",strat5,100,0,0,0,
  "strat6",strat6,100,0,0,0,
  "strat7",strat7,100,0,0,0,
  "strat8",strat8,100,0,0,0,
  "strat9",strat9,100,0,0,0,
  "strat10",strat10,100,0,0,0,
  "strat11",strat11,100,0,1,0,
  "strat12",strat12,100,0,0,0,
  "strat13",strat13,100,0,0,0,
  "strat14",strat14,100,0,0,0,
  "strat15",strat15,100,0,0,0,
  "strat16",strat16,100,0,0,0,
  "strat17",strat17,100,0,0,0,
  "strat18",strat18,100,0,0,0,
  "strat19",strat19,100,0,1,0,
  "strat20",strat20,100,0,0,0,
  "strat21",strat21,100,0,0,0,
  "strat22",strat22,100,0,0,0,
  "strat23",strat23,100,0,0,0,
  "strat24",strat24,100,0,0,0,
  "strat25",strat25,100,0,0,0,
  "strat26",strat26,100,0,0,0,
  "strat27",strat27,100,0,0,0,
  "strat28",strat28,100,0,0,0,
  "strat29",strat29,100,0,0,0,
  "strat30",strat30,100,0,0,0,
  "strat31",strat31,100,0,0,0,
  "strat32",strat32,100,0,0,0,
  "strat33",strat33,100,0,0,0,
  "strat34",strat34,100,0,0,0,
  "strat35",strat35,100,0,0,0,
  "strat36",strat36,100,0,0,0,
  "strat37",strat37,100,0,0,0,
  "strat38",strat38,100,0,0,0,
  "strat40",strat40,100,0,0,0,
  "strat41",strat41,100,0,1,0,
  "strat42",strat42,100,0,0,0,
  "strat43",strat43,100,0,0,0,
  "strat44",strat44,100,0,0,0,
  "strat45",strat45,100,0,0,0,
  "strat46",strat46,100,0,0,0,
  "strat47",strat47,100,0,0,0,
  "strat48",strat48,100,0,0,0,
  "strat49",strat49,100,0,0,0,
  "strat50",strat50,100,0,0,0,
  "strat51",strat51,100,0,0,0,
  "strat52",strat52,100,0,0,0,
  "strat53",strat53,100,0,0,0,
  "strat54",strat54,100,0,0,0,
  "strat55",strat55,100,0,0,0,
  "strat57",strat57,100,0,0,0,
  "strat58",strat58,100,0,0,0,
  "strat59",strat59,100,0,0,0,
  "strat60",strat60,100,0,1,0,
  "strat61",strat61,100,0,0,0,
  "strat62",strat62,100,0,0,0,
  "strat63",strat63,100,0,0,0,
  "strat64",strat64,100,0,0,0,
  "strat66",strat66,100,0,0,0,
  "strat67",strat67,100,0,0,0,
  "strat68",strat68,100,0,0,0,
  "strat69",strat69,100,0,0,0,
  "strat70",strat70,100,0,0,0,
  "strat73",strat73,100,0,0,0,
  "strat74",strat74,100,0,0,0,
  "strat75",strat75,100,0,0,0,
  "strat76",strat76,100,0,0,0,
  "strat77",strat77,100,0,0,0,
  "strat78",strat78,100,0,0,0,
  "strat79",strat79,100,0,0,0,
  "strat80",strat80,100,0,0,0,
  "strat81",strat81,100,0,0,0,
  "strat82",strat82,100,0,0,0,
  "strat83",strat83,100,0,0,0,
  "strat84",strat84,100,0,0,0,
  "strat85",strat85,100,0,0,0,
  "strat86",strat86,100,0,0,0,
  "strat87",strat87,100,0,0,0,
  "strat88",strat88,100,0,0,0,
  "strat89",strat89,100,0,0,0,
  "strat91",strat91,100,0,0,0,
  "strat92",strat92,100,0,0,0,
  "strat93",strat93,100,0,0,0,
  "strat94",strat94,100,0,0,0,
  "strat96",strat96,100,0,0,0,
  "strat98",strat98,100,0,0,0,
  "strat99",strat99,100,0,0,0,
  "strat100",strat100,100,0,0,0,
  "strat101",strat101,100,0,0,0,
  "strat102",strat102,100,0,0,0,
  "strat104",strat104,100,0,0,0,
*/

/*
 * Local Variables:
 * fill-column: 78
 * End:
 */
