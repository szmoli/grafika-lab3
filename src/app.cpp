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

	layout(location = 0) in vec4 wPos; // world position
	layout(location = 1) in vec2 tPos; // texture position

	out vec2 texCoord;

	void main() {
		gl_Position = MVP * wPos;
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
		wCenter = vec4(0.f, 0.f, 1.f, 1.f);
		wSize = vec4(64.f, 64.f, 0.f, 0.f);
	}

	mat4 view() {	
		return translate(vec3(-wCenter.x, -wCenter.y, 0.f));
	}

	mat4 invView() {
		return translate(vec3(wCenter.x, wCenter.y, 0.f));
	}

	mat4 projection() {
		return scale(vec3(2.f / wSize.x, 2.f / wSize.y, 1.f));
	}

	mat4 invProjection() {
		return scale(vec3(wSize.x / 2.f, wSize.y / 2.f, 1.f));	
	}

	vec4 getWSize() {
		return wSize;
	}

// -----------------------------------------
// Private
private:
	vec4 wCenter;
	vec4 wSize;
};

class Map {
// -----------------------------------------
// Public
public:
	Map() {
		wVertices[0] = vec4(-32.f, -32.f, 1.f, 1.f);
		wVertices[1] = vec4(-32.f, 32.f, 1.f, 1.f);
		wVertices[2] = vec4(32.f, -32.f, 1.f, 1.f);
		wVertices[3] = vec4(32.f, 32.f, 1.f, 1.f);
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
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
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
		glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(wVertices) / sizeof(vec4));
		// printf("sizeof(vtxs) / sizeof(vec2) = %ld\n", sizeof(wVertices) / sizeof(vec4));
		// for (int i = 0; i < 4; ++i) {
		// 	vec4 cPos = MVP * wVertices[i];
		// 	printf("w(%lf, %lf, %lf, %lf) -> c(%lf, %lf, %lf, %lf)\n", wVertices[i].x, wVertices[i].y, wVertices[i].z, wVertices[i].w, cPos.x, cPos.y, cPos.z, cPos.w);
		// }
	}

// -----------------------------------------
// Private
private:
	unsigned int vao;
	unsigned int vbo[2];
	unsigned int texture;
	vec4 wVertices[4];
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

	virtual void addWPosition(vec4 wPos) {
		wPositions.push_back(wPos);
	}

	void sync() {
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, wPositions.size() * sizeof(vec4), wPositions.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
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
	std::vector<vec4> wPositions;
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

class Sphere {
// -----------------------------------------
// Public
public:
	Sphere(Camera* camera, float radius) {
		this->camera = camera;
		this->radius = radius;
	}

	float radLatW(vec4 wPos) {
		return radians(wPos.y * (85.f / (camera->getWSize().y / 2)));
	}

	float radLonW(vec4 wPos) {
		return radians(wPos.x * (180.f / (camera->getWSize().x / 2)));
	}

	float radLatS(vec4 sPos) {
		return asin(sPos.z / radius);
	}
	
	float radLonS(vec4 sPos) {
		return atan2(sPos.y, sPos.x);
	}

	vec4 wPos(float radLon, float radLat) {
		float x = degrees(radLon) * ((camera->getWSize().x / 2) / 180.f);
		float y = degrees(radLat) * ((camera->getWSize().y / 2) / 85.f);
		return vec4(x, y, 1.f, 1.f);
	}

	vec4 sPos(float radLon, float radLat) {
		return vec4(
			radius * cos(radLat) * cos(radLon),
			radius * cos(radLat) * sin(radLon),
			radius * sin(radLat),
			1.f
		);
	}

	vec4 slerp(vec4 sP0, vec4 sP1, float t) {
		vec3 p0 = normalize(vec3(sP0));
		vec3 p1 = normalize(vec3(sP1));	
		float dotProd = dot(p0, p1);
		float omega = acos(dotProd);
		float sinOmega = sin(omega);
		float scale0 = sin((1.f - t) * omega) / sinOmega;
		float scale1 = sin(t * omega) / sinOmega;
		vec3 result = scale0 * p0 + scale1 * p1;
		return vec4(result * radius, 1.f);
	}

// -----------------------------------------
// Private
private:
	Camera* camera;
	float radius;
};

class Routes : public Object {
// -----------------------------------------
// Public
public:
	Routes(Sphere* sphere) : Object(GL_LINE_STRIP) {
		this->sphere = sphere;
	}

	void addWPosition(vec4 wPosition) override {
		if (!wPositions.empty()) {
			float lat_p0 = sphere->radLatW(wPositions.back());
			float lon_p0 = sphere->radLonW(wPositions.back());
			float lat_p1 = sphere->radLatW(wPosition);
			float lon_p1 = sphere->radLonW(wPosition);
			vec4 sP0 = sphere->sPos(lon_p0, lat_p0);
			vec4 sP1 = sphere->sPos(lon_p1, lat_p1);
			float lat_p0_s = sphere->radLatS(sP0);
			float lon_p0_s = sphere->radLonS(sP0);
			float lat_p1_s = sphere->radLatS(sP1);
			float lon_p1_s = sphere->radLonS(sP1);
			vec4 wP0 = sphere->wPos(lon_p0_s, lat_p0_s);
			vec4 wP1 = sphere->wPos(lon_p1_s, lat_p1_s);

			printf("p0 stuff:\n\tworld: (%lf, %lf, %lf, %lf)\n\tlon & lat: %lf, %lf\n\tsphere: (%lf, %lf, %lf, %lf)\n\tlon & lat: %lf, %lf\n\tworld: (%lf, %lf, %lf, %lf)\n\n", wPositions.back().x, wPositions.back().y, wPositions.back().z, wPositions.back().w, lon_p0, lat_p0, sP0.x, sP0.y, sP0.z, sP0.w, lon_p0_s, lat_p0_s, wP0.x, wP0.y, wP0.z, wP0.w);
			printf("p1 stuff:\n\tworld: (%lf, %lf, %lf, %lf)\n\tlon & lat: %lf, %lf\n\tsphere: (%lf, %lf, %lf, %lf)\n\tlon & lat: %lf, %lf\n\tworld: (%lf, %lf, %lf, %lf)\n\n", wPosition.x, wPosition.y, wPosition.z, wPosition.w, lon_p1, lat_p1, sP1.x, sP1.y, sP1.z, sP1.w, lon_p1_s, lat_p1_s, wP1.x, wP1.y, wP1.z, wP1.w);
			
			int resolution = 100;
			for (int res = 1; res < resolution; ++res) {
				float t = (float)res / (float)resolution;
				// printf("t: %lf\n", t);
				vec4 sSlerped = sphere->slerp(sP0, sP1, t);
				float lat_slerped = sphere->radLatS(sSlerped);
				float lon_slerped = sphere->radLonS(sSlerped);
				vec4 wSlerped = sphere->wPos(lon_slerped, lat_slerped);
				wPositions.push_back(wSlerped);
			}
		}

		wPositions.push_back(wPosition);
		// printf("wPositions size: %ld\n", wPositions.size());
		// for (int i = 0; i < wPositions.size(); ++i) {
		// 	printf("\t(%lf, %lf, %lf, %lf)\n", wPositions[i].x, wPositions[i].y, wPositions[i].z, wPositions[i].w);
		// }
		// printf("\n");
	}

// -----------------------------------------
// Private
private:
	mat4 WS;
	mat4 invWS;
	Sphere* sphere;
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
	Sphere* sphere;
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
		// sphere = new Sphere(1);
		sphere = new Sphere(camera, 40000 / (2 * M_PI));
		routes = new Routes(sphere);

		glPointSize(10);
		glLineWidth(3);

		onMousePressed(MouseButton::MOUSE_LEFT, 354, 176);
		onMousePressed(MouseButton::MOUSE_LEFT, 105, 201);
		onMousePressed(MouseButton::MOUSE_LEFT, 214, 320);
		onMousePressed(MouseButton::MOUSE_LEFT, 505, 204);
		onMousePressed(MouseButton::MOUSE_LEFT, 356, 175);
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
		vec4 cPos = vec4(cX, cY, 1, 1);
		vec4 wPos = cPos * invMVP;
		// vec4 sDegPos = WS * wPos; // position on the sphere in degrees
		// float degLat = sDegPos.x;
		// float degLon = sDegPos.y;
		// float radLat = radians(degLat);
		// float radLon = radians(degLon);
		
		printf("Clicked:\n\tWorld: (%lf, %lf, %lf, %lf)\n\tClip: (%lf, %lf, %lf, %lf)\n\tLon & lat (deg): %lf, %lf\n\n", wPos.x, wPos.y, wPos.z, wPos.w, cPos.x, cPos.y, cPos.z, cPos.w, degrees(sphere->radLonW(wPos)), degrees(sphere->radLatW(wPos)));
		// printf("Clicked:\n\tWorld: (%lf, %lf, %lf, %lf)\n\tSphere: (%lf, %lf, %lf, %lf)\n\tClip: (%lf, %lf, %lf, %lf)\n\n", wPos.x, wPos.y, wPos.z, wPos.w, sDegPos.x, sDegPos.y, sDegPos.z, sDegPos.w, cPos.x, cPos.y, cPos.z, cPos.w);
		// printf("Lat: %lf deg, %lf rad\nLon: %lf deg, %lf rad\n\n", degLat, radLat, degLon, radLon);

		routes->addWPosition(wPos);
		stations->addWPosition(wPos);
		refreshScreen();
	}
};

MercatorMapApp app;

