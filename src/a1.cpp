// From SIGGRAPH 2013: An Introduction to OpenGL Programming
// This example combines the objects from example6 & example7 to
// demonstrate multiple VAOs

#include "common.h"
#include "particle.h"

#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

const char *WINDOW_TITLE = "Height Field and Cube";
const double FRAME_RATE_MS = 1000.0 / 60.0;
const float GRAVITY = -9.8/6000.0;

typedef glm::vec4  color4;
typedef glm::vec4  point4;

//const int N = 16;
const int NUM_PARTICLES = 500;
const int stackCount = 9;
const int sectorCount = 18;
const int sphereRad = 1;
const int end = 5;
const int begin = 5;
const int wallCol = 10;
const int wallRow = 10;
int projection = 0;
const int wallSize = (end + begin) / wallCol;
glm::vec3 velocity = glm::vec3(0.02f, 0.0f, 0.0f);
glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);

GLuint indices[(stackCount - 1) *sectorCount * 2*3];
GLuint wallIndices[((wallCol) * (wallRow) *2*3) + 6]; //2 for the number of triangles and 3 each of their vertices.
GLuint shadowIndices[300 * 3];
GLuint quadIndices[6];

point4 ballVertices[stackCount * sectorCount];
point4 wallVertices[(wallCol+ 1) * (wallRow +1)];
point4 shadowVertices[301];

point4 particleVertices[3] = {
  point4(-0.5, -0.5,  0.5, 1.0),
  point4(-0.5,  0.5,  0.5, 1.0),
  point4(0.5,  0.5,  0.5, 1.0)
};
Particle particleSystem[NUM_PARTICLES];


void createParticles()
{
	for(int i = 0; i < NUM_PARTICLES; i++)
	{
		particleSystem[i].buildParticle(point4(position.x, position.y, position.z, 1.0));
	}
}

void createVertices()
{
	// to create the sphere here.


	GLfloat pi = glm::pi<float>();
	GLfloat stackStep = pi / (stackCount - 1);
	GLfloat sectorStep = (2 * pi) / sectorCount;
	int index = 0;
	for (int i = 0; i < stackCount; i++)
	{
		GLfloat currSectorAngle, currStackAngle;
		currStackAngle = (pi / 2) - (i * stackStep);
		GLfloat ringRadius = sphereRad * glm::cos(currStackAngle); // This is the xz coordinates. to tell us where in the ring we are. aka the sector. tells us how high we are and where to start in the sector. 
		GLfloat yCoord = sphereRad * glm::sin(currStackAngle); // the y coordniate to tell us how high the ring is. 
		for (int j = 0; j < sectorCount; j++)
		{
			currSectorAngle = j * sectorStep;
			//calculate the x, y, z, and w.
			GLfloat xCoord = ringRadius * glm::cos(currSectorAngle);// now we know what ring we are in, now we are working just with the sector. 
			GLfloat zCoord = ringRadius * glm::sin(currSectorAngle);
			ballVertices[index++] = point4(xCoord, yCoord, zCoord, 1.0);
		}

	}

	//to create the vertices of the wall.
	int wallIndex = 0;
	for(int i = 0; i< wallRow +1; i++)
	{
		for(int j =0; j<wallCol + 1; j++)
		{
			int yPoint = begin - (i * wallSize);
			int xPoint = (j * wallSize) - end;
			wallVertices[wallIndex++] = point4(xPoint, yPoint, 0, 1.0);
		}
	}

	//create a shadow
	 int shadowIndex = 0;
	 shadowVertices[0] = point4(0.0, 0.0, 0.0, 1.0);
	for (int i = 1; i < 301; i++) {
		double angle = 2 * glm::pi<float>() * i / 300;
		double x = sphereRad * glm::cos(angle);
		double y = sphereRad * glm::sin(angle);
		shadowVertices[shadowIndex++] = point4(x, y, 0, 1.0);
	}

}


void createIndicies()
{
	int index = 0;

	for (int strip = 0; strip < stackCount - 1; strip++) {
		for (int col = 0; col < sectorCount; col++) {

			indices[index++] = strip * sectorCount + (col + 1) % sectorCount;
			indices[index++] = strip * sectorCount + col;
			indices[index++] = strip * sectorCount + col + sectorCount;

			indices[index++] = strip * sectorCount + (col + 1) % sectorCount;
			indices[index++] = strip * sectorCount + col + sectorCount;
			indices[index++] = strip * sectorCount + (col + 1) % sectorCount + sectorCount;
		}
	}
	// indices of the wall.
	int wallIndex = 0;
	for(int row =0; row< wallRow; row++ )
	{
		for(int col = 0; col < wallCol ; col++)
		{
			int ourPos = row * (wallCol + 1) + col;

			wallIndices[wallIndex++] = ourPos;
			wallIndices[wallIndex++] = ourPos + (wallCol + 1);
			wallIndices[wallIndex++] = ourPos + 1;

			wallIndices[wallIndex++] = ourPos + 1;
			wallIndices[wallIndex++] = ourPos + (wallCol + 1);
			wallIndices[wallIndex++] = ourPos + (wallCol + 1) + 1;
		}
	}

	//Indices of the shadow
	int shadIndex = 0;
	for (int i = 0; i < 300; i++) 
	{
		shadowIndices[shadIndex++] = 0;
		shadowIndices[shadIndex++] = i;
		shadowIndices[shadIndex++] = (i + 1) % 300;

	}
	// indicies of the quad. to grab the four corners of the wall.
	int quadIndex = 0;
	wallIndices[wallIndex++] = wallCol;
	wallIndices[wallIndex++] = 0;
	wallIndices[wallIndex++] = (wallCol + 1) * wallRow;

	wallIndices[wallIndex++] = wallCol;
	wallIndices[wallIndex++] = (wallCol + 1) * wallRow;
	wallIndices[wallIndex++] = (wallCol + 1) * wallRow + wallRow;

	
}

// Array of rotation angles (in degrees) for each coordinate axis
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int      Axis = Yaxis;
GLfloat  Theta[NumAxes] = { 30.0, 0.0, 0.0 };

// Model-view and projection matrices uniform location
GLuint  ModelView, Projection;
GLuint  VAOIndex, wallColor;

//----------------------------------------------------------------------------

// need global access to VAOs
GLuint vaos[4];

// OpenGL initialization
void init()
{
	//Need to calculate the vertices of the sphere I want to create here.
	createVertices();
	createIndicies();

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vshader7.glsl", "fshader7.glsl");
	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");

	// Create a vertex array object.
	//we  are going to have 3 objects. the sphere, the wall and the floor
	glGenVertexArrays(4, vaos);
	//--------------------------------------------- object #1: The wall ---------------------------------------------
	glBindVertexArray(vaos[0]);
	GLuint wallBuffer;

	glGenBuffers(1, &wallBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, wallBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);

	// set up vertex data for this vao
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Another for the index buffer
	glGenBuffers(1, &wallBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wallBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);

	//-----------------------------------------------object #2: the Shadow-------------------------------------------------------

	glBindVertexArray(vaos[1]);
	GLuint shadowBuffer;
	glGenBuffers(1, &shadowBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, shadowBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(shadowVertices), shadowVertices, GL_STATIC_DRAW);

	// set up vertex data for this vao
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Another for the index buffer
	glGenBuffers(1, &shadowBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shadowBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(shadowIndices), shadowIndices, GL_STATIC_DRAW);

	// --------------------------------------------- object #3: the sphere ---------------------------------------------
	glBindVertexArray(vaos[2]);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ballVertices), ballVertices, GL_STATIC_DRAW);

	// set up vertex data for this vao
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Another for the index buffer
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//------------------------------------------object #4: the particles------------------------------------------------------
	glBindVertexArray(vaos[3]);
	GLuint particleBuffer;
	glGenBuffers(1, &particleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, particleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(particleVertices), particleVertices, GL_STATIC_DRAW);
	
	// set up vertex data for this vao
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	// Retrieve transformation uniform variable locations
	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	VAOIndex = glGetUniformLocation(program, "vaoIndex");
	wallColor = glGetUniformLocation(program, "wallColor");

	// this is used with "flat" in the shaders to get the same solid
	// colour for each face of the cube
	glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

	glEnable(GL_DEPTH_TEST);

	glShadeModel(GL_FLAT);

	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//  Generate the model-view matrix
	glm::vec3 viewer_pos = glm::vec3 (0.0, 0.0, 15.0);
	glm::mat4 cameraTransformation, rot, model_view;
	
	if(projection == 1)
	{
		  rot = glm::mat4(); // Reset matrix
		  rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(1, 0, 0));
	}
	else if(projection == 2)
	{
		viewer_pos = glm::vec3(1.0, 0.0, 15.0);
		rot = glm::mat4(); // Reset matrix
		rot = glm::rotate(rot, glm::radians(40.0f), glm::vec3(1, 0, 0));
		rot = glm::rotate(rot, glm::radians(40.0f), glm::vec3(0, 1, 0)); //rotate in the y
	}

	cameraTransformation = glm::translate(cameraTransformation, -viewer_pos);
	cameraTransformation = cameraTransformation * rot;
	  
	// -------------------------------------- Draw the wall--------------------------------
	glUniform1i(wallColor, 2); // draw the grid 
	glUniform1i(VAOIndex, 0);
	glBindVertexArray(vaos[0]);
	const glm::vec3 wallPos(0.0, 0.0, -5.0);

	rot = glm::mat4(); // Reset matrix
	glm::mat4 wallTrans = glm::translate(glm::mat4(), wallPos);
	model_view = cameraTransformation * rot * wallTrans;
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));

	for (int i = 0; i < sizeof(wallIndices) / sizeof(GLuint) - 6; i += 3) {
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}

	glUniform1i(wallColor, 1); // draw the quad 
	for(int i = sizeof(wallIndices) / sizeof(GLuint) - 6; i < sizeof(wallIndices) / sizeof(GLuint); i+=3)
	{
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}
	

	// -------------------------------------- Draw the floor
	glUniform1i(wallColor, 2); // draw the grid 
	const glm::vec3 floorPos(0.0, -5.02, 0.0);
	

	rot = glm::mat4(); // Reset matrix
	rot = glm::rotate(rot, glm::radians(90.0f), glm::vec3(1, 0, 0));
	glm::mat4 floorTrans = glm::translate(glm::mat4(), floorPos);
	model_view = cameraTransformation * floorTrans * rot;
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
	
	for (int i = 0; i < sizeof(wallIndices) / sizeof(GLuint); i += 3) {
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}


	//--------------------------------------------
	
	glUniform1i(wallColor, 1); // draw the quad 

	floorTrans = glm::mat4();
	floorTrans = glm::translate(glm::mat4(), glm::vec3(0.0, -5.02, 0.0));
	glUniform1i(wallColor, 1); // draw the quad on the floor
	model_view = cameraTransformation * floorTrans * rot;
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
	for (int i = sizeof(wallIndices) / sizeof(GLuint) - 6; i < sizeof(wallIndices) / sizeof(GLuint); i += 3)
	{
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}

	// ----------------------------------- Draw the sphere -------------------------------
	glBindVertexArray(vaos[2]);
	glUniform1i(VAOIndex, 2);
	rot = glm::mat4(); // Reset matrix
	rot = glm::rotate(rot, glm::radians(Theta[Xaxis]), glm::vec3(1, 0, 0));
	rot = glm::rotate(rot, glm::radians(Theta[Yaxis]), glm::vec3(0, 1, 0));
	rot = glm::rotate(rot, glm::radians(Theta[Zaxis]), glm::vec3(0, 0, 1));

	velocity.y += GRAVITY;

	if (position.y - sphereRad < -5.0)
	{
		velocity.y = -1 * velocity.y;
		velocity.y += GRAVITY;
	}
	if(position.x -sphereRad < -end || position.x + sphereRad > end)
	{
		velocity.x = -1 * velocity.x;
	}
	
	position.x += velocity.x;
	position.y += velocity.y;
	glm::mat4 sphereTrans = glm::translate(glm::mat4(), position);

	model_view = cameraTransformation * sphereTrans * rot;
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
	
	

	for (int i = 0; i < sizeof(indices) / sizeof(GLuint); i += 3) {
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}
	//-------------------------------------------------------Draw the shadow------------------------------------
	glBindVertexArray(vaos[1]);
	glUniform1i(VAOIndex, 1);
	rot = glm::mat4();
	
	glm::mat4 shadowTrans = glm::translate(glm::mat4(), glm::vec3(position.x, position.y , -4.5));
	
	model_view = cameraTransformation * shadowTrans * rot;
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
	
	for (int i = 0; i < sizeof(shadowIndices) / sizeof(GLuint); i += 3) {
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}

	//-----------------------------------------------------------Draw the floor shadow
	const glm::vec3 shadowPos(0.0, -4.95 - position.y, 4.0);
	glUniform1i(VAOIndex, 1);
	rot = glm::mat4(); // Reset matrix
	rot = glm::rotate(rot, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	glm::mat4 shadowFloorTrans = glm::translate(glm::mat4(), shadowPos);
	model_view = model_view * shadowFloorTrans * rot;
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));

	for (int i = 0; i < sizeof(shadowIndices) / sizeof(GLuint); i += 3) {
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void *)(i * sizeof(GLuint)));
	}

	//---------------------------------------------------------------DrawParticles--------------------------------------
	glBindVertexArray(vaos[3]);
	glUniform1i(VAOIndex, 3);

	if (position.y - sphereRad < -5.0)
	{
		createParticles();
	
	}
	

	for (int j = 0; j < NUM_PARTICLES; j++)
	{
		long long currTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();

		if (particleSystem[j].startTime + particleSystem[j].lifespan > currTime)
		{
			float elapsedTime = currTime - particleSystem[j].lastTime;
			particleSystem[j].pos.y -= GRAVITY;
			particleSystem[j].lastTime = currTime;
			float distX = particleSystem[j].velocity.x;
			float distY = particleSystem[j].velocity.y;
			particleSystem[j].pos.x += distX;
			particleSystem[j].pos.y += distY;
		}
		else
		{
			//kill the particle. 
			particleSystem[j].flag = false;

		}
	}
	for (int i = 0; i < NUM_PARTICLES; i++) {

		model_view = cameraTransformation * particleSystem[i].buildMatrix() * rot;
		glUniformMatrix4fv(ModelView, 1, GL_FALSE, glm::value_ptr(model_view));
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void
mouse(int button, int state, int x, int y)
{
	//if (state == GLUT_DOWN) {
	//	switch (button) {
	//	case GLUT_LEFT_BUTTON:    Axis = Xaxis;  break;
	//	case GLUT_MIDDLE_BUTTON:  Axis = Yaxis;  break;
	//	case GLUT_RIGHT_BUTTON:   Axis = Zaxis;  break;
	//	}
	//}
}

//----------------------------------------------------------------------------

void
update(void)
{
	Theta[Axis] += 0.5;

	if (Theta[Axis] > 360.0) {
		Theta[Axis] -= 360.0;
	}
}

//----------------------------------------------------------------------------

void
keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case ' ':
		projection += 1;
		projection = projection % 3;
		break;
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

void
reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	GLfloat aspect = GLfloat(width) / height;
	glm::mat4  projection = glm::perspective(glm::radians(45.0f), aspect, 0.5f, 200.0f);

	glUniformMatrix4fv(Projection, 1, GL_FALSE, glm::value_ptr(projection));
}
