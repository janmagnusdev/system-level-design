#ifndef SIMULATION_H_
#define SIMULATION_H_

#include <vector>
#include <stdint.h>
#include <string>

const unsigned NUMBER_OF_SENSORS = 12;

namespace irr 
{
  class IrrlichtDevice;
  namespace video 
  {
    class IVideoDriver;
  }
  namespace scene 
  {
    class ISceneManager;
    class ICameraSceneNode;
  }
  namespace gui {
    class IGUIEnvironment;
    class IGUIStaticText;
  }
}

class MyEventReceiver;
class AutoModel;

class simulation_engine
{
 public:
  simulation_engine(const std::string& scenery_base_path);
  ~simulation_engine();

  bool running();
  void init();
  bool run();

  bool RCMode()
  {
    return RCMode_;
  }

  uint32_t get_timer();

  void set_movement(const float& mov)
  {
    movement_speed = mov;
  }

  float get_movement()
  {
    return movement_speed;
  }

  void set_rotation(const float& rot)
  {
    rotation_speed = rot;
  }

  const std::vector<float>& getSensors();

 private:
  void render();
  void posUpdate();
  void printInfo();

  // sensor data
  std::vector<float> sensors;

  irr::IrrlichtDevice* device;
  irr::video::IVideoDriver* driver;
  irr::scene::ISceneManager* scenemgr;
  irr::scene::ICameraSceneNode* camera;
  irr::gui::IGUIEnvironment* env;
  irr::gui::IGUIStaticText* console;
  AutoModel* autonode;
  MyEventReceiver* receiver;
  float movement_speed;
  float rotation_speed;

  uint32_t lasttime;
  uint32_t virtual_time;
  bool active;
  bool break_flag;

  bool LostTrack;
  uint32_t Direction;
  bool RCMode_;
  bool flagRC;
  bool flagAT;
  std::string base_path;
};

#endif

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
