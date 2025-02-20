#pragma once

#include <stdbool.h>

#include "vao.h"



struct Axis {
	VAO *axis_vao;
	VAO *title_vao;
	const char *title;
	float min;
	float max;
	bool auto_adapt;
};

