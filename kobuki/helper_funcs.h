#ifndef HELPER_FUNCS_H_
#define HELPER_FUNCS_H_

#include <stdint.h>
#include <stdbool.h>

extern float normalize_rot(float desired, float now);

extern float update_dist(float dist, uint16_t prev_encoder, bool is_forward);

extern float read_tilt_theta(void);
extern float read_tilt_psi(void);

extern float get_abs(float var);

#endif /* HELPER_FUNCS_H_ */
