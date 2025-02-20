#include "argus.h"



int main(int argc, char *argv[]) {
	(void)argc; (void)argv;

	argus_init();

	argus_set_size(1280,900);
	argus_set_grid_size(2,2);

	argus_graph_set_title("Graph 1");

	argus_set_current_graph(1,1);
	argus_graph_set_title("Elément numéro 4");

	argus_show();

	argus_quit();
	return 0;
}