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
BOOL updateAll = FALSE;
BOOL redPaint = FALSE;
BOOL TEST = FALSE;
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
     screen_color(screen1, B_BLACK, FH_BLACK, U_BOX1);
     //Top-Bar
     for (i=0;i<d_columns;i++)
          write_ch(screen1, i,1, U_BOX2, B_BLACK, FH_BLACK,TEST);
     for (i=0;i<d_columns;i++)
          write_ch(screen1, i,d_rows, U_BOX2, B_BLACK, FH_BLACK,TEST);
     write_str(screen1,1,1,"DOUBLE SCREEN BUFFER DEMO - by v3l0r3k", B_WHITE, F_BLACK,TEST);
     //window1
     draw_window(screen1,2,3,25,12,B_BLACK,F_WHITE, B_CYAN, TRUE,TRUE,TRUE,TEST);
     write_str(screen1,3,3,"v      OPTIONS     [X]", B_CYAN, F_WHITE,TEST);
     write_str(screen1,3,4,"A: SHOW SCREEN 1", B_BLACK, F_WHITE,TEST);
     write_str(screen1,3,5,"B: SHOW SCREEN 2", B_BLACK, F_WHITE,TEST);
     write_str(screen1,3,6,"C: STOP/START TIMER", B_BLACK, F_WHITE,TEST);
     write_str(screen1,3,7,"D: DUMP SCREEN", B_BLACK, F_WHITE,TEST);
     write_str(screen1,3,8,"P: UPDATE ALL/XOR", B_BLACK, F_WHITE,TEST);
     write_str(screen1,3,9,"R: RED PAINT IF XOR", B_BLACK, F_WHITE,TEST);
     write_str(screen1,3,10,"+: DECREASE SPEED", B_BLACK, F_WHITE,TEST);
     write_str(screen1,3,11,"-: INCREASE SPEED", B_BLACK, F_WHITE,TEST);
     write_ch(screen1,24,11,'+',B_BLACK,FH_BLUE,TEST);
     //window2
     draw_window(screen1,30,10,60,18,B_BLACK,F_WHITE, B_GREEN, TRUE,TRUE,TRUE,TEST);
     draw_window(screen1,32,13,58,17,B_BLUE,F_WHITE, B_BLACK, FALSE,FALSE,FALSE,TEST);
     write_str(screen1,31,10,"v           DATA          [X]", B_GREEN, F_WHITE,TEST);
     write_str(screen1,32,11,"Some screen data", B_BLACK, FH_BLACK,TEST);
     write_str(screen1, 33,13, "Screen 1: ", B_BLUE,FH_WHITE,TEST);
     write_str(screen1, 33,14, "Screen 2: ", B_BLUE,FH_WHITE,TEST);
     write_str(screen1, 33,15, "Screen 1: ", B_BLUE,FH_WHITE,TEST);
     write_str(screen1, 33,15, "Timer(ms): ", B_BLUE,FH_WHITE,TEST);
     write_str(screen1, 33,16, "Ticks: ", B_BLUE,FH_WHITE,TEST);
     update_screen(screen1);
     //Save screen for later
     copy_screen(screen2,screen1);
}

void tick(void){
int i=0; SCREENCELL oldchar;  
//Animation - SCREEN1
   //animation     
     copy_screen(screen1,screen2);
     //restore section on screen if updateAll is false
     if (updateAll == FALSE){
        for (i=0;i<17;i++){
            oldchar = read_cell(screen1, whereX+i,whereY);
           if (redPaint == FALSE) 
		write_ch(screen1, whereX+i,whereY, oldchar.ch, oldchar.backColor, oldchar.foreColor,TEST);
           else 
		write_ch(screen1, whereX+i,whereY, oldchar.ch, B_RED, oldchar.foreColor,TEST);
       }
     }
     if (whereX == d_columns-17) xDIR = -1;
     if (whereX == 0) xDIR = 1;
     if (whereY == 2)  {yDIR = 1;}
     if (whereY == d_rows) {yDIR = -1;}
     whereX = whereX + xDIR;
     whereY = whereY + yDIR; 
     write_str(screen1, whereX,whereY,"PRESS ESC TO EXIT", B_MAGENTA, F_WHITE,TEST);
     write_num(screen1, 43,13, length(&screen1), B_BLUE,FH_WHITE,TEST);
     write_num(screen1, 43,14, length(&screen2), B_BLUE,FH_WHITE,TEST);
     write_num(screen1, 44,15, timer_speed, B_BLUE,FH_WHITE,TEST);
     write_num(screen1, 43,16, timer1.ticks, B_BLUE,FH_WHITE,TEST);
     //Restore screen (only parts that change are updated if updateAll is false);
     if (updateAll == FALSE) xor_update(screen1,screen2);
     else update_screen(screen1);
 
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
        
           if (ch=='a') {stop_timer(&timer1); dump_screen(screen1);}
           if (ch=='b') {stop_timer(&timer1); dump_screen(screen2);}
           if (ch=='c') {if (timer1.ticks != -1) stop_timer(&timer1); else init_timer(&timer1,timer_speed);}
           if (ch=='d') {dump_screen(screen1); if (TEST == TRUE) TEST= FALSE; else TEST = TRUE;}
           if (ch=='+') {timer_speed += 10; init_timer(&timer1,timer_speed);}
           if (ch=='-') {if (timer_speed>10) timer_speed -= 10; init_timer(&timer1,timer_speed);}
           if (ch=='p') { if (updateAll== TRUE) updateAll=FALSE; else updateAll = TRUE;}
           if (ch=='r') { if (redPaint== TRUE) redPaint=FALSE; else redPaint = TRUE;}

       } while (ch != ESC_KEY);     
     //free memory
     if (screen1 != NULL) deleteList(&screen1);
     if (screen2 != NULL) deleteList(&screen2);
     //restore terminal
     close_term();
     return 0;
}

