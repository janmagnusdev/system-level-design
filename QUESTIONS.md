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