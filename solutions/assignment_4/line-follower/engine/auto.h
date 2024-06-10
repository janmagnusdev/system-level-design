#ifndef AUTO_H_
#define AUTO_H_

#include <irrlicht/irrlicht.h>
#include <vector>
#include <string>
#include <iostream>

using namespace irr;

class AutoModel
{
private:
	scene::ISceneNode* sceneNode;
	scene::ISceneNode* carNode;
	video::ITexture* camOutputTextures[2];
	scene::ICameraSceneNode* irrCameras[2];
	video::IVideoDriver* Driver;
	scene::ISceneManager* scenemgr;
	int id; //Nur für meldungen in der Konsole
	static int mainID;

public:
	AutoModel(scene::ISceneManager* scenemgr, gui::IGUIStaticText* console, const std::string& scenery_base_path);
	~AutoModel();

	void update(); //Positionen und Camera Targets updaten

	scene::ICameraSceneNode* getCamera(int number) const { return irrCameras[number]; }
	video::ITexture * getCameraTexture(int number) const { return camOutputTextures[number]; }
	int getID() const { return id;}
	scene::ISceneNode* getSceneNode() const { return sceneNode; }
	scene::ISceneNode* getCarSceneNode() const { return carNode; }
        std::string base_path;
};

#endif

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
