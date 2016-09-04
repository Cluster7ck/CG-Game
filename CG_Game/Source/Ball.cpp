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
	/*
	if (upCallback) {
		if (side != 0 && straight != 0){
			if (LeftRight == 0) {
				side = 0;
				sideForce = 0;
				straightForce = 1;
			}
			else if (ForwardBackward == 0) {
				straight = 0;
				straightForce = 0;
				sideForce = 1;
			}
		}	
	}
	else {
		this->straight = ForwardBackward;
		this->side = LeftRight;
		sideForce = 1;
		straightForce = 1;
	}*/
	this->straight = ForwardBackward;
	this->side = LeftRight;
	sideForce = 1;
	straightForce = 1;
}

void Ball::update(float DeltaTime, SceneNode* chunkObjects) {
	Matrix transM;
	static float accumulatedTime = 0;
	accumulatedTime += DeltaTime;
	/*
	straightForce -= DeltaTime * 0.5f;
	sideForce -= DeltaTime * 0.5f;
	straightForce = straightForce < 0 ? 0 : straightForce;
	sideForce = sideForce < 0 ? 0 : sideForce;
	*/
	Vector camDirStraight = m_Ball.translation() - g_Camera.getPosition();
	Vector camDirSide = camDirStraight.rotationY(90);

	Vector moveVector = (camDirStraight * straight) + (camDirSide * side);
	Vector forwardVector = (camDirStraight * straight).normalize();
	Vector sideVector = (camDirSide * side).normalize();
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
	checkBox.draw();
	SceneNode* collisionNode = chunkObjects->collision(checkBox);
	float nodeVolume = FLT_MIN;
	float ballVolume = this->getBoundingBox().getVolume();
	bool collision = false;
	if (collisionNode != NULL) {
		collision = true;
		nodeVolume = collisionNode->getTransformedBoundingBox().getVolume();
	}

	if (nodeVolume > ballVolume) {
		newPos.X = m_Ball.translation().X - newPos.X;
		newPos.Y = m_Ball.translation().Y - (newPos.Y);
		newPos.Z = m_Ball.translation().Z - newPos.Z;
	}
	else {
		if (collision) {
			chunkObjects->removeChild(collisionNode);
			delete collisionNode;
			scale += 0.1 * nodeVolume;
			accumulatedScale *= (1 + 0.1 * nodeVolume);
		}
		m_accTranslation = m_accTranslation * transM;

		//(-x^2+1)*3 as rotation speed decline function
		//float rotationFactorStraight = 3 - 3 * pow(1 - straightForce, 2);
		//float rotationFactorSide = 3 - 3 * pow(1 - sideForce, 2);
		Matrix rotX, rotZ, fullRotation;
		rotX.rotationAxis(rotationAxisStraight, 0.5* M_PI  * straight * DeltaTime * 3);
		rotZ.rotationAxis(rotationAxisSide, 0.5* M_PI * -side * DeltaTime * 3);

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

			scale = 1;
		}

		//Camera transform
		g_Camera.setTarget(m_Ball.translation());
		g_Camera.setPosition(transM * g_Camera.getPosition()); //, terrainNoise.GetHeight(m_Ball.translation().X, m_Ball.translation().Z)
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
	//std::cout << "Up: "<< m_Ball.up(). << std::endl;
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