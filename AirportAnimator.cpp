#include "AirportAnimator.hpp"

#include <assert.h>
#include<curses.h>
#include <pthread.h>
#include <string>
#include <time.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// how far from the left edge of the screen to start drawing the airport
static int left_base=8;

// internal lock to make sure no two (or more) threads are drawing at same time
static pthread_mutex_t __screenLock;

// internal utility to lock drawing screen. 
static void lockScreen()
{
  pthread_mutex_lock(&__screenLock);
}

// internal utility to unlock drawing screen. 
static void unlockScreen()
{
  pthread_mutex_unlock(&__screenLock);
}

void
AirportAnimator::init()
{
  // initialize the lock for drawing on the screen. 
  if (pthread_mutex_init(&__screenLock, NULL) != 0) {
    cerr << "mutex init has failed -- contact Dr. Blythe!!!" << endl;
  }

  //initialize screen, ignore newline presses, and clear the screen. 
  initscr();
  nonl();
  erase();

  // draws left "wall" for airport terminal
  for (int r=0; r<3; r++)
    mvprintw(r, left_base-1, "|");

  // draw in gate and airplane for each airplane. 
  for(int i=0; i<8; i++)
    {
      // draw in gate right wall
      for (int r=0; r<3; r++)
	mvprintw(r, left_base+8*i+7, "|");

      // draw in gate info
      mvprintw(0, left_base+ 8*i, "Plane:%d", i);
      mvprintw(1, left_base+ 8*i, "Pass:%2d", 0);
      mvprintw(2, left_base+ 8*i, "[BOARD]");

      // calculate column for airplane. 
      int col = i*8+left_base+2;

      // draw in airplane
      mvprintw(3, col, " |");
      mvprintw(4, col, "/%1d\\", i);

      // force all additions to display. 
      refresh();
    }
  
  
  // draw runway
  mvprintw(15,10,
	   "===========================================================");
  mvprintw(19,10,
	   "===========================================================");

  // force runway to display. 
  refresh();

  // draw initial number of tours completed as 0
  updateTours(0);
}

void
AirportAnimator::end()
{
  // lock up screen
  lockScreen();

  erase();  // blank the screen
  endwin(); // return to regular terminal 

  // unlock the screen. 
  unlockScreen();
}

void
AirportAnimator::taxiOut(int plane_num)
{
  // find column number for airplane.
  int col = plane_num*8+left_base+2;

  // lock up screen so we can draw
  lockScreen();

  // draw the airplane heading out
  mvprintw(3, col, "\\%1d/", plane_num);
  mvprintw(4, col, " |");
  refresh();

  // let others draw if they want to!
  unlockScreen();

  // move the location of this airpalne toward runway
  for (int r=3; r<=12; r++)
    {
      // lock screen so we can draw on it
      lockScreen();

      // redraw the location of the airplane, leaving blanks "behind" it. 
      mvprintw(r, col, "   ");
      mvprintw(r+1, col, "\\%1d/", plane_num);
      mvprintw(r+2, col, " |");
      refresh();      

      // let others draw
      unlockScreen();

      // show slight delay for each bit of taxi-ing the airplane does. 
      usleep(7E5);
    }
}

void
AirportAnimator::takeoff(int plane_num)
{
  int col = plane_num*8+left_base+2;
  
  // clear out plane that is doen taxi-ing 
  lockScreen();

  mvprintw(13, col, "   ");
  mvprintw(14, col, "   ");
  refresh();

  WINDOW *planeWin = newwin(3,9, 16, 10);
  
  mvwaddstr(planeWin, 0,1, "   \\\\");

  stringstream fuselage;
  fuselage <<  ">)" << plane_num << "))))";
  mvwaddstr(planeWin, 1,1, fuselage.str().c_str());

  mvwaddstr(planeWin, 2,1, "   //");
  wrefresh(planeWin);

  unlockScreen();
  
  long int sleep_time=100000;
  for(int col=10; col<=65; col++)
    {
      int tts=sleep_time;

      usleep(tts);
      
      sleep_time= (int) (sleep_time/1.05);

      lockScreen();

      mvwin(planeWin, 16, col);

      wrefresh(planeWin);
      unlockScreen();
    }

  lockScreen();

  wclear(planeWin);
  wrefresh(planeWin);
  delwin(planeWin);  
  
  unlockScreen();
}

void
AirportAnimator::land(int plane_num)
{
  lockScreen();
  WINDOW *planeWin = newwin(3,9, 16, 10);

  mvwaddstr(planeWin, 0,0, "   //");
  
  stringstream fuselage;
  fuselage <<  "((((";
  fuselage << plane_num;
  fuselage <<  "(<";				      
  mvwaddstr(planeWin, 1,1, fuselage.str().c_str());
  mvwaddstr(planeWin, 2,0, "   \\\\");
  unlockScreen();
  
  int sleep_time=10000;
  for(int col=65; col>=13; col--)
    {
      int tts=sleep_time;

      usleep(tts);
      
      sleep_time= (int) (sleep_time*1.07);

      lockScreen();
      mvwin(planeWin, 16,col);      

      wrefresh(planeWin);
      unlockScreen();
    }

  lockScreen();
  werase(planeWin);
  wrefresh(planeWin);
  delwin(planeWin);
  unlockScreen();
}



void
AirportAnimator::taxiIn(int plane_num)
{
  int col = plane_num*8+left_base+2;
  
  lockScreen();
  mvprintw(13, col, " |");
  mvprintw(14, col, "/%1d\\", plane_num);
  refresh();
  unlockScreen();
  
  for (int r=12; r>=3; r--)
    {
      usleep(7E5);
      lockScreen();


      mvprintw(r, col, " |");
      mvprintw(r+1, col, "/%1d\\", plane_num);
      mvprintw(r+2, col, "   ");

      refresh();      
      
      unlockScreen();

  
    }
}


void
AirportAnimator::updateStatus(int plane_num, string status)
{
  lockScreen();

  mvprintw(2, left_base+8*plane_num, "[%5s]", status.c_str());
  refresh();

  unlockScreen();
}


void
AirportAnimator::updatePassengers(int plane_num, int numpassengers)
{
  lockScreen();


  mvprintw(1, left_base+8*plane_num, "Pass:%2d", numpassengers);
  touchwin(stdscr);
  refresh();

  unlockScreen();
}

void
AirportAnimator::updateTours(int num_tours)
{
  lockScreen();

  mvprintw(20, 20, "Tours Completed: %4d", num_tours);
  refresh();

  unlockScreen();
}

