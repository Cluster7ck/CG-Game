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

	Vector camDirStraight = m_Ball.translation() - g_Camera.getPosition();
	Vector camDirSide = (m_Ball.translation() - g_Camera.getPosition()).rotationY(90);
	Vector moveVector = (camDirStraight * straight) + (camDirSide * side);
	
	if (moveVector != Vector(0, 0, 0))
		moveVector.normalize();

	//moveVector.X += speed * side * DeltaTime;
	//moveVector.Z += speed * -straight * DeltaTime;

	//shorten vector if diagonal
	if (straight != 0 && side != 0) {
		moveVector = moveVector * speed * DeltaTime * 0.7f;
	}
	else {
		moveVector = moveVector * speed * DeltaTime;
	}

	/*/
	//reduce speed
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

	Vector camDirStraightZeroY = Vector(camDirStraight.X, 0, camDirStraight.Z);
	Vector velForAngle = velocity;
	if (velocity != Vector(0, 0, 0)) {
	float angleForwardCamStraight = acos(camDirStraightZeroY.normalize().dot(velForAngle.normalize()));
	velocity = velocity.rotationY(angleForwardCamStraight * 180 / M_PI);
	}
	*/
	Vector rotationAxisStraight(camDirSide.X, 0, camDirSide.Z);
	Vector rotationAxisSide(camDirStraight.X, 0, camDirStraight.Z);

	float deltaY = terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5 - m_Ball.translation().Y;
	Vector newPos(m_Ball.translation().X + moveVector.X, terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5, m_Ball.translation().Z + moveVector.Z);

	rotX.rotationAxis(rotationAxisStraight, 0.5* M_PI * straight * DeltaTime * abs(moveVector.Z) * 100);
	rotZ.rotationAxis(rotationAxisSide, 0.5* M_PI * -side * DeltaTime * abs(moveVector.X) * 100);
	//rotX.rotationAxis(Vector(1,0,0), 0.5* M_PI * straight * DeltaTime * abs(velocity.Z) * 100);
	//rotZ.rotationAxis(Vector(0, 0, 1), 0.5* M_PI * side * DeltaTime * abs(velocity.X) * 100);

	//Transformation
	m_Rotation = (rotX*rotZ);
	m_Ball = ( m_Ball.invert() * m_Rotation).invert();
	
	//Set new WorldPos
	m_Ball.m03 = newPos.X;
	m_Ball.m13 = newPos.Y;
	m_Ball.m23 = newPos.Z;
	
	if (accumulatedTime >= 2) {
		//std::cout << "\nX: " << m_Ball.translation().X << " Y: " << terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5 << " Z: " << m_Ball.translation().Z<< std::endl;
		std::cout << "Vel.X: " << moveVector.X << " Vel.Z: " << moveVector.Z << std::endl;
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

	//velocity direction
	glColor3f(1.0, 0.6, 0);//orange+
	temp = (velocity * 2);
	glVertex3f(m_Ball.translation().X, m_Ball.translation().Y, m_Ball.translation().Z);
	glVertex3f(m_Ball.translation().X + temp.X, m_Ball.translation().Y, m_Ball.translation().Z + temp.Z);

	glEnd();
	glEnable(GL_LIGHTING);
	//std::cout << "Up: "<< m_Ball.up(). << std::endl;
}

float clamp(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}