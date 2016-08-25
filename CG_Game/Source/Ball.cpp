//
//  ball.cpp
//
//  Created by  on 30.04.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/ball.h"
float clamp(float n, float lower, float upper);

Ball::Ball(float speed, PerlinNoise terrainNoise) {
	this->speed = speed;
	this->terrainNoise = terrainNoise;
	velocity = Vector(0, 0, 0);
}

Ball::~Ball() {
}

bool Ball::load(const char* BallModel, const Vector& StartPos) {
	g_Model_ball.load(BallModel, false);
	this->aimTarget = Vector(0, 0, 0);
	m_Ball.translation(StartPos);

	return true;
}

void Ball::steer(float ForwardBackward, float LeftRight) {
	this->straight = ForwardBackward;
	this->side = LeftRight;
}

void Ball::update(float DeltaTime) {
	
	Matrix transM, rotX, rotZ;
	static float accumulatedTime = 0;
	accumulatedTime += DeltaTime;

	if (velocity.Z > 0) {
		velocity.Z -= 0.7 * DeltaTime;
		velocity.Z = (velocity.Z < 0) ? 0 : velocity.Z;
	}
	if(velocity.Z < 0) {
		velocity.Z += 0.7 * DeltaTime;
		velocity.Z = (velocity.Z > 0) ? 0 : velocity.Z;
	}

	if (velocity.X > 0) {
		velocity.X -= 0.7 * DeltaTime;
		velocity.X = (velocity.X < 0)? 0 : velocity.X;
	}
	if(velocity.X < 0) {
		velocity.X += 0.7 * DeltaTime;
		velocity.X = (velocity.X > 0) ? 0 : velocity.X;
	}

	velocity.X = clamp(velocity.X, -0.025, 0.025);
	velocity.Z = clamp(velocity.Z, -0.025, 0.025);

	velocity.X += speed * side * DeltaTime;
	velocity.Z += speed * -straight * DeltaTime;

	Vector newPos(m_Ball.translation().X + velocity.X, terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5, m_Ball.translation().Z + velocity.Z);

	rotX.rotationAxis(Vector(1, 0, 0), 0.5* M_PI * straight * DeltaTime * abs(velocity.Z) * 100);
	rotZ.rotationAxis(Vector(0, 0, 1), 0.5* M_PI * side * DeltaTime * abs(velocity.X) * 100);

	m_Rotation = (rotX*rotZ);
		
	m_Ball = (m_Ball.invert() * m_Rotation).invert();

	
	m_Ball.m03 = newPos.X;
	m_Ball.m13 = newPos.Y;
	m_Ball.m23 = newPos.Z;

	if (accumulatedTime >= 2) {
		std::cout << "\nX: " << m_Ball.translation().X << " Y: " << terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5 << " Z: " << m_Ball.translation().Z<< std::endl;
		std::cout << "Vel.X: " << velocity.X << " Vel.Z: " << velocity.Z << std::endl;
		accumulatedTime = 0;
	}

	draw(DeltaTime);
	
}

void Ball::draw(float DeltaTime) {
	glPushMatrix();
	glMultMatrixf(m_Ball);
	g_Model_ball.draw();
	glPopMatrix();
	drawAxis();
}

void Ball::drawAxis() {
	Vector temp;
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0, 0, 0);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(m_Ball.translation().X + 1, m_Ball.translation().Y, m_Ball.translation().Z);

	glColor3f(0, 1.0, 0);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y+ 1, m_Ball.translation().Z);

	glColor3f(0, 0, 1.0);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z+ 1);

	glColor3f(1.0, 0, 1.0);
	temp = (m_Ball.translation() + m_Ball.up() * 2);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(temp.X, temp.Y, temp.Z);

	glColor3f(1.0, 1.0, 0);
	temp = (m_Ball.translation() + m_Ball.forward() * 2);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(temp.X, temp.Y, temp.Z);


	glColor3f(0, 1.0, 1.0);
	temp = (m_Ball.translation() + m_Ball.right()*2);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(temp.X,temp.Y,temp.Z);

	glEnd();
	glEnable(GL_LIGHTING);
	//std::cout << "Up: "<< m_Ball.up(). << std::endl;
}

float clamp(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}