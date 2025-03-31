#include "argus.h"
#include "vector.h"

#include <math.h>
#include <stdio.h>



void f(void* args, double dt) {
	static int i = 0;
	printf("---> %lf\n", dt*(i+1));
	++i;
}


#define N 1000


int main(int argc, char *argv[]) {
	(void)argc; (void)argv;

	argus_init();

	argus_set_size(1280,900);
	argus_set_grid_size(2,2);

	argus_graph_set_title("Graph 1");

	argus_set_update_function(f, NULL);
	argus_set_update_frequency(1);
	argus_set_update_duration(0.8);
	argus_set_update_timestep(0.1);


	argus_set_current_graph(1,1);
	argus_graph_set_title("Elément numéro 4");
	argus_graph_set_x_title("Amplitude (mV)");
	argus_graph_set_y_title("Temps (ms)");

	float x[N];
	float y[N];
	for (int i = 0; i < N; ++i) {
		x[i] = 2*3.14159265358979323/N*i;
		y[i] = sin(x[i]);
	}


	argus_set_screenshot_path("./screenshot/sub///");

	argus_graph_add_curve();
	argus_graph_set_current_curve(0);
	argus_curve_set_size(N);
	argus_curve_add_x_raw(x, N);
	argus_curve_add_y_raw(y, N);
	argus_graph_set_y_limits(-2,2);
	argus_curve_set_color(COLOR_TEAL);
	argus_show();

	argus_quit();
	return 0;
}