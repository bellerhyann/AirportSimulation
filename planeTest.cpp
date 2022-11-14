/*
 October 2022
 Written by Belle Erhardt
 Computer Systems 1
 */


#include "AirportAnimator.hpp"
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <iostream>
#include <sys/sem.h>
#include <sys/shm.h>
using namespace std;
//Varibles
#define NUM_SEMAPHORES 1
#define NUM_OPS 1

pthread_t threadData[8];
unsigned int passengers;
unsigned int tours;
unsigned int planeNum;
bool toursComplete;
int runway;         //this semaphoreAllows 1 plane to access runway at a time
int boarding;       //this semaphore Allows 1 thread to alter passengers at a time
int airTraffic;     //this semaphoreAllows 1 thread to alter tours at a time
int tourCount;

void* doThread(void* args)
{
    //set up wait for semaphores
    struct sembuf semWaitCommand[NUM_OPS];
    semWaitCommand[0].sem_num = 0; // do this to the first semaphore in set
    semWaitCommand[0].sem_op = -1; // standard wait (decr and go to queue)
    semWaitCommand[0].sem_flg = 0; // good default flags
    //set up signal for semaphores
    struct sembuf semSigCommand[NUM_OPS];
    semSigCommand[0].sem_num = 0; // do this to the first semaphore in set
    semSigCommand[0].sem_op = +1; // standard signal (incr and wake queue)
    semSigCommand[0].sem_flg = 0; // good default flags
    
    while (!toursComplete)
    {
        //prep the plane number as per the thread.
        int *myPtr = (int *)  args;
        int planeNum = *myPtr;
        //board the passengers on the plane
        for(int passenger=1; passenger<=12; passenger++)
        {
            semop(boarding, semWaitCommand, NUM_OPS);
            AirportAnimator::updatePassengers(planeNum, passenger);
            sleep(rand()%3);
        }
        AirportAnimator::updateStatus(planeNum, "TAXI");
        AirportAnimator::taxiOut(planeNum);
        
        semop(runway, semWaitCommand, NUM_OPS);
        AirportAnimator::updateStatus(planeNum, "TKOFF");
        AirportAnimator::takeoff(planeNum);
        semop(runway, semSigCommand, NUM_OPS);
        
        AirportAnimator::updateStatus(planeNum,"TOUR");
        sleep(15+rand()%31);
        AirportAnimator::updateStatus(planeNum,"LNDRQ");
        sleep(2);
        
        semop(runway, semWaitCommand, NUM_OPS);
        AirportAnimator::updateStatus(planeNum,"LAND");
        AirportAnimator::land(planeNum);
        semop(runway, semSigCommand, NUM_OPS);

        AirportAnimator::updateStatus(planeNum, "TAXI");
        AirportAnimator::taxiIn(planeNum);
        AirportAnimator::updateStatus(planeNum, "DEPLN");
        for(int passenger=11; passenger>=0; passenger--)
        {
            AirportAnimator::updatePassengers(planeNum, passenger);
            semop(boarding, semSigCommand, NUM_OPS);
            sleep(1);
        }

        AirportAnimator::updateStatus(planeNum, "DEPLN");
        tourCount++;
        AirportAnimator::updateTours( tourCount);
        
        semop(airTraffic, semWaitCommand, NUM_OPS);
        tours--;
        semop(airTraffic, semSigCommand, NUM_OPS);

        sleep(2);
        
        if(tours <= 0) //this allows each plane to finish once the goal tour number has been reached.
        {toursComplete= true;}
    }
    return NULL;
}




int main(int argc, char *argv[])
{
                                                //Identify Passengers & tours
    passengers = atoi(argv[1]);
    tours = atoi(argv[2]);
    toursComplete = false;
    tourCount = 0;
                                                //create and initalize semaphors
    airTraffic = semget(4, NUM_SEMAPHORES, IPC_CREAT|IPC_EXCL|0600 );
    if(airTraffic<0)
    {
        airTraffic = semget(4, NUM_SEMAPHORES, 0 );
    }
    semctl(airTraffic, 0 ,     SETVAL, 1);
    
    bookFlight = semget(1, NUM_SEMAPHORES, IPC_CREAT|IPC_EXCL|0600 );
    if(bookFlight<0)
    {
        bookFlight = semget(1, NUM_SEMAPHORES, 0 );
    }
    semctl(bookFlight, 0 ,     SETVAL, 1);
   
    runway = semget(2, NUM_SEMAPHORES, IPC_CREAT|IPC_EXCL|0600 ); // this semaphore allows one plane to use runway at a time.
    if(runway<0)
    {
        runway = semget(2, NUM_SEMAPHORES, 0 );
    }
    semctl(runway, 0 , SETVAL, 1);
    
    boarding = semget(3, NUM_SEMAPHORES, IPC_CREAT|IPC_EXCL|0600 );
    if(boarding<0)
    {
        boarding = semget(3, NUM_SEMAPHORES, 0 );
    }
    semctl(boarding, 0 ,  SETVAL, passengers); //<- important: allows each passenger to board 1 plane at a time.
                                            //Start Animation
    AirportAnimator::init();
                                            //create Threads
    int pNum;
    for(pNum=0; pNum<8; pNum++)
    {
        int *data = new int(pNum);
        pthread_create(&threadData[pNum], NULL, doThread, (void *) data); //unsure about the void* data portion of this line
    }
                                            //join Threads
    for(int tNum=0; tNum<8; tNum++)
        pthread_join(threadData[tNum], NULL);
                                            //end Animation
  AirportAnimator::end();
}
