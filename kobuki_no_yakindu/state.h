#ifndef _STATE_H
#define _STATE_H

typedef enum {
	Work = 0,
	Rotating,
	// Path Finding Here,

  //end here
  Tail,
} state_t;

void fsm_loop(void);

#endif