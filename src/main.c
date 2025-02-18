#include "argus.h"



int main(int argc, char *argv[]) {
	argus_init();

	argus_set_graph_grid(4,2);

	argus_show();

	argus_quit();
	return 0;
}