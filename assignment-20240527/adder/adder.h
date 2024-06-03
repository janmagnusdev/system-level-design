#ifndef ADDER_H_
#define ADDER_H_

#include <systemc>

SC_MODULE( adder )
{
  /* ----- input ports ----- */

  /* ----- output ports ----- */


  /* ----- constructor ----- */
  SC_CTOR( adder )
  {
    /* ----- process definitions ----- */
  }

private:
  /* ----- process(es) ----- */
  void add();

}; // adder

#endif // ADDER_H_
