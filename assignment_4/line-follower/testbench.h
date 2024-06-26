#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include "car.h"
#include "car_controller.h"
#include "data_types.h"


#include <systemc>

SC_MODULE(testbench)
{
public:
    /* DONE: ----- submodules ----- */
    car m_car;
    car_controller m_car_controller;

    /* DONE: ----- fifo channels ----- */
    sc_core::sc_fifo<sensor_data> fifo_car_to_controller;
    sc_core::sc_fifo<control_data> fifo_controller_to_car;

    /* ----- constructor ----- */
    SC_CTOR(testbench)
    /* DONE: ----- initialiser list ----- */
    : m_car("m_car"),
      m_car_controller("m_car_controller")
    {
        /* DONE: ----- port to channel binding ----- */
        // round trip of binding
        m_car.sensors(fifo_car_to_controller);
        m_car_controller.sensors(fifo_car_to_controller);
        m_car_controller.control(fifo_controller_to_car);
        m_car.control(fifo_controller_to_car);
    }

};

#endif // TESTBENCH_H_
