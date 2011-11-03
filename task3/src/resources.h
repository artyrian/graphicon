#ifndef RESOURCES_H
#define RESOURCES_H

const int numSectors = 64;

const float axis_rad = 0.005;
const float axis_depth = 0.015;

const float ellipse_a = 0.55;
const float ellipse_b = 0.10;
const float ell_depth = 0.05;

const float body_rad = 0.5;
const float body_depth = 0.08;

const float rim_depth = 0.025;
const float rim2_depth =  rim_depth / 2.0;
const float rim_orad = body_rad - 0.03;
const float rim_irad = body_rad - 0.08;
const float back_rim_depth = 0.02;

const float benches_widht = 2 * body_rad - 0.4;
const float benches_height = 2 * body_rad + 0.01 ;
const float benches_rad = body_rad;
const float benches_depth = body_depth - 0.001;
const float benches_border = 0.12;

const float perch_rad = 0.007;
const float prism_delta = 0.035;
const float perch_length  = benches_widht - 2 * benches_border / 2;

const float belt_width = perch_length - 0.15;
const float belt_bottom_height = 0.8;
const float belt_top_height = 1.3 * belt_bottom_height;
const float belt_depth = 0.02;
const float belt_near_hole_height = 0.1;
const float belt_hole = 0.02;
const float belt_after_hole = belt_bottom_height/2.0;
const int count_holes_in_belt = 5;

const int num_winch_tri = 24;
const float winch_ledge = 0.05;
const float winch_rad = body_depth / 2.0 - 0.01;
const float winch_depth = 0.03;

const float tri_width = benches_height;
const float tri_height = 0.15;
const float tri_depth = benches_depth;

const float marks_rad = 3 * body_rad / 4.0 - 0.03;
const float marks_width = 0.01;
const float marks_height = 0.05;
const float marks_depth = 0.005;
const float mark_q_width = 0.012;

const float glass_depth = 0.03;

const float mark_second_height = 0.002;

const float second_edge = 0.46;
const float second_x_base = 0.01;
const float second_y_base = 3 * second_x_base;
const float second_depth = 0.005;

const float minute_x_base = marks_rad;
const float minute_y_base = 0.02;
const float minute_depth = 0.005;

const float hour_x_base = 0.26;
const float hour_y_base = 0.03;
const float hour_depth = 0.005;

#endif // RESOURCES_H
