
#include "process_data.h"

void car_controller_set_control_data(struct sensor_data  const* sdata,
                                     struct control_data* cdata)
{
    rotation_type  rotation_speed = 0;
    const unsigned threshold = 200;
    unsigned direction = 0;

    unsigned Lp = sdata->sensor[0] + sdata->sensor[1] + sdata->sensor[3]  + sdata->sensor[5];
    unsigned Ln = sdata->sensor[5] + sdata->sensor[7] + sdata->sensor[9]  + sdata->sensor[11];
    unsigned Rp = sdata->sensor[0] + sdata->sensor[2] + sdata->sensor[4]  + sdata->sensor[6];
    unsigned Rn = sdata->sensor[6] + sdata->sensor[8] + sdata->sensor[10] + sdata->sensor[11];

    if ((sdata->sensor[0] >= threshold && sdata->sensor[11] >= threshold))
    { // go straight
      rotation_speed = 0;
      direction = 0;
    } else {
      if ((Lp + Ln) > (Rp + Rn)) {
        rotation_speed =  -angle_speed;  // left curve
        direction = 1;
      } else if ((Rp + Rn) > (Lp + Ln)) {
        rotation_speed =  +angle_speed;  // right curve
        direction = 2;
      } else if ((Lp + Rn) > (Rp + Ln)) {
        rotation_speed =  -angle_speed; // left curve
        direction = 1;
      } else if ((Rp + Ln) > (Lp + Rn)) {
        rotation_speed =  angle_speed; // right curve
        direction = 2;
      } else if ((Rp+Ln+Lp+Rn) < (16*threshold)) {
        if (direction == 1) rotation_speed = -angle_speed;
        else if (direction == 2) rotation_speed = angle_speed;
        else if (direction == 0) rotation_speed = 0;
      }
    }

    if (sdata->movement != 0)
      cdata->movement = sdata->movement; // dont change movement speed
    else
      cdata->movement = default_speed; // initially set movement speed
    cdata->rotation = rotation_speed;
}

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
