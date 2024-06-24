# Questions for Tutor

## Assignment 4, on 17.06.2024
- [ ] `accumulator`
  - [ ] Why can't we bind to the three ports and channel.write() here, so that all three ports receive the data?
  - Seems like Signal Channel only supports one port on each side, i.e., two in total. 
    - [ ] How to make port have 2 connections to channels, or stated differently, allow channel to be connected to two ports?
- [ ] `line-follower`
  - [ ] What time unit should be used for wait() in car, so for consuming e.get_timer()?
  - [ ] How is made sure that Car fires sensor data first?
  - [ ] The solution actually passes no time as well

## Assignment 5, on 17.06.2024
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
- [ ] Why bind the module to itself with `out(*this)`?