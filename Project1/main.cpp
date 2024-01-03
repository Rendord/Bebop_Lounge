#include <iostream>
#include <algorithm>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "objloader.h"
#include <vector>
#include <map>

#include "glsl.h"
#include "texture.h"
#include "MatrixHelper.h"
#include <string>
#include "DroneCamera.h"
#include "WalkCamera.h"
#include "Mesh.h"
#include "PrimitiveModel.h"
#include "Shader.h"

using namespace std;

//light source struct
struct LightSource
{
	glm::vec3 position;
};

//--------------------------------------------------------------------------------
// Consts
//--------------------------------------------------------------------------------

//width and height of the screen
const int WIDTH = 800, HEIGHT = 600;

//shader paths
const char* vertexShaderPath = "vertexshader.vert";
const char* fragmentShaderPath = "fragmentshader.frag";

//delta time variable used by glut 
unsigned const int DELTA_TIME = 0;

//keybuffer for using simultaneous keypresses
bool keybuffer[127];

//axes for easy rotations and scaling
const glm::vec3 X_AXIS = glm::vec3(1.0, 0.0, 0.0);
const glm::vec3 Y_AXIS = glm::vec3(0.0, 1.0, 0.0);
const glm::vec3 Z_AXIS = glm::vec3(0.0, 0.0, 1.0);


//--------------------------------------------------------------------------------
// Variables
//--------------------------------------------------------------------------------

//delta time variables
int previoustimepassed;
int deltatime;

//light source
LightSource light;

// Matrices
glm::mat4 view, projection;

//cameras
DroneCamera* droneCamera;
WalkCamera* walkCamera;
bool dronemode = false;

//primitives
Box box;
StairTrapezoid side_of_stair;
PrimitiveModel ground_floor;
PrimitiveModel stair1;
PrimitiveModel stair2;
PrimitiveModel fan;

//models
vector<Model> models;

//materials
Material basic;
Material* current_shader_setting;
vector<Material> object_materials;

//deprecated
vector<Material> shader_settings;
int shader_setting_index = 0;

//shaders
Shader standard;
Shader cool; 

//door animation
bool door_closing;
bool door_opening;
bool door_half_open;
bool door_open;
bool door_closed;
float door_open_z;
float door_open_x;
float door_close_z;
float door_close_x;
int nframes_door_closed = 0;
int nframes_door_open = 0;
int door_anim_pause = 120;

//------------------------------------------------------------
// glm::vec3 rgbToVec3()
// Function that returns a vector 3 based on the input RGB values
//------------------------------------------------------------

glm::vec3 rgbToVec3(float r, float g, float b) {
	r = r / 255;
	g = g / 255;
	b = b / 255;
	return glm::vec3(r, g, b);
}

//------------------------------------------------------------
// glm::vec3 switchLightingEffect()
// deprecated function that is no longer used anymore, could still be used eventually for a lighting setting but would have to add extra uniforms to shader
//------------------------------------------------------------
void switchLightingEffect(Material material, Shader& shader) {
	shader.Use();
	shader.setVec3("mat_ambient", material.ambient_color);
	shader.setVec3("mat_diffuse", material.diffuse_color);
	shader.setVec3("mat_specular", material.specular);
	shader.setFloat("mat_power", material.power);
	std::cout << material.name << std::endl;
}



//--------------------------------------------------------------------------------
// Door animation
//--------------------------------------------------------------------------------
void MoveDoor() {
	//if door is opening move it along the z axis (negative because the model is rotated) and until it reaches the preset Z coordinate at which point door = half open
	if (door_opening) {
		if (!door_half_open) {
			models[11].Translate(glm::vec3(0.0, 0.0, 0.02));
			if (models[11].position.z >= door_open_z) {
				door_half_open = true;
			}
		}
		//if the door is half open translate along the X coordinate instead until it reaches the door open X coordinate at which point door opening becomes false and door closing becomes true
		else {
			models[11].Rotate(0.9, Z_AXIS);
			models[11].Translate(glm::vec3(0.02, 0.0, 0.0));
			
			if (models[11].position.x >= door_open_x) {
				door_opening = false;
				door_open = true;
			}

		}
	}
	else if (door_open) {
		nframes_door_open++;
		// keep door open for door_anim_pause frames
		if (nframes_door_open > door_anim_pause)
		{
			door_open = false;
			door_closing = true;
			door_half_open = false;
			nframes_door_open = 0;
		}
	}
	
	//does the opposite of the first half of the animation
	if (door_closing) {
		if (!door_half_open) {
			models[11].Rotate(-0.9, Z_AXIS);
			models[11].Translate(glm::vec3(-0.02, 0.0, 0.0));
			
			if (models[11].position.x <= door_close_x) {
				door_half_open = true;
			}
		}
		else {
			models[11].Translate(glm::vec3(0.0, 0.0, -0.02));
			if (models[11].position.z <= door_close_z) {
				door_closing = false;
				door_closed = true;
			}
		}
		
	}
	else if (door_closed) {
	    nframes_door_closed++;
		// keep door closed for door_anim_pause frames
		if (nframes_door_closed > door_anim_pause) {
			door_closed = false;
			door_opening = true;
			door_half_open = false;
			nframes_door_closed = 0;
		}
	}
}

//--------------------------------------------------------------------------------
// Keyboard handling
//--------------------------------------------------------------------------------
void KeyboardHelper(unsigned char key, bool down) {
	if (down)
		keybuffer[key] = true;
	else
		keybuffer[key] = false;
}

void keyboardDownHandler(unsigned char key, int a, int b)
{
	KeyboardHelper(key, true);
	if (key == 27) {
		std::cout << "exiting program" << std::endl;
		glutExit();
	}
	
	if (key == 118) { //v
		dronemode = !dronemode;
		std::cout << "switched viewing mode" << std::endl;
	}
}


void keyboardUpHandler(unsigned char key, int a, int b)
{
	
	KeyboardHelper(key, false);
	//up
	//down
	//pitch
	//roll 
	///yaw
}

//--------------------------------------------------------------------------------
// Rendering
//--------------------------------------------------------------------------------

void Render()
{

	//get DeltaTime
	int timepassed = glutGet(GLUT_ELAPSED_TIME);
	deltatime = timepassed - previoustimepassed; 
	previoustimepassed = timepassed;

	//handle movement and view
	if (!dronemode) {
		walkCamera->Movement(keybuffer, (float)deltatime);
		view = walkCamera->View();
	}
	else {
		droneCamera->Movement(keybuffer, (float)deltatime);
		view = droneCamera->View();
	}
	
	//clearbuffer 
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Animations

	//fan blades
	models[12].Rotate(0.2, Y_AXIS);
	
	//door animation
	MoveDoor();

	//render all the imported models
	for (int i = 0; i < models.size(); i++) {
		models[i].Render(view);
	}

	//render primitive models
	ground_floor.Render(view);
	stair1.Render(view);
	stair2.Render(view);
	

	//unbind vertex arrray and swap buffers
	glBindVertexArray(0);
    glutSwapBuffers();
}


//------------------------------------------------------------
// void Render(int n)
// Render method that is called by the timer function
//------------------------------------------------------------

void Render(int n)
{
    Render();
    glutTimerFunc(DELTA_TIME, Render, 0);
}


//------------------------------------------------------------
// void InitGlutGlew(int argc, char **argv)
// Initializes Glut and Glew
//------------------------------------------------------------

void InitGlutGlew(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Main Lounge, it's a vibe in here.");
    glutDisplayFunc(Render);
    glutKeyboardFunc(keyboardDownHandler);
	glutKeyboardUpFunc(keyboardUpHandler);
    glutTimerFunc(DELTA_TIME, Render, 0);

    glewInit();
}


//------------------------------------------------------------
// void InitShaders()
// Initializes the fragmentshader and vertexshader
//------------------------------------------------------------


void InitShaders()
{
		const char* vert1 = "default.vert";
		const char* frag1 = "default.frag";
		const char* frag2 = "fragshader.frag";

		//standard shader
		standard = Shader(vert1, frag2);
		
		//different shader for experimentation
		cool = Shader(vert1, frag1);
}


//------------------------------------------------------------
// void InitModels()
// initializes all the imported and primitive models
//------------------------------------------------------------

void InitPrimitives() {
	box = Box(glm::vec3(0, 0, 0), 4, 4, 4, "Textures/white.bmp", &standard, basic);
	
	//pot
	models.push_back(Model("pot.obj", "Textures/pot.bmp", &standard, object_materials[0]));
	//table
	models.push_back(Model("table.obj", "Textures/dark_matte_metal.bmp", &standard, object_materials[2]));
	//small box
	models.push_back(Model("smallbox.obj", "Textures/metal_rock.bmp", &standard, basic));
	//pipes
	models.push_back(Model("pipes_remastered.obj", "Textures/copperpipes.bmp", &standard, object_materials[4]));
	//couch
	models.push_back(Model("couch_back.obj", "Textures/grey_metal.bmp", &standard, object_materials[2]));
	models.push_back(Model("couch_bottom.obj", "Textures/grey_metal.bmp", &standard, object_materials[2]));
	models.push_back(Model("couch_cushion.obj", "Textures/leather.bmp", &standard, basic));
	models.push_back(Model("couch_cushion2.obj", "Textures/leather.bmp", &standard, basic));
	//chair
	models.push_back(Model("chair_frame.obj", "Textures/grey_metal.bmp", &standard, object_materials[2]));
	models.push_back(Model("chair_cushion.obj", "Textures/leather.bmp", &standard, basic));
	//door and right wall
	models.push_back(Model("door_outer_wall.obj", "Textures/grey_metal.bmp", &standard, basic));
	models.push_back(Model("door_inner.obj", "Textures/grey_metal.bmp", &standard, object_materials[3]));
	//fan
	models.push_back(Model("fan_blades.obj", "Textures/fan_blades.bmp", &standard, basic));
	models.push_back(Model("lamp.obj", "Textures/warm_white_lamp.bmp", &standard, basic));
	models.push_back(Model("lamp_base.obj", "Textures/fan_parts.bmp", &standard, basic));
	models.push_back(Model("fan_motor.obj", "Textures/fan_parts.bmp", &standard, basic));
	models.push_back(Model("fan_motor2.obj", "Textures/fan_parts.bmp", &standard, basic));
	models.push_back(Model("cable.obj", "Textures/lamp_cable.bmp", &standard, basic));

	//room
	ground_floor = PrimitiveModel();
	ground_floor.AddMesh(Box(glm::vec3(4.75, -3, 0), 26.5, 6, 36, "Textures/uvtemplate.bmp", &standard, basic));
	ground_floor.AddMesh(Box(glm::vec3(12, 4.5, -18.25), 12, 21, 0.5, "Textures/grey_metal.bmp", &standard, basic));
	ground_floor.AddMesh(Box(glm::vec3(18.25, 4.5, 0), 0.5, 21, 36, "Textures/grey_metal.bmp", &standard, basic));
	ground_floor.AddMesh(Box(glm::vec3(12, 1.25, -12), 12, 2.5, 12, "Textures/grey_metal.bmp", &standard, basic));
	ground_floor.AddMesh(Box(glm::vec3(15, 1.25, -2), 6, 2.5, 8, "Textures/grey_metal.bmp", &standard, basic));
	ground_floor.AddMesh(Box(glm::vec3(12, 1.25, 10), 12, 2.5, 16, "Textures/grey_metal.bmp", &standard, basic));
	ground_floor.AddMesh(Box(glm::vec3(0, 4.5, 16), 9, 1.2, 4, "Textures/grey_metal.bmp", &standard, basic));

	//first stair
	stair1.AddMesh(StairTrapezoid(glm::vec3(0, 1.25, -1.5), 2.5, 2.5, 0.25, 1, "Textures/wood.bmp", &standard, object_materials[1]));
	stair1.AddMesh(StairTrapezoid(glm::vec3(0, 1.25, 1.5), 2.5, 2.5, 0.25, 1, "Textures/wood.bmp", &standard, object_materials[1]));
	stair1.AddMesh(Box(glm::vec3(-0.5, 0.25, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));
	stair1.AddMesh(Box(glm::vec3(0, 0.75, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));
	stair1.AddMesh(Box(glm::vec3(0.5, 1.25, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));
	stair1.AddMesh(Box(glm::vec3(1, 1.75, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));

	//second stair
	stair2.AddMesh(StairTrapezoid(glm::vec3(0, 1.25, -1.5), 2.5, 2.5, 0.25, 1, "Textures/wood.bmp", &standard, object_materials[1]));
	stair2.AddMesh(StairTrapezoid(glm::vec3(0, 1.25, 1.5), 2.5, 2.5, 0.25, 1, "Textures/wood.bmp", &standard, object_materials[1]));
	stair2.AddMesh(Box(glm::vec3(-0.5, 0.25, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));
	stair2.AddMesh(Box(glm::vec3(0, 0.75, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));
	stair2.AddMesh(Box(glm::vec3(0.5, 1.25, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));
	stair2.AddMesh(Box(glm::vec3(1, 1.75, 0), 0.67, 0.2, 2.75, "Textures/wood.bmp", &standard, object_materials[1]));


	
}

//------------------------------------------------------------
// void setupScene()
// setups the scene, positions all the models and sets the variables for the door animation
//------------------------------------------------------------
void setupScene() {

	//move the stairs to the correct position
	stair1.Translate(glm::vec3(8, 0, 0.75));
	stair1.Rotate(-90, Y_AXIS);

	//move the second set of stairs in the correct position
	stair2.Translate(glm::vec3(5.75, 2.5, 16));
	stair2.Rotate(-180, Y_AXIS);


	box.Translate(glm::vec3(0.0, 5, 3));
	box.Rotate(45, X_AXIS);
	//ground_floor.Rotate(45, Y_AXIS);

	// pot
	models[0].Translate(glm::vec3(8.0, 4.0, -16.0));

	// table
	models[1].Translate(glm::vec3(0.0, 1.0, 4.0));

	// small box
	models[2].Translate(glm::vec3(4.5, 1.0, -10.0));

	// pipes
	models[3].Translate(glm::vec3(11.0, 10.0, 17.0));

	// couch
	for (int i = 4; i <= 7; i++) {
		models[i].Translate(glm::vec3(-1.4, 1.2, 8.0));
	}

	// chair
	for (int i = 8; i <= 9; i++) {
		models[i].Translate(glm::vec3(-3.0, 0.75, -1.0));
		models[i].Scale(glm::vec3(0.8, 0.8, 0.8));
		models[i].Rotate(200, Y_AXIS);
	}

	// door
	for (int i = 10; i <= 11; i++) {
		models[i].Translate(glm::vec3(0.0, 10.0, 17.5));
		models[i].Rotate(180, Y_AXIS);
	}
	//animation variables
	door_opening = true;
	door_closing = false;
	door_half_open = false;
	door_open_x = 8.0;
	door_open_z = 19.5;
	door_close_x = 0.0;
	door_close_z = 17.5;

	// fan
	for (int i = 12; i <= 17; i++) {
		models[i].Translate(glm::vec3(0.0, 10.0, 0.0));
	}


}


//------------------------------------------------------------
// void InitMaterials()
// Initializes all the materials that are used in the scene
//------------------------------------------------------------
void InitMaterials() {
	
	//standard material
	basic.name = "basic";
	basic.ambient_color = glm::vec3(0.5, 0.5, 0.5);
	basic.diffuse_color = glm::vec3(0.3, 0.3, 0.3);
	basic.specular = glm::vec3(0.5, 0.5, 0.5);
	basic.power = 1;
	//pot material
	Material pot;
	pot.ambient_color = glm::vec3(0.25, 0.25, 0.25);
	pot.diffuse_color = glm::vec3(1.0, 1.0, 1.0);
	pot.specular = glm::vec3(0.0, 0.0, 0.0);
	pot.power = 1;
	//wood material (stairs e.g.)
	Material wood;
	wood.ambient_color = glm::vec3(0.25, 0.25, 0.25);
	wood.diffuse_color = glm::vec3(1.0, 1.0, 1.0);
	wood.specular = glm::vec3(0.0, 0.0, 0.0);
	wood.power = 1;
	//dark metal used for the floors
	Material dark_metal;
	dark_metal.ambient_color = glm::vec3(0.35, 0.35, 0.35);
	dark_metal.diffuse_color = glm::vec3(1.0, 1.0, 1.0);
	dark_metal.specular = glm::vec3(1, 1, 1);
	dark_metal.power = 8;
	//lighter metal used for the inner door
	Material lighter_metal;
	lighter_metal.ambient_color = glm::vec3(0.8, 0.8, 0.8);
	lighter_metal.diffuse_color = glm::vec3(0.6, 0.6, 0.8);
	lighter_metal.specular = glm::vec3(1.0, 0.5, 0.0);
	lighter_metal.power = 8;
	//used for the pipes
	Material copper_pipes;
	copper_pipes.ambient_color = glm::vec3(0.45, 0.45, 0.45);
	copper_pipes.diffuse_color = glm::vec3(0.8, 0.6, 0.6);
	copper_pipes.specular = glm::vec3(0.25, 1.0, 0.25);
	copper_pipes.power = 64;


	object_materials.push_back(pot);
	object_materials.push_back(wood);
	object_materials.push_back(dark_metal);
	object_materials.push_back(lighter_metal);
	object_materials.push_back(copper_pipes);
	
}

//------------------------------------------------------------
// void InitMatrices()
// initializes the view and projection matrix fov is set to 70
//------------------------------------------------------------

void InitMatrices()
{			
	view = droneCamera->View();
	projection = glm::perspective(
		glm::radians(70.0f),
		1.0f * WIDTH / HEIGHT, 0.1f,
		120.0f);
}


//------------------------------------------------------------
// void InitLight()
// sets the position of the light 
//------------------------------------------------------------
void InitLight() {
	light.position = glm::vec3(0.0, 10.0, 0.0);
}

//------------------------------------------------------------
// void initUniforms()
// sets the uniform variables
//------------------------------------------------------------
void InitUniforms()
{

	//activate shader
	standard.Use();
	
	//set uniform variables
	standard.setMat4("projection", projection);
	standard.setVec3("light_pos", light.position);

}

//------------------------------------------------------------
// void initCameras()
// initializes the two camera objects that are used
// drone camera overlooks and walk camera is set within the scene
// walk camera Y level is locked
// jump is possible in walk mode with spacebar
//------------------------------------------------------------
void InitCameras() {
	
	droneCamera = new DroneCamera(glm::vec3(-24.0f, 18.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -30.0f);
	walkCamera = new WalkCamera(glm::vec3(-4.0f, 6.0f, -12.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f, 0.0f);
	
}





int main(int argc, char** argv)
{
	InitCameras();
    InitGlutGlew(argc, argv);
    InitShaders();
	InitMaterials();
    InitMatrices();
	InitLight();
    InitUniforms();
	InitPrimitives();
	setupScene();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // Show console window
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_NORMAL);

    // Main loop
    glutMainLoop();

    return 0;
}
