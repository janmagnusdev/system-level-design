/*
 * events.h
 *
 *  Created on: 08.01.2009
 *      Author: maha
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <irrlicht/irrlicht.h>

class MyEventReceiver : public irr::IEventReceiver
{
public:
        // This is the one method that we have to implement
        virtual bool OnEvent(const irr::SEvent& event)
        {
                // Remember whether each key is down or up
                if (event.EventType == irr::EET_KEY_INPUT_EVENT)
                        KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

                return false;
        }

        // This is used to check whether a key is being held down
        bool IsKeyDown(irr::EKEY_CODE keyCode) const
        {
                return KeyIsDown[keyCode];
        }
        bool IsKeyUp(irr::EKEY_CODE keyCode) const
        {
                return !KeyIsDown[keyCode];
        }

        MyEventReceiver()
        {
                for (irr::u32 i=0; i<irr::KEY_KEY_CODES_COUNT; ++i)
                        KeyIsDown[i] = false;
        }

private:
        // We use this array to store the current state of each key
        bool KeyIsDown[irr::KEY_KEY_CODES_COUNT];
};


#endif /* EVENTS_H_ */

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
