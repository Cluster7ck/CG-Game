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
	sideForce = 1;
	straightForce = 1;
}

void Ball::update(float DeltaTime) {
	
	Matrix transM, rotX, rotZ;
	static float accumulatedTime = 0;
	accumulatedTime += DeltaTime;
	straightForce -= DeltaTime * 0.5;
	sideForce -= DeltaTime * 0.5;
	straightForce = straightForce < 0 ? 0 : straightForce;
	sideForce = sideForce < 0 ? 0 : sideForce;

	Vector camDirStraight = m_Ball.translation() - g_Camera.getPosition();
	Vector camDirSide = (m_Ball.translation() - g_Camera.getPosition()).rotationY(90);
	Vector moveVector = (camDirStraight * straight * straightForce) + (camDirSide * side * sideForce);
	Vector forwardVector = camDirStraight * straight * straightForce;
	Vector sideVector = (camDirSide * side * sideForce);

	//shorten vector if diagonal
	if (straight != 0 && side != 0) {
		moveVector = moveVector * speed * DeltaTime * 0.7f;
	}
	else {
		moveVector = moveVector * speed  * DeltaTime;
	}

	Vector rotationAxisStraight(camDirSide.X, 0, camDirSide.Z);
	Vector rotationAxisSide(camDirStraight.X, 0, camDirStraight.Z);

	float deltaY = terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5 - m_Ball.translation().Y;
	Vector newPos(m_Ball.translation().X + moveVector.X, terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5, m_Ball.translation().Z + moveVector.Z);

	//(-x^2+1)*3 as rotation speed decline function
	float rotationFactorStraight = 3 - 3 * pow(1-straightForce, 2);
	float rotationFactorSide = 3 - 3 *pow(1 - sideForce, 2);
	rotX.rotationAxis(rotationAxisStraight, 0.5* M_PI  * straight * DeltaTime * rotationFactorStraight);
	rotZ.rotationAxis(rotationAxisSide, 0.5* M_PI * -side * DeltaTime * rotationFactorSide);

	//Transformation
	m_Rotation = (rotX*rotZ);
	m_Ball = ( m_Ball.invert() * m_Rotation).invert();
	
	//Set new WorldPos
	m_Ball.m03 = newPos.X;
	m_Ball.m13 = newPos.Y;
	m_Ball.m23 = newPos.Z;
	
	if (accumulatedTime >= 2) {
		//std::cout << "\nX: " << m_Ball.translation().X << " Y: " << terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5 << " Z: " << m_Ball.translation().Z<< std::endl;
		std::cout << "Vel.X: " << moveVector.X << " Vel.Z: " << moveVector.Z << " length Forward: "<< rotationFactorStraight << std::endl;
		accumulatedTime = 0;
	}
	
	transM.translation(moveVector.X, deltaY, moveVector.Z);
	g_Camera.setTarget(m_Ball.translation());
	g_Camera.setPosition(transM * g_Camera.getPosition());
	//g_Camera.apply();

	draw(DeltaTime);
	
}

void Ball::draw(float DeltaTime) {
	
	glPushMatrix();
	glMultMatrixf(m_Ball);
	g_Model_ball.draw();
	glPopMatrix();
	drawAxis();
	
	//g_Model_ball.drawBuffer();
}

void Ball::drawAxis() {
	Vector temp;
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	//world
	glColor3f(1.0, 0, 0);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(m_Ball.translation().X + 1, m_Ball.translation().Y, m_Ball.translation().Z);

	glColor3f(0, 1.0, 0);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y+ 1, m_Ball.translation().Z);

	glColor3f(0, 0, 1.0);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z+ 1);

	//local
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