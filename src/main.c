#include "argus.h"



int main(int argc, char *argv[]) {
	argus_init();

	argus_set_size(1280,900);
	argus_set_grid_size(2,2);

	argus_graph_set_title("Graph 1");

	argus_show();

	argus_quit();
	return 0;
}