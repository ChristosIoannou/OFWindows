#pragma once

#define MAX_PARTICLES	150000

enum DrawMode {
	DrawCos,
	DrawSin,
	DrawMap
};

class ParticleRiver {

public:

	void setup();
	void update();
	void draw();
	void exit();

	void initParts();
	void buildNoise();
	float cosineInterpolate(float a, float b, float x);
	float Noise(int x, int y);
	float smoothedNoise1(float x, float y);
	float interpolatedNoise1(float x, float y);
	float getPerlin(float x, float y);
	void resize();
	void updateSins();

	// Listeners
	void resetSpreadLimits(bool& drawMap);

	DrawMode drawMode;

	GLuint		vbo[2];

	//float* noise;
	std::vector<float> noise;

	float	pos[MAX_PARTICLES][3];
	float	vel[MAX_PARTICLES][3];
	float	col[MAX_PARTICLES][4];

	int numParticles = 150000;
	int adder = 10000;
	float currentRot = 0;
	bool vsync = false;
	bool vboInited = false;
	float pointSize = 1.4;
	float rc = 0.01;

	bool drawParts = true;
	bool output = false;
	float samp_coeff = 0.015;// 0.035f;

	ofParameter<bool> drawMap = false;
	ofParameter<float> spread = 1.0f;
	ofParameter<float> speed = 2.5f;
	ofParameter<float> samp_var = 0.005f;
	ofParameter<float> circ_coeff = 0.01; //0.25;//0.1f;
	ofParameter<bool> spreadSin = false;
	ofParameter<bool> speedSin = false;
	ofParameter<bool> circ_coeffSin = false;
};