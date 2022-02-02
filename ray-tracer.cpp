#include <qapplication.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "Viewer.h"
#include "Scene.h"
#include "Sphere.h"
#include "Material.h"
#include "PointLight.h"

using namespace std;
using namespace rt;

void prgm0(Scene& scene);
void prgm1(Scene& scene);
void prgm2(Scene& scene);

int main(int argc, char** argv)
{
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  // Creates a 3D scene
  Scene scene;

  prgm2(scene);
  
  // Instantiate the viewer.
  Viewer viewer;
  // Give a name
  viewer.setWindowTitle("Ray-tracer preview");

  // Sets the scene
  viewer.setScene( scene );

  // Make the viewer window visible on screen.
  viewer.show();
  // Run main loop.
  application.exec();
  return 0;
}

void prgm2(Scene& scene)
{
  // Light at infinity
  Light* light0 = new PointLight( GL_LIGHT0, Point4( 0,0,1,0 ),
    Color( 1.0, 1.0, 1.0 ) );
  Light* light1 = new PointLight( GL_LIGHT1, Point4( -10,-4,2,1 ),
    Color( 1.0, 1.0, 1.0 ) );
  scene.addLight( light0 );
  scene.addLight( light1 );

  // Objects
  Sphere* sphere1 = new Sphere( Point3( 0, 0, 0), 2.0, Material::bronze() );
  Sphere* sphere2 = new Sphere( Point3( 0, 4, 0), 1.0, Material::emerald() );
  Sphere* sphere3 = new Sphere( Point3( 6, 6, 0), 3.0, Material::whitePlastic() );
  scene.addObject( sphere1 );
  scene.addObject( sphere2 );
  scene.addObject( sphere3 );
}

void prgm1(Scene& scene)
{
  // Light at infinity
  Light* light0 = new PointLight( GL_LIGHT0, Point4( 0,0,1,0 ),
                                    Color( 1.0, 1.0, 1.0 ) );
  Light* light1 = new PointLight( GL_LIGHT1, Point4( 7,5,15,1 ),
                                  Color( 1.0, 1.0, 1.0 ) );
  scene.addLight( light0 );
  scene.addLight( light1 );

  // Objects
  Sphere* sphere1 = new Sphere( Point3( 0, 0, 0), 2.0, Material::bronze() );
  Sphere* sphere2 = new Sphere( Point3( 0, 4, 0), 1.0, Material::emerald() );
  Sphere* sphere3 = new Sphere( Point3( 2, 2, 4), 1.0, Material::emerald() );
  scene.addObject( sphere1 );
  scene.addObject( sphere2 );
  scene.addObject( sphere3 );
}

void prgm0(Scene& scene)
{
  // Light at infinity
  Light* light0 = new PointLight( GL_LIGHT0, Point4( 0,0,1,0 ),
                                    Color( 1.0, 1.0, 1.0 ) );
  Light* light1 = new PointLight( GL_LIGHT1, Point4( 7,5,15,1 ),
                                  Color( 1.0, 1.0, 1.0 ) );
  scene.addLight( light0 );
  scene.addLight( light1 );

  // Objects
  Sphere* sphere1 = new Sphere( Point3( 0, 0, 0), 2.0, Material::bronze() );
  Sphere* sphere2 = new Sphere( Point3( 0, 4, 0), 1.0, Material::emerald() );
  scene.addObject( sphere1 );
  scene.addObject( sphere2 );
}
