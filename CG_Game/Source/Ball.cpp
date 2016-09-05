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
	scale = 1;
	accumulatedScale = 1;
	m_Ball.translation(StartPos);
	m_accTranslation = m_Ball;

	this->straight = 0;
	this->side = 0;
	return true;
}

void Ball::steer(float ForwardBackward, float LeftRight, bool upCallback) {
	this->straight = ForwardBackward;
	this->side = LeftRight;
}

void Ball::update(float DeltaTime, SceneNode* chunkObjects) {
	Matrix transM;
	static float accumulatedTime = 0;
	accumulatedTime += DeltaTime;
	Vector camDirStraight = m_Ball.translation() - g_Camera.getPosition();
	Vector camDirSide = camDirStraight.rotationY(90);

	Vector moveVector = (camDirStraight * straight) + (camDirSide * side);
	moveVector = Vector(moveVector.X, 0, moveVector.Z);
	if (moveVector != Vector(0, 0, 0))
		moveVector.normalize();
	//shorten vector if diagonal
	if (straight != 0 && side != 0) {
		moveVector = moveVector * speed * DeltaTime ;
	}
	else {
		if (straight != 0) {
			moveVector = moveVector * speed  * DeltaTime;
		}
		else {
			moveVector = moveVector * speed  * DeltaTime;
		}
	}

	Vector rotationAxisStraight(camDirSide.X, 0, camDirSide.Z);
	Vector rotationAxisSide(camDirStraight.X, 0, camDirStraight.Z);
	
	float distToGround = (getBoundingBox().Max.Y - getBoundingBox().Min.Y) / 2;
	float deltaY = terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + distToGround - m_Ball.translation().Y;
	Vector newPos(m_Ball.translation().X + moveVector.X, terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + distToGround, m_Ball.translation().Z + moveVector.Z);
	transM.translation(moveVector.X, deltaY, moveVector.Z);
	
	//COLLISION
	BoundingBox checkBox = getBoundingBox() * transM;
	SceneNode* collisionNode = chunkObjects->collision(checkBox);
	float nodeVolume = FLT_MIN;
	float ballVolume = this->getBoundingBox().getVolume();
	bool collision = false;
	if (collisionNode != NULL) {
		collision = true;
		nodeVolume = collisionNode->getTransformedBoundingBox().getVolume();
	}

	if (nodeVolume > ballVolume) {
		newPos.X = m_Ball.translation().X - moveVector.X;
		newPos.Y = m_Ball.translation().Y; //- newPos.Y;
		newPos.Z = m_Ball.translation().Z - moveVector.Z;
	}
	else {
		if (collision) {
			chunkObjects->removeChild(collisionNode);
			delete collisionNode;
			scale += 0.1f / accumulatedScale;
			accumulatedScale *= (1 + (0.1 / accumulatedScale));
		}
		m_accTranslation = m_accTranslation * transM;

		//(-x^2+1)*3 as rotation speed decline function
		float rotationFactorStraight =clamp( accumulatedScale / 2,1,FLT_MAX);
		float rotationFactorSide = accumulatedScale;
		Matrix rotX, rotZ, fullRotation;
		rotX.rotationAxis(rotationAxisStraight, 0.5* M_PI  * straight * DeltaTime * 3 / rotationFactorStraight);
		rotZ.rotationAxis(rotationAxisSide, 0.5* M_PI * -side * DeltaTime * 3 / rotationFactorStraight);

		//Transformation
		fullRotation = (rotX*rotZ);
		m_accRotation = m_accRotation * fullRotation;
		m_Ball = (m_Ball.invert() * fullRotation).invert();
		
		//Set new WorldPos
		m_Ball.m03 = newPos.X;
		m_Ball.m13 = newPos.Y;
		m_Ball.m23 = newPos.Z;
		if (scale != 1) {
			Matrix sm;
			m_Ball = m_Ball * sm.scale(scale);
			camDirStraight = camDirStraight * ((1-scale)/accumulatedScale);
			g_Camera.setTarget(m_Ball.translation());
			g_Camera.setPosition((transM * g_Camera.getPosition())+ camDirStraight, accumulatedScale);

			scale = 1;
		}
		else{
			g_Camera.setTarget(m_Ball.translation());
			g_Camera.setPosition(transM * g_Camera.getPosition());
		}
	}

	/*
	if (accumulatedTime >= 2) {
		//std::cout << "\nX: " << m_Ball.translation().X << " Y: " << terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z) + 0.5 << " Z: " << m_Ball.translation().Z<< std::endl;
		std::cout << "Current x: "<< newPos.X << "Current y: " << newPos.Y << "Current z: " << newPos.Z << std::endl;
		accumulatedTime = 0;
	}*/
	draw(DeltaTime);
	
}


void Ball::draw(float DeltaTime) {
	glPushMatrix();
	glMultMatrixf(m_Ball);
	g_Model_ball.drawBuffer();
	glPopMatrix();
	//drawAxis();
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
}

void Ball::drawBoundingBox() {
	getBoundingBox().draw();
}

BoundingBox Ball::getBoundingBox() {
	BoundingBox tempBox = g_Model_ball.getBoundingBox();
	Matrix sm;
	tempBox = tempBox*sm.scale(accumulatedScale);
	tempBox = tempBox*m_accTranslation;
	return tempBox;
}

float clamp(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}