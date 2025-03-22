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

	Vector *x = vector_create(4);
	Vector *y = vector_create(4);

	vector_push_back(x, -0.5);
	vector_push_back(x, 1);
	vector_push_back(x, 2);
	vector_push_back(x, 4.5);

	vector_push_back(y, 0);
	vector_push_back(y, 1.5);
	vector_push_back(y, -1.5);
	vector_push_back(y, 0);

	argus_graph_add_curve();
	argus_graph_set_current_curve(0);
	argus_curve_set_size(4);
	argus_curve_add_x(x);
	argus_curve_add_y(y);



	argus_show();

	vector_free(&x);
	vector_free(&y);

	argus_quit();
	return 0;
}