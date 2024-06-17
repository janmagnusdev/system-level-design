#ifndef DATA_TYPES_H_
#define DATA_TYPES_H_

#include <simulation.h>
#include <systemc>

typedef float movement_type;
typedef float sensor_type;
typedef float rotation_type;


/* ----- user defined class for sensor data ----- */
struct sensor_data {
    movement_type movement;
    sensor_type   sensor[NUMBER_OF_SENSORS];
};

/* ----- user defined class for control data ----- */
struct control_data {
    movement_type movement;
    rotation_type rotation;
};

/* ----- declaration of trace functions ----- */
void sc_trace(sc_core::sc_trace_file* f, const sensor_data& sd, const std::string& s);
void sc_trace(sc_core::sc_trace_file* f, const control_data& cd, const std::string& s) ;

/* ----- declaration of output stream operator ----- */
std::ostream& operator<<(std::ostream& os, const sensor_data& sd);
std::ostream& operator<<(std::ostream& os, const control_data& cd);

/* ----- declaration of operator== ----- */
bool operator==(const control_data& left, const control_data& right);
bool operator==(const sensor_data& left, const sensor_data& right);

#endif // DATA_TYPES_H_
