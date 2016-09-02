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
	m_Ball.translation(StartPos);
	
	g_Model_ball.m_BoundingBox.Min.X += StartPos.X;
	g_Model_ball.m_BoundingBox.Max.X += StartPos.X;
	g_Model_ball.m_BoundingBox.Min.Y += StartPos.Y;
	g_Model_ball.m_BoundingBox.Max.Y += StartPos.Y;
	g_Model_ball.m_BoundingBox.Min.Z += StartPos.Z;
	g_Model_ball.m_BoundingBox.Max.Z += StartPos.Z;
	
	return true;
}

void Ball::steer(float ForwardBackward, float LeftRight) {
	this->straight = ForwardBackward;
	this->side = LeftRight;
	sideForce = 1;
	straightForce = 1;
}

void Ball::update(float DeltaTime, SceneNode* chunkObjects) {
	
	Matrix transM, rotX, rotZ;
	static float accumulatedTime = 0;
	accumulatedTime += DeltaTime;
	straightForce -= DeltaTime * 0.5;
	sideForce -= DeltaTime * 0.5;
	straightForce = straightForce < 0 ? 0 : straightForce;
	sideForce = sideForce < 0 ? 0 : sideForce;

	Vector camDirStraight = m_Ball.translation() - g_Camera.getPosition();
	Vector camDirSide = camDirStraight.rotationY(90);

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
	m_Translation.translation(moveVector.X, deltaY, moveVector.Z);
	//COLLISION
	Matrix mTrans = m_Ball;
	mTrans.m03 = newPos.X;
	mTrans.m13 = newPos.Y;
	mTrans.m23 = newPos.Z;
	BoundingBox checkBox = g_Model_ball.getBoundingBox() * m_Translation;
	checkBox.draw();
	SceneNode* collisionNode = chunkObjects->collision(checkBox);
	Vector nodeScale(1, 1, 1);
	bool collision = false;
	if (collisionNode != NULL) {
		collision = true;
		nodeScale = collisionNode->getScaling();
	}

	if (nodeScale.X > 2) {
		newPos.X = m_Ball.translation().X - newPos.X;
		newPos.Y = m_Ball.translation().Y - newPos.Y;
		newPos.Z = m_Ball.translation().Z - newPos.Z;
	}
	else {
		if (collision) {
			chunkObjects->removeChild(collisionNode);
			delete collisionNode;
			scale += 0.1 * nodeScale.X;
		}
		Matrix m_Scale;
		m_Scale.scale(scale);
		//(-x^2+1)*3 as rotation speed decline function
		float rotationFactorStraight = 3 - 3 * pow(1 - straightForce, 2);
		float rotationFactorSide = 3 - 3 * pow(1 - sideForce, 2);
		rotX.rotationAxis(rotationAxisStraight, 0.5* M_PI  * straight * DeltaTime * rotationFactorStraight);
		rotZ.rotationAxis(rotationAxisSide, 0.5* M_PI * -side * DeltaTime * rotationFactorSide);

		//Transformation
		m_Rotation = (rotX*rotZ);
		m_Ball = (m_Ball.invert() * m_Rotation).invert();
		
		//Set new WorldPos
		m_Ball.m03 = newPos.X;
		m_Ball.m13 = newPos.Y;
		m_Ball.m23 = newPos.Z;
		if (scale != 1) {
			m_Ball = m_Ball * m_Scale;
			scale = 1;
		}
		
		

		recalculateBoundingBox();
		//Camera transform
		g_Camera.setTarget(m_Ball.translation());
		g_Camera.setPosition(m_Translation * g_Camera.getPosition());
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

void Ball::drawBoundingBox() {
	g_Model_ball.drawBoundingBox();
}

void Ball::recalculateBoundingBox() {
	g_Model_ball.setBoundingBox(g_Model_ball.getBoundingBox()*m_Translation);
}

float clamp(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}