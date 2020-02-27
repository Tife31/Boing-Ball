#include <iostream>
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec4 point4;
class Particle
{
public:
	float size;
	long long startTime;
	long long lastTime;
	point4 pos;
	point4 velocity;
	int lifespan;
	float angle;
	float speed;
	bool flag;

	Particle();

	glm::mat4 buildMatrix();
	void buildParticle(point4 origin);

};