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
#include "Plan.h"

using namespace std;
using namespace rt;

void addBubble( Scene& scene, Point3 c, Real r, Material transp_m );
void prgm0(Scene& scene);
void prgm1(Scene& scene);
void prgm2(Scene& scene);
void prgm3(Scene& scene);
void prgm4(Scene& scene);

int main(int argc, char** argv)
{
  // Read command lines arguments.
  QApplication application(argc,argv);
  
  // Creates a 3D scene
  Scene scene;

  if( argc > 2 ) {
    scene.loadFromFile(argv[1], argv[2]);
  } else {
    prgm3(scene);
  }
  
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

void prgm4(Scene& scene)
{
  scene.loadFromFile( "scene1", "materials" );
}

void prgm3(Scene& scene)
{
  addBubble( scene, Point3( -5, 4, -1 ), 2.0, Material::glass() );
  prgm2(scene);
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


void addBubble( Scene& scene, Point3 c, Real r, Material transp_m )
{
  Material revert_m = transp_m;
  std::swap( revert_m.in_refractive_index, revert_m.out_refractive_index );
  Sphere* sphere_out = new Sphere( c, r, transp_m );
  Sphere* sphere_in  = new Sphere( c, r-0.02f, revert_m );
  scene.addObject( sphere_out );
  scene.addObject( sphere_in );
}

