#pragma once

void argus_init();
void argus_quit();



void argus_set_window_size(int w, int h);
void argus_set_graph_grid(int m, int n);
void argus_set_current_graph(int x, int y);

void argus_set_update_function(void (*func)(void*, double), void *args);

void argus_show();
