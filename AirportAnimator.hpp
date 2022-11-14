/*
 * FILE: AirportAnimator.hpp
 * Author: S. Blythe
 * DATE: 7/2022  
 *
 * provides the animation of airplanes fro project 2
 */
#ifndef _AIRPORT_ANIMATOR_HPP_
#define _AIRPORT_ANIMATOR_HPP_

#include <string>

class AirportAnimator
{
public:
  /* initializes screen and draws airport, taxiways, and runway */
  /* DO NOT PRINT *ANYTHING* TO THE SCREEN AFTER CALLING THIS,  */
  /*   WITH THE EXCEPTION OF THE OTHER METHODS IN THIS CLASS    */
  static void init();

  /* clean up the screen and get rid of the airport on it. */
  static void end();

  /* draw plane number [plane_num] taxi-ing from the airport to the runway. */
  static void taxiOut(int plane_num);

  /* draw plane number [plane_num] taking off. */
  static void takeoff(int plane_num);

  /* draw plane number [plane_num] landing. */
  static void land(int plane_num);

  /* draw plane number [plane_num] taxiing from the runway to the airport. */
  static void taxiIn(int plane_num);

  /* change the status message for plane number [plane_num] to be [status] */
  static void updateStatus(int plane_num, std::string status);

  /* change #passengers on board plane [plane_num] to be [numpassengers] */
  static void updatePassengers(int plane_num, int numpassengers);

  /* print the total number of tours [num_tours] to the screen */
  static void updateTours(int num_tours);
};

#endif
