//=============================================================================================
// Z�ld h�romsz�g: A framework.h oszt�lyait felhaszn�l� megold�s
//=============================================================================================
#include "../inc/framework.h"
#include <string>
#include <iostream>
#include <vector>
#include <math.h>

using namespace glm;

// cs�cspont �rnyal�
const char * vertSource = R"(
	#version 330				
    precision highp float;

	uniform mat4 MVP;

	layout(location = 0) in vec2 wPos; // world position
	layout(location = 1) in vec2 tPos; // texture position

	out vec2 texCoord;

	void main() {
		gl_Position = MVP * vec4(wPos.x, wPos.y, 1, 1);
		texCoord = tPos;
	}
)";

// pixel �rnyal�
const char * fragSource = R"(
	#version 330
    precision highp float;

	uniform sampler2D samplerUnit;
	uniform bool coloring;
	uniform vec3 color;

	in vec2 texCoord;
	
	out vec4 fragmentColor;

	void main() {
		if (coloring) {
			fragmentColor = vec4(color, 1);
		}
		else {
			fragmentColor = texture(samplerUnit, texCoord);
		}
	}
)";

class Camera {
// -----------------------------------------
// Public
public:
	Camera() {
		wCenter = vec2(0.f, 0.f);
		wSize = vec2(64.f, 64.f);
	}

	mat4 view() {	
		return translate(vec3(-wCenter.x, -wCenter.y, 0.0f));
	}

	mat4 invView() {
		return translate(vec3(wCenter.x, wCenter.y, 0));
	}

	mat4 projection() {
		return scale(vec3(2.0f / wSize.x, 2.0f / wSize.y, 1.0f));
	}

	mat4 invProjection() {
		return scale(vec3(wSize.x / 2.0f, wSize.y / 2.0f, 1.0f));	
	}

// -----------------------------------------
// Private
private:
	vec2 wCenter;
	vec2 wSize;
};

class Map {
// -----------------------------------------
// Public
public:
	Map() {
		wVertices[0] = vec2(-32.f, -32.f);
		wVertices[1] = vec2(-32.f, 32.f);
		wVertices[2] = vec2(32.f, -32.f);
		wVertices[3] = vec2(32.f, 32.f);
		tVertexUVs[0] = vec2(0.f, 0.f);
		tVertexUVs[1] = vec2(0.f, 1.f);
		tVertexUVs[2] = vec2(1.f, 0.f);
		tVertexUVs[3] = vec2(1.f, 1.f);

// -----------------------------------------
// VAO & VBO
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		// [0] -> vertex coords
		// [1] -> texture coords
		glGenBuffers(2, vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(wVertices), wVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(tVertexUVs), tVertexUVs, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

// -----------------------------------------
// Texture
		std::string imgStr = "252, 252, 252, 252, 252, 252, 252, 252, 252, 0, 9, 80, 1, 148, 13, 72, 13, 140, 25, 60, 21, 132, 41, 12, 1, 28, 25, 128, 61, 0, 17, 4, 29, 124, 81, 8, 37, 116, 89, 0, 69, 16, 5, 48, 97, 0, 77, 0, 25, 8, 1, 8, 253, 253, 253, 253, 101, 10, 237, 14, 237, 14, 241, 10, 141, 2, 93, 14, 121, 2, 5, 6, 93, 14, 49, 6, 57, 26, 89, 18, 41, 10, 57, 26, 89, 18, 41, 14, 1, 2, 45, 26, 89, 26, 33, 18, 57, 14, 93, 26, 33, 18, 57, 10, 93, 18, 5, 2, 33, 18, 41, 2, 5, 2, 5, 6, 89, 22, 29, 2, 1, 22, 37, 2, 1, 6, 1, 2, 97, 22, 29, 38, 45, 2, 97, 10, 1, 2, 37, 42, 17, 2, 13, 2, 5, 2, 89, 10, 49, 46, 25, 10, 101, 2, 5, 6, 37, 50, 9, 30, 89, 10, 9, 2, 37, 50, 5, 38, 81, 26, 45, 22, 17, 54, 77, 30, 41, 22, 17, 58, 1, 2, 61, 38, 65, 2, 9, 58, 69, 46, 37, 6, 1, 10, 9, 62, 65, 38, 5, 2, 33, 102, 57, 54, 33, 102, 57, 30, 1, 14, 33, 2, 9, 86, 9, 2, 21, 6, 13, 26, 5, 6, 53, 94, 29, 26, 1, 22, 29, 0, 29, 98, 5, 14, 9, 46, 1, 2, 5, 6, 5, 2, 0, 13, 0, 13, 118, 1, 2, 1, 42, 1, 4, 5, 6, 5, 2, 4, 33, 78, 1, 6, 1, 6, 1, 10, 5, 34, 1, 20, 2, 9, 2, 12, 25, 14, 5, 30, 1, 54, 13, 6, 9, 2, 1, 32, 13, 8, 37, 2, 13, 2, 1, 70, 49, 28, 13, 16, 53, 2, 1, 46, 1, 2, 1, 2, 53, 28, 17, 16, 57, 14, 1, 18, 1, 14, 1, 2, 57, 24, 13, 20, 57, 0, 2, 1, 2, 17, 0, 17, 2, 61, 0, 5, 16, 1, 28, 25, 0, 41, 2, 117, 56, 25, 0, 33, 2, 1, 2, 117, 52, 201, 48, 77, 0, 121, 40, 1, 0, 205, 8, 1, 0, 1, 12, 213, 4, 13, 12, 253, 253, 253, 141,";
		std::vector<unsigned char> imgBytes;
		std::string byteStr = "";
		for (char byteChar : imgStr) {
			if (byteChar == ' ') {
				// Skip spaces
				continue;
			}
			else if (byteChar == ',') {
				if (byteStr.empty()) {
					continue;
				}

				// End of byte reached -> convert string to byte -> add to imgBytes
				unsigned char imgByte = static_cast<unsigned char>(std::stoi(byteStr));
				unsigned char h = (imgByte >> 2) & 0b00111111;
				unsigned char i = imgByte & 0b00000011;
				
				// Loop for h + 1
				for (unsigned char j = 0; j <= h; ++j) {
					switch (i) {
						case 0:
							// White: R=255, G=255, B=255, A=255
							imgBytes.push_back(255); // R
							imgBytes.push_back(255); // G
							imgBytes.push_back(255); // B
							imgBytes.push_back(255); // A
							break;
						case 1:
							// Blue: R=0, G=0, B=255, A=255
							imgBytes.push_back(0);
							imgBytes.push_back(0);
							imgBytes.push_back(255);
							imgBytes.push_back(255);
							break;
						case 2:
							// Green: R=0, G=255, B=0, A=255
							imgBytes.push_back(0);
							imgBytes.push_back(255);
							imgBytes.push_back(0);
							imgBytes.push_back(255);
							break;
						case 3:
							// Black: R=0, G=0, B=0, A=255
							imgBytes.push_back(0);
							imgBytes.push_back(0);
							imgBytes.push_back(0);
							imgBytes.push_back(255);
							break;
					}
				}

				byteStr = "";
			}
			else {
				byteStr += byteChar;
			}
		}
		// printf("imgBytes size: %f\n", sqrt(imgBytes.size() / 4));

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBytes.data());
	}

	void draw(GPUProgram* prog, mat4 MVP) {
		prog->Use();
		prog->setUniform(MVP, "MVP");
		prog->setUniform(0, "samplerUnit");
		prog->setUniform(false, "coloring");
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(wVertices) / sizeof(vec2));
		// printf("sizeof(vtxs) / sizeof(vec2) = %ld\n", sizeof(vtxs) / sizeof(vec2));
	}

// -----------------------------------------
// Private
private:
	unsigned int vao;
	unsigned int vbo[2];
	unsigned int texture;
	vec2 wVertices[4];
	vec2 tVertexUVs[4];
};

class Object {
// -----------------------------------------
// Public
public:
	Object (unsigned int primitiveType) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		this->primitiveType = primitiveType;
	}

	void addWPosition(vec2 wPos) {
		wPositions.push_back(wPos);
	}

	void sync() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, wPositions.size() * sizeof(vec2), wPositions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
	}

	void draw(GPUProgram* prog, mat4 MVP, vec3 color) {
		prog->Use();
		prog->setUniform(MVP, "MVP");
		prog->setUniform(true, "coloring");
		prog->setUniform(color, "color");
		glBindVertexArray(vao);
		glDrawArrays(primitiveType, 0, wPositions.size());
	}	

// -----------------------------------------
// Protected
protected: 
	std::vector<vec2> wPositions;
	unsigned int vao;
	unsigned int vbo;
	unsigned int primitiveType;
};

class Stations : public Object {
// -----------------------------------------
// Public
public:
	Stations() : Object(GL_POINTS) {}
};

class Routes : public Object {
// -----------------------------------------
// Public
public:
	Routes() : Object(GL_LINE_STRIP) {}
};

float degrees(float radians) {
	return radians * (180 / M_PI);
}

float radians(float degrees) {
	return degrees * (M_PI / 180);
}

const int winWidth = 600, winHeight = 600;

class MercatorMapApp : public glApp {
	GPUProgram* gpuProgram;	
	Map* map;
	Camera* camera;
	Stations* stations;
	Routes* routes;
	mat4 MVP;
	mat4 invMVP;
public:
	MercatorMapApp() : glApp("Lab3") { }

	void onInitialization() override {
		camera = new Camera();
		MVP = camera->projection() * camera->view();
		invMVP = camera->invView() * camera->invProjection();

		gpuProgram = new GPUProgram(vertSource, fragSource);
		map = new Map();
		stations = new Stations();
		routes = new Routes();

		glPointSize(10);
		glLineWidth(3);
	}

	void onDisplay() override {
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, winWidth, winHeight);

		map->draw(gpuProgram, MVP);
		routes->sync();
		routes->draw(gpuProgram, MVP, vec3(1.f, 1.f, 0.f));
		stations->sync();
		stations->draw(gpuProgram, MVP, vec3(1.f, 0.f, 0.f));
	}

	void onMousePressed(MouseButton but, int pX, int pY) override {
		float cX = (2.f * pX) / winWidth - 1.f;
    	float cY = 1.f - (2.f * pY) / winHeight;
		vec4 wPos = vec4(cX, cY, 1, 1) * invMVP;
		vec4 sDegPos = scale(vec3(5.625f, 2.65625f, 1.f)) * wPos; // position on the sphere in degrees

		// printf("Clicked (device coords): (%d, %d)\n", pX, pY);
		// printf("Clicked (clip coords): (%lf, %lf)\n", cX, cY);
		// printf("Clicked (world coords): (%lf, %lf)\n", wPos.x, wPos.y);
		printf("Clicked:\n\tWorld: (%lf, %lf)\n\tSphere: (%lf, %lf)\n", wPos.x, wPos.y, sDegPos.x, sDegPos.y);

		routes->addWPosition(vec2(wPos.x, wPos.y));
		stations->addWPosition(vec2(wPos.x, wPos.y));
		refreshScreen();
	}
};

MercatorMapApp app;

