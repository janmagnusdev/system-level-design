#include "auto.h"
#include <iostream>
#include <cassert>

using namespace irr;


typedef core::dimension2d<u32> dim_type;

int AutoModel::mainID=0;
AutoModel::~AutoModel()
{
	if(camOutputTextures[0])
		Driver->removeTexture(camOutputTextures[0]);
	// entfernt auch alle Kinder
	if (sceneNode)
		sceneNode->remove();
}

AutoModel::AutoModel(scene::ISceneManager* _scenemgr
                    , gui::IGUIStaticText* /* unused */
                    , const std::string& scenery_base_path )
  : sceneNode(0)
  , carNode(0)
  , Driver(0)
  , scenemgr(_scenemgr)
  , base_path(scenery_base_path)
{
	id = ++mainID;

	Driver = scenemgr->getVideoDriver();

        std::cout << "create target texture" << std::endl;
	camOutputTextures[0] =
          Driver->addRenderTargetTexture( dim_type(64,64) );
        assert( camOutputTextures[0] );

	// Hauptknoten
	sceneNode = scenemgr->addEmptySceneNode();
	sceneNode->setPosition(core::vector3df(61.0f,0.0f,-65.0f));
        sceneNode->setRotation(core::vector3df(0,90,0));

	// Das Automodell
	bool carWithLight = true;

	carNode = scenemgr->addMeshSceneNode(scenemgr->getMesh((base_path+"f360.ms3d").c_str()), sceneNode);
	if(carNode)
	{
		carNode->setRotation(core::vector3df(0,-90,0));
		carNode->setScale(core::vector3df(.21f,.21f,.21f));
		carNode->setPosition(core::vector3df(0,3.0f,0));
		if (carWithLight)
		{
			carNode->setMaterialFlag(video::EMF_NORMALIZE_NORMALS, true);
			carNode->getMaterial(0).SpecularColor=0xff555555;
			carNode->getMaterial(0).Shininess = 128.0f;
			
		}
		else
			carNode->setMaterialFlag(video::EMF_LIGHTING, false);
	}

	//TODO: korrekte Position
	irrCameras[0] = scenemgr->addCameraSceneNode(sceneNode);
	dim_type size = camOutputTextures[0]->getSize();
	irrCameras[0]->setAspectRatio((f32) (size.Width / size.Height));
	irrCameras[0]->bindTargetAndRotation(true);
	irrCameras[0]->setPosition(core::vector3df(8,6,0));
	irrCameras[0]->setUpVector(core::vector3df(0,-1,0));

	update();
}

#if 0
void AutoModel::saveCams() const
{
	void* data = camOutputTextures[0]->lock(true);
	video::IImage* image = Driver->createImageFromData(camOutputTextures[0]->getColorFormat(),
		camOutputTextures[0]->getSize(), data, false, false);
	if (!image)
		std::cout << "failed image1" << std::endl;
	bool res = Driver->writeImageToFile(image, "links.ppm");
	if (!res)
		std::cout << "failed write1" << std::endl;
	image->drop();
	camOutputTextures[0]->unlock();

	data = camOutputTextures[1]->lock(true);
	image = Driver->createImageFromData(camOutputTextures[1]->getColorFormat(),
		camOutputTextures[1]->getSize(), data, false, false);
	if (!image)
		std::cout << "failed image2" << std::endl;
	res=Driver->writeImageToFile(image, "rechts.ppm");
	if (!res)
		std::cout << "failed write2" << std::endl;
	image->drop();
	camOutputTextures[1]->unlock();
}
#endif

void AutoModel::update()
{
	irrCameras[0]->setTarget(irrCameras[0]->getAbsolutePosition()+core::vector3df(0,-5,0));
}

// :tag: (exercise1,s) (exercise2,s) (exercise4,s)
