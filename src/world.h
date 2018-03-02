#pragma once

#include <glm/glm.hpp>

extern float world_ph;
extern float world_th;
extern float world_ro;

extern bool world_fill;

void world_init();
glm::vec3 trace_ray(glm::vec3 ray, glm::vec3 camera);
float toca_triangulo(glm::vec3 origen, glm::vec3 vec, glm::vec3 V1, glm::vec3 V2, glm::vec3 V3);
unsigned char* world_display(int width, int height);
void world_clean();
