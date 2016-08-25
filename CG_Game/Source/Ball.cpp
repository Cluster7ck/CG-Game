//
//  ball.cpp
//
//  Created by  on 30.04.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#include "../Header/ball.h"
float clamp(float n, float lower, float upper);

Ball::Ball(float speed) {
	this->speed = speed;
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
	/*
	Matrix transM, rotX, rotZ;
	rotationX = straight;
	rotationZ = side;
	if (velocity != Vector(0, 0, 0)) {
		velocity.X = -0.0005 * DeltaTime;
		velocity.X = clamp(velocity.X, 0, FLT_MAX);

		velocity.Z = -0.0005 * DeltaTime;
		velocity.Z = clamp(velocity.Z, 0, FLT_MAX);
	}

	velocity.X += speed * -side * DeltaTime;
	velocity.Z += speed * straight * DeltaTime;
	transM.translation(velocity.X, 0, velocity.Z);

	if (velocity.Z != 0 || velocity.X != 0) {
		//http://mathworld.wolfram.com/EulerAngles.html
		//http://gamedev.stackexchange.com/questions/67199/how-to-rotate-an-object-around-world-aligned-axes

		

		Vector localAxisX =  m_Ball * Vector(1, 0, 0);
		Vector localAxisZ = m_Ball * Vector(0, 0, 1);
		rotX.rotationAxis(Vector(1, 0, 0), 0.5* M_PI * straight * DeltaTime);
		rotZ.rotationAxis(Vector(0, 0, 1), 0.5* M_PI * side * DeltaTime);

		m_Rotation = (rotX*rotZ);
		//m_Ball.translation(newPos);
		//rotX.rotationX(0.5* M_PI * straight * DeltaTime * 3);
		//rotZ.rotationZ(0.5* M_PI * side * DeltaTime * 3);
		//Matrix fullRotation.rotationYawPitchRoll(Vector(0,  0.5* M_PI * straight * DeltaTime, 0.5* M_PI * side * DeltaTime));
		m_Ball = transM * m_Ball * m_Rotation;
	}
	else {
		m_Ball = transM * m_Ball;
	}
	
	*/
	
	
	
	if(straight != 0 || side != 0) {




		Vector camDirection1 = m_Ball.translation() - g_Camera.getPosition();
		Vector camDirection2 = (m_Ball.translation() - g_Camera.getPosition()).rotationY(90);
		Vector camDirection = (camDirection1 * straight * DeltaTime) + (camDirection2 * side * DeltaTime);

		if(straight != 0 && side != 0) {
			camDirection = camDirection * 0.7f;
		}

		Matrix mt1, mr1, mr2;
		mt1.translation(camDirection.X, 0, camDirection.Z);

		
		Vector rotationAxisStraight(camDirection2.X, 0, camDirection2.Z);
		Vector rotationAxisSide(camDirection1.X, 0, camDirection1.Z);
		
		//Vector localAxisX = m_Ball.transformVec4x4(rotationAxisStraight);
		//Vector localAxisZ = m_Ball.transformVec4x4(rotationAxisSide);

		mr1.rotationAxis(rotationAxisStraight, -0.5* M_PI * straight * DeltaTime);
		mr2.rotationAxis(rotationAxisSide, (90 * M_PI / 180.0) * side * DeltaTime);
		
		//mr1.rotationX(0.5* M_PI * straight * DeltaTime);
		m_Rotation = (mr1*mr2);
		m_Ball = mt1 * m_Ball * m_Rotation;

		// Kameraverfolgung
		g_Camera.setTarget(mt1 * g_Camera.getTarget());
		g_Camera.setPosition(mt1 * g_Camera.getPosition());
		g_Camera.apply();
		//std::cout << rotationAxisStraight.X << ", " << rotationAxisStraight.Y << ", " << rotationAxisStraight.Z << std::endl;
		//std::cout << rotationAxisSide.X << ", " << rotationAxisSide.Y << ", " << rotationAxisSide.Z << std::endl;
	} 
	
	draw(DeltaTime);
	
}

void Ball::draw(float DeltaTime) {
	glPushMatrix();
	glMultMatrixf(m_Ball);
	/*
	if(rotationX)
		glRotatef(0.5* M_PI * straight * DeltaTime, 1.0, 0.0, 0.0);
	if(rotationZ)
		glRotatef(0.5* M_PI * straight * DeltaTime, 0.0, 0.0, 1.0);
	*/
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