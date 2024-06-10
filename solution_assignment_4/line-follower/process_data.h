#ifndef PROCESS_DATA_H_
#define PROCESS_DATA_H_

/* Include {control,sensor}_data types */
#if defined( TARGET_IMPL )
#include "iss/data_types.h"
#else
#include "data_types.h"
#endif


static const movement_type
default_speed = 0.005f;

static const rotation_type
angle_speed = 0.025f;

#ifdef __cplusplus
extern "C" {
#endif

void car_controller_set_control_data(struct sensor_data const* sdata,
                                     struct control_data* cdata);

#ifdef __cplusplus
}
#endif

#endif

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
