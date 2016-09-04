//
//  ball.h
//
//  Created by on 30.04.16.
//  Copyright © 2016 hsos. All rights reserved.
//

#ifndef Ball_h
#define Ball_h

#define _USE_MATH_DEFINES
#define NOMINMAX

#include <algorithm>

#include "PerlinNoise.h"
#include "sceneNode.h"
#include "vector.h"
#include "model.h"
#include "matrix.h"
#include "camera.h"

extern Camera g_Camera;

class Ball {
public:
	Model g_Model_ball;
	Vector aimTarget;
	float straight = 0.0;
	float side = 0.0;

	Matrix m_Ball;
	Matrix m_accRotation;
	Matrix m_accTranslation;
	Ball(float speed, PerlinNoise terrainNoise);
	~Ball();
	bool load(const char* BallModel, const Vector& StartPos);
	void steer(float ForwardBackward, float LeftRight, bool upCallback);
	void update(float DeltaTime, SceneNode* chunkObjects);
	void draw(float DeltaTime);
	void drawAxis();
	void drawBoundingBox();
	BoundingBox getBoundingBox();
private:
	float straightForce;
	float sideForce;
	float scale;
	float accumulatedScale;
	PerlinNoise terrainNoise;
	float speed;
	int rotationX;
	int rotationZ;
};

#endif /* ball_h */
