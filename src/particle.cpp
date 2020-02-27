#include "particle.h"

Particle::Particle()
{
	
	speed = (rand() % 1000) / 10000.0 + 0.1;
	angle = rand() % 360;
	//angle = ang;
	lifespan = 2000000;
	startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
		std::chrono::system_clock::now().time_since_epoch()).count();
	lastTime = startTime;
	velocity = point4(speed * cos(angle), speed * sin(angle), speed * cos(angle), 1.0);
	flag = true;
	size = 1.0;
	pos = point4(100.0, 100.0, 100.0, 1.0); //starting positions of the particles. 
}

glm::mat4 Particle::buildMatrix()
{
	glm::mat4 particleTrans = glm::translate(glm::mat4(), glm::vec3(pos.x, pos.y, pos.z));
	glm::mat4 particleRot = glm::rotate(glm::mat4(), glm::radians(angle), glm::vec3(1, 0, 0));
	particleRot = glm::rotate(particleRot, glm::radians(angle), glm::vec3(0, 1, 0));
	particleRot = glm::rotate(particleRot, glm::radians(angle), glm::vec3(0, 0, 1));
	glm::mat4 particleScale = glm::scale(glm::mat4(), glm::vec3(0.2, 0.2, 0.2));

	particleTrans = particleTrans * particleRot * particleScale;

	return particleTrans;
}

void Particle::buildParticle(point4 origin)
{
	pos = origin;
}