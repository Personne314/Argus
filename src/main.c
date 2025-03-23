#include "argus.h"
#include "vector.h"



int main(int argc, char *argv[]) {
	(void)argc; (void)argv;

	argus_init();

	argus_set_size(1280,900);
	argus_set_grid_size(2,2);

	argus_graph_set_title("Graph 1");

	argus_set_current_graph(1,1);
	argus_graph_set_title("Elément numéro 4");
	argus_graph_set_x_title("Amplitude (mV)");
	argus_graph_set_y_title("Temps (ms)");

	float x[4] = {-0.5,1,2,4.5};
	Vector *y = vector_create(4);

	vector_push_back(y, 0);
	vector_push_back(y, 1.5);
	vector_push_back(y, -1.5);
	vector_push_back(y, 0);

	argus_graph_add_curve();
	argus_graph_set_current_curve(0);
	argus_curve_set_size(4);
	argus_curve_add_x_raw(x, 4);
	argus_curve_add_y(y);
	argus_curve_set_color(COLOR_TEAL);
	argus_show();

	vector_free(&y);

	argus_quit();
	return 0;
}