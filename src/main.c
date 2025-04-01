#include "argus.h"
#include "vector.h"

#include <math.h>
#include <stdio.h>




#define N 1000


void f(float *x, float *y, double dt) {
	static int i = 0;
	*x = 2*3.14159265358979323/N*i;
	*y = sin(*x);
	++i;
}




int main(int argc, char *argv[]) {
	(void)argc; (void)argv;

	argus_init();

	argus_set_size(1280,900);
	argus_set_grid_size(2,2);

	argus_graph_set_title("Graph 1");

	argus_set_update_frequency(200);
	argus_set_update_duration(1);
	argus_set_update_timestep(0.001);


	argus_set_current_graph(1,1);
	argus_graph_set_title("Elément numéro 4");
	argus_graph_set_x_title("Amplitude (mV)");
	argus_graph_set_y_title("Temps (ms)");

	argus_graph_add_curve();
	argus_graph_set_current_curve(0);
	
	argus_curve_set_size(N);
	argus_curve_set_update_function(f);

	argus_graph_set_y_limits(-2,2);
	argus_curve_set_color(COLOR_TEAL);
	argus_show();

	argus_quit();
	return 0;
}