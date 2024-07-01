# Questions

## During the Tutorium/ Course
### Assignment 4, on 17.06.2024
- [ ] `accumulator`
  - [ ] Why can't we bind to the three ports and channel.write() here, so that all three ports receive the data?
  - Seems like Signal Channel only supports one port on each side, i.e., two in total. 
    - [ ] How to make port have 2 connections to channels, or stated differently, allow channel to be connected to two ports?
- [ ] `line-follower`
  - [ ] What time unit should be used for wait() in car, so for consuming e.get_timer()?
  - [ ] How is made sure that Car fires sensor data first?
  - [ ] The solution actually passes no time as well

### Assignment 5, on 17.06.2024
- [x] Exports in Practice?
  - Adapter, da ist immer ein Export gefragt
  - Port Channel Port; horizontales Level
  - Vertikale Ebene -> Exports; dieselbe Ebene also
  - Binding von Ports/ Exports in Bezug auf Hierarchien:
    - Ports:
      - Ports auf höherer Ebene
      - Channels
      - Exports auf derselben Ebene
    - Exports:
      - Exports at lower hierarchy levels
      - Channels
    - Auch auf Folie Nummer 141 zu finden!
- [x] Why bind the module to itself with `out(*this)`?
  - required, so that the exports are connected on both sides
  - [x] Warum müssen Export-Blöcke zwangsläufig an sich selbst gebindet werden, und wann spezifisch nicht?
  - [x] Und warum muss man das bei `Ports` nicht machen?
- [x] Adapter vs. Transactor
  - transactor is a specific type of adapter, which translates higher level interfaces to lower level and vice versa
- [ ] How does UART work?


### Assignments 6, on 24.06.2024
- Für diese Zeit erstmal nur Aufgabe 1 und 2a); der Rest kommt dann später
- [ ] Wie funktioniert TLM?
  - VHDL, FPGAs, IP Komponenten (Bussysteme), Code Fragmente die wir selbst machen wollen
  - Verilog und VHDL sind zu spezifisch, TLM abstrahiert vieles weg, was wir sonst nervig manuell machen müssen
- [ ] Der RAM hat in Assignment 6 eine Breite von 16, und der Datentyp ist `unsigned`, also `unsigned int`
  - Also haben wir einen 16 * sizeof(unsigned) Breiten RAM
  - Die Breite der Datentypen ist dabei frei definierbar, man könnte `unsigned` also durch arbitrary structs ersetzen

## From working on the Exercises outside the Course
### Assignment 5, on 17.06.2024
- [ ] what does wait() (wait call without parameters) actually do?
  - in this case, it waits for the next clock cycle, since we registered forward() on the clock posedge() event
- [ ] why do we need the to_byte() call?
- [ ] module vs. prim channel
  - [ ] sensor_fifo was a regular SystemC module - with exports
  - [ ] control_fifo however will be a primitive channel which we will implement ourselves
  - how do sensor_fifo and control_fifo differ now, really? Why should we write control_fifo as a primitive channel, when sensor_fifo is a normal module which is just as sufficient?
- [ ] How **can** the Synchronization be solved cleverly?
  - probably with events?
  - what synchronization, anyway?
- [ ] Why are the the local events like `sc_core::sc_event m_data_written_event;` not initialised somewhere? does SystemC do it automatically?

- Note: I would not be able to do anything of this without al the comments lying around - which is quite frustrating. This is very complex, and I can barely grasp the core ideas. Maybe I should have listened more in the lectures...

# Important Topics for Exam
- a
- b
- c