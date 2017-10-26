#define GL_GLEXT_PROTOTYPES

#include <GL/glu.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "mesh/Mesh.hxx"
#include "shader/ShaderProgram.hxx"
#include "shader/Shader.hxx"

void displayToonMesh(
    Mesh* mesh, GLuint blackBorderProgramID, GLuint celShadingProgramID,
    GLfloat tx, GLfloat ty, GLfloat tz,
    GLfloat angle, GLfloat rx, GLfloat ry, GLfloat rz){
  glPushMatrix();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(tx, ty, tz);
  glRotatef(angle, rx, ry, rz);

  // Display black borders
  glUseProgram(blackBorderProgramID);
  glCullFace(GL_FRONT);
  mesh->draw();

  // Display cel-shading mesh
  glUseProgram(celShadingProgramID);
  glCullFace(GL_BACK);
  mesh->draw();

  glPopMatrix();
}


int main(){
  // Create window
  sf::ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antialiasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 0;

  int width(800);
  int height(600);
  sf::Window window(
      sf::VideoMode(width, height),
      "3DToonChess",
      sf::Style::Default,
      settings
  );
  window.setVerticalSyncEnabled(true);
  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Enable backface culling
  glEnable(GL_CULL_FACE);
  glClearColor(1, 1, 1, 1);

  // Create projection matrix
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(70, (double)width/height, 1, 1000);

  // Load cel-shading shaders
  Shader* celShadingVertexShader = new Shader(
    "../shaders/celShadingVertexShader.glsl",
    GL_VERTEX_SHADER
  );

  Shader* celShadingFragmentShader = new Shader(
    "../shaders/celShadingFragmentShader.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> celShadingShaders = {
    celShadingVertexShader, celShadingFragmentShader};
  ShaderProgram* celShadingShaderProgram = new ShaderProgram(celShadingShaders);

  // Try to compile shaders
  bool compilationIsSuccess = celShadingShaderProgram->compile();
  if(!compilationIsSuccess){
    delete celShadingShaderProgram;

    return 1;
  }

  // Load black border shaders
  Shader* blackBorderVertexShader = new Shader(
    "../shaders/blackBorderVertexShader.glsl",
    GL_VERTEX_SHADER
  );

  Shader* blackBorderFragmentShader = new Shader(
    "../shaders/blackBorderFragmentShader.glsl",
    GL_FRAGMENT_SHADER
  );

  std::vector<Shader*> blackBorderShaders = {
    blackBorderVertexShader, blackBorderFragmentShader};
  ShaderProgram* blackBorderShaderProgram = new ShaderProgram(
    blackBorderShaders);

  // Try to compile shaders
  compilationIsSuccess = blackBorderShaderProgram->compile();
  if(!compilationIsSuccess){
    delete celShadingShaderProgram;
    delete blackBorderShaderProgram;

    return 1;
  }

  // Create and load meshes
  Mesh* king = new Mesh("../assets/king.obj");
  king->initBuffers();

  Mesh* queen = new Mesh("../assets/queen.obj");
  queen->initBuffers();

  Mesh* jester = new Mesh("../assets/jester.obj");
  jester->initBuffers();

  Mesh* tower = new Mesh("../assets/tower.obj");
  tower->initBuffers();

  Mesh* knight = new Mesh("../assets/knight.obj");
  knight->initBuffers();

  Mesh* pawn = new Mesh("../assets/pawn.obj");
  pawn->initBuffers();

  // Render loop
  float angle = 0.0;
  bool running = true;
  while(running){
    sf::Event event;
    while(window.pollEvent(event)){
      if(event.type == sf::Event::Closed){
        running = false;
      }
      else if(event.type == sf::Event::Resized){
        width = event.size.width;
        height = event.size.height;

        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(70, (double)width/height, 1, 1000);
      }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    gluLookAt(0, 15, 8, 0, 0, 0, 0, 0, 1);

    angle += 0.5;

    displayToonMesh(
      king, blackBorderShaderProgram->id, celShadingShaderProgram->id,
      -10.0, 0.0, 0.0,
      angle, 0, 0, 1
    );

    displayToonMesh(
      queen, blackBorderShaderProgram->id, celShadingShaderProgram->id,
      -6.0, 0.0, 0.0,
      angle, 0, 0, 1
    );

    displayToonMesh(
      jester, blackBorderShaderProgram->id, celShadingShaderProgram->id,
      -2.0, 0.0, 0.0,
      angle, 0, 0, 1
    );

    displayToonMesh(
      knight, blackBorderShaderProgram->id, celShadingShaderProgram->id,
      2.0, 0.0, 0.0,
      angle, 0, 0, 1
    );

    displayToonMesh(
      tower, blackBorderShaderProgram->id, celShadingShaderProgram->id,
      6.0, 0.0, 0.0,
      angle, 0, 0, 1
    );

    displayToonMesh(
      pawn, blackBorderShaderProgram->id, celShadingShaderProgram->id,
      10.0, 0.0, 0.0,
      angle, 0, 0, 1
    );

    glFlush();

    window.display();
  }

  delete king;
  delete queen;
  delete jester;
  delete knight;
  delete tower;
  delete pawn;
  delete celShadingShaderProgram;
  delete blackBorderShaderProgram;

  return 0;
}
