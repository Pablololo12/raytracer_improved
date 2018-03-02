#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/polar_coordinates.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "world.h"
#include "obj.h"
#include "png.h"

using namespace std;

const float PI = 3.14159265358979323846;  /* pi */


glm::mat4	view;
glm::vec3   light_pos;
glm::vec3   light_color;
glm::vec3	obj_color;
glm::vec4	dasr;

float EPSILON = 0.00001;
OBJ obj;
PNG textura;

void world_init()
{
	glm::mat4 xf = glm::rotate(glm::radians(90.0f),glm::vec3(1.0f,0.0f,0.0f));

//	obj.load("./model/cube.obj");
	obj.load("./model/teapot.obj",xf);
//	obj.load("./model/sphere.obj");
//	obj.load("./model/venus.obj",xf);
//	obj.load("./model/bunny.obj",xf);
//	obj.load("./model/dragon.obj",xf);
//	obj.load("./model/armadillo.obj",xf);
//	obj.load("./model/tyra.obj",xf);
//	obj.load("./model/nefertiti.obj");
//	obj.load("./model/Interceptor.obj",xf);
//	obj.load("./model/bb8.obj",xf);

	textura.load("./tex/checker.png");

	cout << obj.faces().size() << endl;

	light_pos = {0.0,-1.0,1.0};
	light_color = {2.0,2.0,2.0};
	obj_color = {1.0,1.0,1.0};
	dasr = {0.0,0.0,0.0,1.0};
}

float toca_triangulo(glm::vec3 origen, glm::vec3 vec, glm::vec3 V1, glm::vec3 V2, glm::vec3 V3)
{
	glm::vec3 e1 = {V2.x-V1.x, V2.y-V1.y, V2.z-V1.z};
	glm::vec3 e2 = {V3.x-V1.x, V3.y-V1.y, V3.z-V1.z};

	glm::vec3 P;
	//crossproduct(&vec, &e2, &P);
	P = glm::cross(vec,e2);

	float det = glm::dot(e1, P);

	if(det > -EPSILON && det < EPSILON) return -1.0;
	float inv_det = 1.0 / det;

	glm::vec3 T = {origen.x-V1.x, origen.y-V1.y, origen.z-V1.z};

	float u = glm::dot(T, P) * inv_det;

	if(u < 0.0 || u > 1.0) return -1.0;

	glm::vec3 Q = {0.0, 0.0, 0.0};
	Q = glm::cross(T,e1);

	float v = glm::dot(vec, Q) * inv_det;

	if(v < 0.0 || u + v > 1.0 ) return -1.0;

	float t = glm::dot(e2, Q) * inv_det;

	if(t > EPSILON) {
		return t;
	}

	return -1.0;
}

int get_triangle(glm::vec3 ray, glm::vec3 camera, float *distance)
{
	int vertex_number = obj.faces().size();
	float max_distance = std::numeric_limits<double>::infinity();
	int max_index = 0;

	for (int i = 0; i<vertex_number; i=i+3) {
		float distance = toca_triangulo(camera,ray,obj.faces()[i],obj.faces()[i+1],obj.faces()[i+2]);
		if (distance > -1.0 && max_distance > distance) {
			//cout << "Toca" << endl;
			max_distance = distance;
			max_index = i;
		}
	}

	if (max_distance == std::numeric_limits<double>::infinity()) {
		*distance = max_distance;
		return -1;
	} else {
		*distance = max_distance;
		return max_index;
	}
}

glm::vec3 color(glm::vec3 ray, glm::vec3 camera, float dist, unsigned int index)
{
	glm::vec3 punto = camera + ray*dist;
	
	glm::vec3 e1 = {obj.faces()[index+1].x-obj.faces()[index].x, obj.faces()[index+1].y-obj.faces()[index].y, obj.faces()[index+1].z-obj.faces()[index].z};
	glm::vec3 e2 = {obj.faces()[index+2].x-obj.faces()[index].x, obj.faces()[index+2].y-obj.faces()[index].y, obj.faces()[index+2].z-obj.faces()[index].z};

	glm::vec3 P;
	P = glm::cross(ray,e2);
	float det = glm::dot(e1, P);
	float inv_det = 1.0 / det;
	glm::vec3 T = {punto.x-obj.faces()[index].x, punto.y-obj.faces()[index].y, punto.z-obj.faces()[index].z};
	float u = glm::dot(T, P) * inv_det;
	glm::vec3 Q = {0.0, 0.0, 0.0};
	Q = glm::cross(T,e1);
	float v = glm::dot(ray, Q) * inv_det;

	glm::vec3 normal = obj.normals()[index]*(1.0-u-v)+obj.normals()[index+1]*u+obj.normals()[index+2]*v;
	//return glm::vec3(glm::abs(normal)*255);
	glm::vec3 coordtext = obj.texcoord()[index];
	//glm::vec3 coordtext = obj.texcoord()[index]*(1.0-u-v)+obj.texcoord()[index+1]*u+obj.texcoord()[index+2]*v;
	// A por la luz
	glm::vec3 vector_luz = light_pos-punto;
	float distance;
	int ind = get_triangle(glm::normalize(vector_luz),punto,&distance);

	if (ind != -1) {
		return {0,0,0};
	}

	float light = vector_luz.x*vector_luz.x + vector_luz.y*vector_luz.y + vector_luz.z*vector_luz.z;
	float dist_luz = sqrt(light);

	glm::vec3 color_luz = light_color/dist_luz;

	glm::vec3 omegao = {-camera.x, -camera.y, -camera.z};
	omegao = glm::normalize(omegao);
	
	//Obtenemos omega r
	float dotproduc = vector_luz.x*normal.x + vector_luz.y*normal.y + vector_luz.z*normal.z;
	glm::vec3 omegar;
	omegar.x = vector_luz.x - 2*(vector_luz.x - normal.x*dotproduc);
	omegar.y = vector_luz.y - 2*(vector_luz.y - normal.y*dotproduc);
	omegar.z = vector_luz.z - 2*(vector_luz.z - normal.z*dotproduc);
	omegar = glm::normalize(omegar);


	float dotproductIntegral = glm::dot(omegar, normal);
	if (dotproductIntegral < 0) dotproductIntegral = -dotproductIntegral;

	// Por ultimo aplicamos phong
	float dotproductPhong = glm::dot(omegao, omegar);
	if (dotproductPhong < 0) dotproductPhong = -dotproductPhong;
	dotproductPhong = pow(dotproductPhong, 5);
	
	float especular = 0.5 * (5 + 2) / 2 * dotproductPhong;

	glm::vec3 rgb={0.0,0.0,0.0};
	rgb.r = color_luz.r * (obj_color.r + especular) / M_PI * dotproductIntegral;
	rgb.g = color_luz.g * (obj_color.g + especular) / M_PI * dotproductIntegral;
	rgb.b = color_luz.b * (obj_color.b + especular) / M_PI * dotproductIntegral;
	//cout << glm::to_string(obj.texcoord()[index]) << endl;
	int index_text = coordtext.y*textura.height()*textura.width()*3 + coordtext.x*textura.width()*3;
	glm::vec3 text_color = {textura.pixels()[index_text],textura.pixels()[index_text+1],textura.pixels()[index_text+2]};

	//return glm::clamp(rgb*text_color)*255;
	return text_color*255;
}

glm::vec3 trace_ray(glm::vec3 ray, glm::vec3 camera)
{
	unsigned int vertex_number = obj.faces().size();
	unsigned int max_index = 0;
	float max_distance;

	max_index = get_triangle(ray,camera,&max_distance);

	if (max_index == -1) {
		return {0,0,0};
	} else {
		return color(ray, camera, max_distance, max_index);
	}
}


unsigned char* world_display(int width,int height)
{
	float world_ph=M_PI;//M_PI/2.0;
	float world_th=0;
	float world_ro=1.5;
	float aspect = float(width)/float(height);
	glm::vec3 to = {0.0,0.0,0.0};

	glm::mat4 pers = glm::perspective(glm::radians(45.0f),aspect,0.01f,1000.0f);
	glm::vec3 axis;
	axis.x = cos(world_ph)*cos(world_th);
	axis.y = sin(world_ph)*cos(world_th);
	axis.z = sin(world_th);

	glm::vec3 origin = to+world_ro*axis;

	glm::vec3 up = {0,0,-1};
	glm::vec3 front = -glm::normalize(world_ro*axis);
	glm::vec3 right = glm::normalize(glm::cross(front,up))*tan(glm::radians(45.0f)/2.0) * aspect;
	up = up * tan(glm::radians(45.0f)/2.0);


	unsigned char *pixels = (unsigned char*) malloc(sizeof(unsigned char)*3*width*height);

	float w_i = 2.0/width;
	float h_i = 2.0/height;
	float u = -1.0 + w_i/2.0;
	float v = -1.0 + h_i/2.0;

	//cout << glm::to_string(to+world_ro*axis) << endl;
	//cout << glm::to_string(front) << endl;
	//cout << glm::to_string(up) << endl;
	//cout << glm::to_string(right) << endl;

	for (int i = 0; i<height; i++) {
		for (int d = 0; d<width*3; d=d+3) {
			glm::vec3 RGB = {0,255,0};

			glm::vec3 ray = glm::normalize(u*right+v*up+front);

			RGB = trace_ray(ray,origin);
			
			pixels[i*width*3+d] = RGB.x;
			pixels[i*width*3+d+1] = RGB.y;
			pixels[i*width*3+d+2] = RGB.z;

			u = u + w_i;
		}
		u = -1.0;
		v = v + h_i;
	}

	return pixels;

}
