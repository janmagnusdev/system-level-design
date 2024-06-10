#include "simulation.h"
#include "auto.h"
#include "events.h"

using namespace irr;
typedef core::dimension2d<u32> dim_type;

// manual override with keys
#define MANUAL_STEERING 
// use the 12 sensor version with autopilot
#define SENSOR12
#define SENSOR_TEST

static const video::E_DRIVER_TYPE irr_driver  = irr::video::EDT_BURNINGSVIDEO;
static const bool                 fullscreen  = false;
static const dim_type             window_size = (fullscreen) 
                                                ? dim_type( 1680,1050 )
                                                : dim_type(  800, 600 );

//Destruktor Irrlicht killen!
simulation_engine::~simulation_engine()
{
	delete autonode;
	//taction->~TerminalAction();
	if(device)
		device->drop();
}

//Konstruktor Irrlicht initialisieren!
simulation_engine::simulation_engine(const std::string& scenery_base_path) :
	device(0), driver(0), scenemgr(0), camera(0), autonode(0),
	LostTrack(false), Direction(0), RCMode_(false), base_path(scenery_base_path)
{
  receiver = new MyEventReceiver;
  device = createDevice( irr_driver, window_size
                       , 32, fullscreen, false, false, receiver );
  if (!device) return;
  driver = device->getVideoDriver();
  env = device->getGUIEnvironment();
  gui::IGUISkin* skin = env->getSkin();
  skin->setFont(env->getFont((base_path+"fonthaettenschweiler.bmp").c_str()));

  console = env->addStaticText(L"Test", core::rect<s32>(10,475,790,590), true,true,0,-1,true);

  scenemgr = device->getSceneManager();

  device->setWindowCaption(L"3D-Environment: Line Follower");
  device->getCursorControl()->setVisible(false); // : Mauszeiger Unsichtbar werden lassen

  scene::ILightSceneNode* sun = scenemgr->addLightSceneNode(0, core::vector3df(-1000.0f,1000.0f,0.0f), video::SColorf(0.9f,0.9f,0.9f));
  if(sun) {
    video::SLight& lightdata = sun->getLightData();
    lightdata.AmbientColor = video::SColorf(0.4f,0.4f,0.4f);
    sun->setRotation(core::vector3df(40.0f,180.0f,0.0f));
    sun->setLightType(video::ELT_DIRECTIONAL);
  }
  scenemgr->setAmbientLight(video::SColor(0xff222222));

  scenemgr->loadScene((base_path+"umgebung2.irr").c_str());

  driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);
  scenemgr->addSkyDomeSceneNode(driver->getTexture((base_path+"skydome.jpg").c_str()));
  driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);

  camera = scenemgr->addCameraSceneNodeFPS(0,100.0f, 0.20f);
  if (camera)
    camera->setPosition(core::vector3df(0.0f,40.0f,-20.0f));

  autonode = new AutoModel(scenemgr, console, base_path);
}

uint32_t simulation_engine::get_timer()
{
  return virtual_time;
}

bool simulation_engine::running()
 {
    return device && break_flag;
}

void simulation_engine::init()
{
  lasttime = device->getTimer()->getRealTime();
  active=true;
  movement_speed = 0.f;
  rotation_speed = 0.f;
  break_flag = false;
  virtual_time = 0;
}

bool simulation_engine::run()
{
  if (!device || break_flag )
    return false;

  device->run();

  // switch between RC mode (manual mode) and automatic mode
  if (RCMode_ && !flagRC) {
    autonode->getCarSceneNode()->setMaterialTexture(0,(driver->getTexture((base_path+"fskinM.jpg").c_str())));
    flagRC = true;
    flagAT = false;
  } else if(!(RCMode_) && !flagAT) {
    autonode->getCarSceneNode()->setMaterialTexture(0,(driver->getTexture((base_path+"fskin.jpg").c_str())));
    flagAT = true;
    flagRC = false;
  }

  posUpdate();
  render();
  printInfo();

  return true;
}

void simulation_engine::printInfo()
{
    const std::vector<float>& sen = getSensors();
    core::stringw out = L"Sensor Values:";

    for (u32 i=0; i<sen.size(); ++i) {
      out += L"   ";
      // out += i+1;
      //out += L"_{ ";	
      if (sen[i] < 10) 
        out += L"00"; // ist der Sensorwert kleiner 10, also einstellig, so werden vor die ausgabe zwei Nullen gesetzt
      else if  (sen[i] < 100) 
        out+= L"0";  // ist der Sensorwert kleiner 100 und gr��er/gleich 10, so ist er zweistellig 

      out += int(sen[i]);
      //out += L" }";
    }
    //if(!RCMode){
    //}
    out+=L"\n\n";

    if (rotation_speed == 0)
      out += L"Current Direction: [  |  (STRAIGHT)";
    else if(rotation_speed < 0)
      out += L"Current Direction: [  \\  (LEFT)";
    else if(rotation_speed > 0)
      out += L"Current Direction: [  /  (RIGHT)";
      out+=L"\nCurrent Speed: ";
      out+= movement_speed*1000; 
      out+= L" x/s\n";
    if(RCMode_)
      out += L"Active Mode: MANUAL\n";
    else
      out += L"Active Mode: AUTONOMIC\n";
    out+= L"Active Algorithm: SIMPLE \n\n";
    if (LostTrack) {
      out += L" (!) Car left road! - ";
      if (Direction == 2)
        out += L"Guessing right curve";
      else if (Direction == 1)
        out += L"Guessing left curve ";
      else if (Direction == 0)
        out += L"Road line seems to be broken!";
     }

     console->setText(out.c_str());
}

void simulation_engine::posUpdate()
{

	// elapsed sets the time per frame. As slow rendering affects sensor
	// feedback and can thus make correct line-follower implementations seem to
	// fail, we use a fixed time step per frame.
	const u32 elapsed = 40; // nominal 25 FPS

	// Wait until at least elapsed ms have passed. On fast machines, this
	// ensures output is in real time. On slow machines (e.g. networked X), this
	// is a no-op and output will be slower than real-time.
	u32 now = device->getTimer()->getRealTime();
	struct timespec one_ms;
	one_ms.tv_sec = 0;
	one_ms.tv_nsec = 1000000;
	while (now-lasttime < elapsed) {
		nanosleep(&one_ms, NULL);
		now = device->getTimer()->getRealTime();
	}
	lasttime = now;

	virtual_time += elapsed;

	if(receiver->IsKeyDown(irr::KEY_F1))
	{
		active = !active;
		camera->setInputReceiverEnabled(active);
	}
	if(receiver->IsKeyDown(irr::KEY_ESCAPE))
	{
          break_flag = true;
          return;
	}

	core::vector3df nodePosition = autonode->getSceneNode()->getPosition();
	core::vector3df nodeRotation = autonode->getSceneNode()->getRotation();

	//------------------[HOTKEYS FUER MANUELLE STEUERUNG <anfang>]-------------
	if(receiver->IsKeyDown(irr::KEY_KEY_W))      // : irr::KEY_KEY_I (d)
		movement_speed+=.001f;                  //
	else if(receiver->IsKeyDown(irr::KEY_KEY_S)) // : irr::KEY_KEY_K (d#ifdef STEUERUNG_TEST
		movement_speed-=.001f;                  // ----------------------------
	if(receiver->IsKeyDown(irr::KEY_KEY_A))      // : irr:KEY_KEY_J  (d)
		rotation_speed-=.001f;                  //
	else if(receiver->IsKeyDown(irr::KEY_KEY_D)) // : irr::KEY_KEY_L (d)
		rotation_speed+=.001f;                  //  . . . . . . . . . . . . . .  
	else if(RCMode_ && receiver->IsKeyUp(irr::KEY_KEY_J))   // : irr::KEY_KEY_J (u)          | Selektive Bedingung ist hier (so ziemlich)
		rotation_speed=0.f;                     //                               | immer richtig, somit wird die Rotationsstaerke    
	else if(RCMode_ && receiver->IsKeyUp(irr::KEY_KEY_L))   // : irr::KEY_KEY_L (u)          | schlie�lich stetig auf 0 gesetzt 
		rotation_speed=0.f;                     // ----------------------------
	// : Geschwindigkeit ist noch relativ schwer zu kontrollieren, folglich
	//   wird eine Taste f�r "Nullschub" (hier = Q) eingef�hrt
	if(receiver->IsKeyDown(irr::KEY_KEY_Q))          movement_speed = 0.f;    
	// : vordefinierte Geschwindigkeiten im "groben" Wertebereich (+)
	else if(receiver->IsKeyDown(irr::KEY_KEY_1))     movement_speed =  0.01f;   
	else if(receiver->IsKeyDown(irr::KEY_KEY_2))     movement_speed =  0.02f;  
	else if(receiver->IsKeyDown(irr::KEY_KEY_3))     movement_speed =  0.03f;  
	else if(receiver->IsKeyDown(irr::KEY_KEY_4))     movement_speed =  0.04f;  
	else if(receiver->IsKeyDown(irr::KEY_KEY_5))     movement_speed =  0.05f;  
	// : vordefinierte Geschwindikigkeiten (0.+)
	else if(receiver->IsKeyDown(irr::KEY_KEY_6))     movement_speed =  0.001f;   
	else if(receiver->IsKeyDown(irr::KEY_KEY_7))     movement_speed =  0.002f;  
	else if(receiver->IsKeyDown(irr::KEY_KEY_8))     movement_speed =  0.004f;  
	else if(receiver->IsKeyDown(irr::KEY_KEY_9))     movement_speed =  0.006f;  
	else if(receiver->IsKeyDown(irr::KEY_KEY_0))     movement_speed =  0.008f;  
	// : vordefinierte Geschwindigkeiten (kleinerer Betrag)
	else if(receiver->IsKeyDown(irr::KEY_KEY_E)){    RCMode_ = true; movement_speed = 0.0f;}
	else if(receiver->IsKeyDown(irr::KEY_KEY_R))     RCMode_ = false;
	else if(receiver->IsKeyDown(irr::KEY_SPACE))     Direction = 0;
	// : Autopoistion wieder auf Koordinatenursprung der Welt zur�cksetzen
	if(receiver->IsKeyDown(irr::KEY_KEY_U)){
		movement_speed = 0.0f;
		nodePosition = core::vector3df(0.0f,0.0f,0.0f);
		nodeRotation = core::vector3df(0.0f,0.0f,0.0f);
	}
	else if(receiver->IsKeyDown(irr::KEY_KEY_P)) {
		RCMode_ = !RCMode_;
	}
	// : Algorithmus umstellen:
//	else if(receiver->IsKeyDown(irr::KEY_KEY_F)) Algorithm = Standard;
//	else if(receiver->IsKeyDown(irr::KEY_KEY_G)) Algorithm = Rauschen;
	
	//------------------[HOTKEYS FUER MANUELLE STEUERUNG <ende>]----------------
	

	nodeRotation.Y += rotation_speed * elapsed;
	core::matrix4 mat;
	mat.setRotationDegrees(nodeRotation);
	core::vector3df tmp(1,0,0);
	mat.transformVect(tmp);
	nodePosition += (movement_speed * elapsed)*tmp;

	autonode->getSceneNode()->setRotation(nodeRotation);
	autonode->getSceneNode()->setPosition(nodePosition);
	autonode->getSceneNode()->updateAbsolutePosition();
	autonode->getCamera(0)->updateAbsolutePosition();
	autonode->getCamera(0)->setUpVector(tmp);
	autonode->update();
	camera->updateAbsolutePosition();
	camera->setTarget(nodePosition);
}


void simulation_engine::render()
{
	driver->beginScene(true,true,video::SColor(255,0,0,255));
	//Camera rendern
	driver->setRenderTarget(autonode->getCameraTexture(0), true,true,video::SColor(255,0,0,255));
	scenemgr->setActiveCamera(autonode->getCamera(0));
	autonode->getCarSceneNode()->setVisible(false);
	scenemgr->drawAll();
	autonode->getCarSceneNode()->setVisible(true);

	// :Sensorkamera Camera rendern
	driver->setRenderTarget(0, true, true, video::SColor(255,0,0,255));
	scenemgr->setActiveCamera(this->camera);
	scenemgr->drawAll();
	env->drawAll();
	if (autonode)
	{
		driver->draw2DImage(autonode->getCameraTexture(0), core::position2d<s32>(0,0));
	}
	
	driver->endScene();		
}


const std::vector<float>& simulation_engine::getSensors()
{
	const u8 positions[] = {
#ifdef SENSOR12
						 31,10,
                     26,15, 36,15,
                   21,20,     41,20,
                 16,25,         46,25,
                    21,30,    41,30,
                     26,35, 36,35,
						 31,40
#else
                 16,25,         46,25,
#endif
	};

	const s8 offset[] = {0,0, 1,0, 0,1, 1,1};
	const u8 numSensors = sizeof(positions)/2;
	const u8 numOffsets = sizeof(offset)/2;

	sensors.clear();
	sensors.reserve(numSensors);
#if (IRRLICHT_VERSION_MAJOR == 1 && IRRLICHT_VERSION_MINOR >= 8)
	u32* data = (u32*) autonode->getCameraTexture(0)->lock( irr::video::ETLM_READ_ONLY );
#else
	u32* data = (u32*) autonode->getCameraTexture(0)->lock( true );
#endif
	const u32 width = autonode->getCameraTexture(0)->getSize().Width;
	for (u32 i=0; i<numSensors; ++i)
	{
		const u8 x=positions[2*i];
		const u8 y=positions[2*i+1];
		float tmp = 0.f;
		for (u32 j=0; j<numOffsets; ++j)
		{
			video::SColor color = data[ (y+offset[2*j+1])*width+x+offset[2*j] ];
			tmp += color.getLuminance();
		}
		sensors.push_back(tmp/4.f);
	}
	autonode->getCameraTexture(0)->unlock();
	return sensors;
}

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
