#include <stdio.h>
#include "scbuf.h"
#include "rterm.h"
#include "tm.h"

#define TIMER_SPEED 30
#define U_BOX1 0x2591 
#define U_BOX2 0x2592
#define ESC_KEY '\e'

//Prototypes
void draw_screen();
void tick();

// GLOBALS : SCREEN1 : ANIMATED | SCREEN2 : BASE SCREEN
SCREENCELL *screen1=NULL;
SCREENCELL *screen2=NULL;

NTIMER timer1;
int d_rows=0, d_columns=0;
int newsc_rows=0, newsc_columns=0;
int whereX=10; int whereY=10;
int yDIR = 1;
int xDIR = 1;
int timer_speed = TIMER_SPEED;

//FUNCTIONS

void draw_screen(){
//BASE SCREEN IS STORED IN SCREEN 2
int i=0;

     whereX = 10;
     xDIR = 1;
     yDIR = 1;
     whereY = 10;
 
     if (screen1 != NULL) deleteList(&screen1);
     if (screen2 != NULL) deleteList(&screen2);
     //Init 2 : Create 2 Screens for a double buffer  approach  
     d_rows=newsc_rows;
     d_columns=newsc_columns;
     create_screen(&screen1);
     create_screen(&screen2);
     //SCREEN 2
     screen_color(screen2, B_BLACK, FH_BLACK, U_BOX1);
     update_screen(screen2);
     //Top-Bar
     for (i=0;i<d_columns;i++)
          write_ch(screen2, i,1, U_BOX2, B_BLACK, FH_BLACK);
     for (i=0;i<d_columns;i++)
          write_ch(screen2, i,d_rows, U_BOX2, B_BLACK, FH_BLACK);
     //Save screen for later
     update_screen(screen2);
}

void tick(void){
//Animation - SCREEN1
     copy_screen(screen1,screen2);
     write_str(screen1,1,1,"DOUBLE SCREEN BUFFER DEMO - by v3l0r3k", B_WHITE, F_BLACK);
     //window1
     draw_window(screen1,2,3,25,9,B_BLACK,F_WHITE, B_CYAN, TRUE,TRUE,TRUE);
     write_str(screen1,3,3,"v      OPTIONS     [X]", B_CYAN, F_WHITE);
     write_str(screen1,3,4,"A: SHOW SCREEN 1", B_BLACK, F_WHITE);
     write_str(screen1,3,5,"B: SHOW SCREEN 2", B_BLACK, F_WHITE);
     write_str(screen1,3,6,"C: STOP/START TIMER", B_BLACK, F_WHITE);
     write_str(screen1,3,7,"D: RESUME TIMER", B_BLACK, F_WHITE);
     write_str(screen1,3,7,"+: INCREASE SPEED", B_BLACK, F_WHITE);
     write_str(screen1,3,8,"-: DECREASE SPEED", B_BLACK, F_WHITE);
     write_ch(screen1,24,8,'+',B_BLACK,FH_BLUE);
     //window2
     draw_window(screen1,30,10,60,18,B_BLACK,F_WHITE, B_GREEN, TRUE,TRUE,TRUE);
     draw_window(screen1,32,13,58,17,B_BLUE,F_WHITE, B_BLACK, FALSE,FALSE,FALSE);
     write_str(screen1,31,10,"v           DATA          [X]", B_GREEN, F_WHITE);
     write_str(screen1,32,11,"Some screen data", B_BLACK, FH_BLACK);
     write_str(screen1, 33,13, "Screen 1: ", B_BLUE,FH_WHITE);
     write_num(screen1, 43,13, length(&screen1), B_BLUE,FH_WHITE);
     write_str(screen1, 33,14, "Screen 2: ", B_BLUE,FH_WHITE);
     write_num(screen1, 43,14, length(&screen2), B_BLUE,FH_WHITE);
     write_str(screen1, 33,15, "Screen 1: ", B_BLUE,FH_WHITE);
     write_num(screen1, 44,15, timer_speed, B_BLUE,FH_WHITE);
     write_str(screen1, 33,15, "Timer(ms): ", B_BLUE,FH_WHITE);
     write_num(screen1, 43,16, timer1.ticks, B_BLUE,FH_WHITE);
     write_str(screen1, 33,16, "Ticks: ", B_BLUE,FH_WHITE);
     //animation
     if (whereX == d_columns-17) xDIR = -1;
     if (whereX == 0) xDIR = 1;
     if (whereY == 2)  {yDIR = 1;}
     if (whereY == d_rows) {yDIR = -1;}
     whereX = whereX + xDIR;
     whereY = whereY + yDIR; 
     write_str(screen1, whereX,whereY,"PRESS ESC TO EXIT", B_MAGENTA, F_WHITE);

     //Restore screen (only parts that change are updated);
     update_screen(screen1);
}

int main(){

char ch=0;
int keypressed = 0;

    //Init timer
    init_timer(&timer1,TIMER_SPEED);
    //Init Terminal
    init_term();
    draw_screen();
     do{    
         keypressed = kbhit();

         get_terminal_dimensions(&newsc_rows,&newsc_columns);

         if (keypressed == TRUE) 
          ch=readch();
         else
          ch=0;
                
	      if (newsc_rows != d_rows || newsc_columns != d_columns)
               {
		//Refresh screen size buffers if terminal dimension changes
		 draw_screen();
              }  
	 if (timerC(&timer1) == TRUE){
	      //Animation
              tick();
           }
           if (ch=='a') {stop_timer(&timer1); update_screen(screen1);}
           if (ch=='b') {stop_timer(&timer1); update_screen(screen2);}
           if (ch=='c') {if (timer1.ticks != -1) stop_timer(&timer1); else init_timer(&timer1,timer_speed);}
           if (ch=='d') {init_timer(&timer1,TIMER_SPEED);}
           if (ch=='+') {timer_speed += 10; init_timer(&timer1,timer_speed);}
           if (ch=='-') {if (timer_speed>10) timer_speed -= 10; init_timer(&timer1,timer_speed);}

       } while (ch != ESC_KEY);     
     //free memory
     if (screen1 != NULL) deleteList(&screen1);
     if (screen2 != NULL) deleteList(&screen2);
     //restore terminal
     close_term();
     return 0;
}

