#ifndef RESOURCES_H
#define RESOURCES_H

const int numSectors = 64;

const float axis_rad = 0.005;
const float axis_depth = 0.03;

const float body_rad = 0.5;
const float body_depth = 0.05;

const float rim_depth = 0.05;
const float rim_orad = body_rad;
const float rim_irad = body_rad - rim_depth;


const float marks_rad = 4 * body_rad / 5.0;
const float marks_width = 0.01;
const float marks_height = 0.02;
const float marks_depth = 0.005;

const float mark_second_height = 0.002;

const float second_edge = 0.49;
const float second_x_base = 0.01;
const float second_y_base = 3 * second_x_base;
const float second_depth = 0.005;

const float minute_x_base = 0.4;
const float minute_y_base = 0.02;
const float minute_depth = 0.005;

const float hour_x_base = 0.3;
const float hour_y_base = 0.03;
const float hour_depth = 0.005;

#endif // RESOURCES_H
