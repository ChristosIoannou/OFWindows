#include "ofMain.h"
#include "ParticleRiver.h"

void ParticleRiver::initParts()
{
	for (int i = 0; i < MAX_PARTICLES; i++) {
		pos[i][0] = ofRandom(0, ofGetWidth());
		pos[i][1] = ofRandom(0, ofGetHeight());
		pos[i][2] = 0;

		vel[i][0] = ofRandom(-1, 1);
		vel[i][1] = ofRandom(-1, 1);
		vel[i][2] = ofRandom(-1, 1);

		col[i][0] = 0.7f;//ofRandom(0, 1);
		col[i][1] = 0.1;//ofRandom(0, 1);
		col[i][2] = 0.1;//ofRandom(0, 1);
		col[i][3] = 0;//ofRandom(0, 1);
	}
}

//--------------------------------------------------------------
void ParticleRiver::setup() {
	ofBackground(0);
	ofSetFrameRate(30);

	int w = ofGetWidth();
	int h = ofGetHeight();
	//noise = new float[w * h];
	noise.resize(w * h);
	//memset(noise, 0x00, w * h * sizeof(float));

	drawMode = DrawMode::DrawSin;

	buildNoise();
	initParts();
	vboInited = true;
	drawMap.addListener(this, &ParticleRiver::resetSpreadLimits);

	// initialize VBO
	glGenBuffersARB(2, vbo);

	// vbo for vertex positions
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(pos), pos, GL_STREAM_DRAW_ARB);
	//	
	//	// vbo for vertex colors
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(col), col, GL_STREAM_DRAW_ARB);
}

//--------------------------------------------------------------
void ParticleRiver::update() {
	updateSins();

	int w = ofGetWidth();
	int h = ofGetHeight();

	if (drawParts) {

		for (int i = 0; i < numParticles; i++) {
			float x = pos[i][0];
			float y = pos[i][1];

			int idx_x = x;
			int idx_y = y;
			float idxNorm = (float)i / (float)numParticles;
			float a = noise[idx_x + idx_y * h] + idxNorm * circ_coeff;

			float angle = ofLerp(0.0, spread * 3.14159f, a);
			float cang = cos(angle);
			float sang = sin(angle);

			x += speed * cang;// * 0.5 + 0.4;
			y += speed * sang;// * 0.5 + 0.4;

//			float alph = 1 - ((cang * 0.5 + 0.5f) + (sang * 0.5 + 0.5))/2.0;

			if (x < 1 || x >= w || y < 1 || y >= h) {
				pos[i][0] = ofRandom(0, w);
				pos[i][1] = ofRandom(0, h);

				float x2 = pos[i][0];
				float y2 = pos[i][1];

				int idx_x2 = x2;
				int idx_y2 = y2;
				float idxNorm = i / (float)numParticles;
				float a = noise[idx_x2 + idx_y2 * h] + idxNorm * circ_coeff;

				float angle = ofLerp(0.0, spread * 3.14159f, a);
				float cang = cos(angle);
				float sang = sin(angle);

				float alph = 1 - ((cang * 0.5 + 0.5f) + (sang * 0.5 + 0.5)) / 2.0;

				col[i][0] = max(0.7f, alph * 0.95f);
				col[i][2] = min((1 - alph) * 0.7f, 0.3f);
				col[i][3] = alph;

			}
			else {
				pos[i][0] = x;
				pos[i][1] = y;

			}
		}
	}
}

//--------------------------------------------------------------
void ParticleRiver::draw() {
	int w = ofGetWidth();
	int h = ofGetHeight();

	if (drawMap) {
		glEnable(GL_DEPTH_TEST);
		glPointSize(1);
		ofEnableAlphaBlending();
		glBegin(GL_POINTS);

		for (int x = 0; x < w; x++) {
			for (int y = 0; y < h; y++) {

				float idxNorm = (x + y * h) / (float)numParticles;
				float a = noise[x + y * h];// + idxNorm * circ_coeff;	

				float angle = ofLerp(0.0, spread * 3.14159f, a);

				float v;
				if (drawMode == DrawCos)
					v = ofMap(cos(angle), -1.0f, 1.0f, 0.0f, 1.0f);
				else if (drawMode == DrawSin)
					v = ofMap(sin(angle), -1.0f, 1.0f, 0.0f, 1.0f);
				else
					v = a;
				glColor4f(1.0, 1.0f, 1.0f, v);
				glVertex2f(x, y);
			}
		}
		output = true;
		glEnd();
		glDisable(GL_DEPTH_TEST);
		ofDisableAlphaBlending();
	}

	if (drawParts) {
		ofEnableAlphaBlending();
		glPushMatrix();

		//		glTranslatef(ofGetWidth()/2, 0, 0);
		//		glRotatef(currentRot, 0, 1, 0);

		ofSetColor(255);

		glEnable(GL_DEPTH_TEST);
		glPointSize(pointSize);

		glEnable(GL_POINT_SPRITE);
		glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, numParticles * 3 * sizeof(float), pos);
		glVertexPointer(3, GL_FLOAT, 0, 0);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
		glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, numParticles * 4 * sizeof(float), col);
		glColorPointer(4, GL_FLOAT, 0, 0);


		glDrawArrays(GL_POINTS, 0, numParticles);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

		glDisable(GL_POINT_SPRITE);
		glDisable(GL_DEPTH_TEST);
		glPopMatrix();
		ofDisableAlphaBlending();

	}
}

void ParticleRiver::exit() {
	if (vboInited) 
		glDeleteBuffersARB(2, vbo);
}

// ========================================================

float ParticleRiver::cosineInterpolate(float a, float b, float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft)) * 0.5f;
	return  a * (1 - f) + b * f;
}


float ParticleRiver::Noise(int x, int y)
{
	int n = x + y * 57;
	n = (n << 13) ^ n;
	return (1.f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
}

float ParticleRiver::smoothedNoise1(float x, float y)
{
	float corners = (Noise(x - 1, y - 1) + Noise(x + 1, y - 1) + Noise(x - 1, y + 1) + Noise(x + 1, y + 1)) / 16.0f;
	float sides = (Noise(x - 1, y) + Noise(x + 1, y) + Noise(x, y - 1) + Noise(x, y + 1)) / 8.0f;
	float center = Noise(x, y) / 4.0f;
	return corners + sides + center;
}

float ParticleRiver::interpolatedNoise1(float x, float y)
{
	int integer_X = (int)floor(x);
	float fractional_X = fabs(x - integer_X);

	int integer_Y = (int)floor(y);
	float fractional_Y = fabs(y - integer_Y);

	float v1 = smoothedNoise1(integer_X, integer_Y);
	float v2 = smoothedNoise1(integer_X + 1, integer_Y);
	float v3 = smoothedNoise1(integer_X, integer_Y + 1);
	float v4 = smoothedNoise1(integer_X + 1, integer_Y + 1);

	float i1 = cosineInterpolate(v1, v2, fractional_X);
	float i2 = cosineInterpolate(v3, v4, fractional_X);

	return cosineInterpolate(i1, i2, fractional_Y);
}

float ParticleRiver::getPerlin(float x, float y)
{
	float total = 0.0f;
	float p = /*persistence*/0.5f;
	float freq = 2.0f;
	int n = /*Number_Of_Octaves*/8 - 1;

	for (int i = 0; i < n; ++i)
	{
		float frequency = pow(freq, (float)i);
		float amplitude = pow(p, (float)i);

		total = total + interpolatedNoise1(x * frequency, y * frequency) * amplitude;
	}

	return total;
}

void ParticleRiver::buildNoise() {
	int w = ofGetWidth();
	int h = ofGetHeight();
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			//float n = ofSignedNoise(x * samp_coeff + ofRandom(-rc, rc), y * samp_coeff + ofRandom(-rc, rc)) * 0.5 + 0.5;
			float n = getPerlin(x * samp_coeff + ofRandom(-rc, rc), y * samp_coeff + ofRandom(-rc, rc)) * 0.5 + 0.5;
			noise[x + y * h] = n;
		}
	}
	printf("sample_coeff %.5f\n", samp_coeff);
}

void ParticleRiver::resize() {
	int w = ofGetWidth();
	int h = ofGetHeight();
	noise.resize(w * h);
	buildNoise();
	initParts();
}

void ParticleRiver::resetSpreadLimits(bool& drawMap) {
	if (drawMap) {
		spread.setMin(0.5);
		spread.setMax(150);
	}
	else {
		spread.setMin(0.5);
		spread.setMax(20);
	}
}

void ParticleRiver::updateSins() {
	if (spreadSin)
		spread = (0.45 * spread.getMax()) * (1 + sin(ofGetFrameNum() * 0.005)) + 0.1 * spread.getMax();

	if (drawMap) 
		spread = (0.45 * spread.getMax()) * (1 + sin(ofGetFrameNum() * 0.02)) + 0.1 * spread.getMax();

	if (speedSin)
		speed = (0.45 * speed.getMax()) * (1 + sin(ofGetFrameNum() * 0.009)) + 0.1 * speed.getMax();

	if (circ_coeffSin)
		circ_coeff = (0.45 * circ_coeff.getMax()) * (1 + sin(ofGetFrameNum() * 0.013)) + 0.1 * circ_coeff.getMax();

}