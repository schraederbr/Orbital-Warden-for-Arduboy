#include <Arduboy2.h>
#include <math.h>

// -----------------------
// Ship Settings & Movement
// -----------------------


// // Returns true if the point (px,py) is inside the planet polygon.
// bool pointInPlanet(float px, float py) {
//   bool inside = false;
//   // The planet is defined by circle_points.
//   // Each point in circle_points is relative to (0,0); the actual world position is offset by worldCenterX/Y.
//   int j = circle_num_points - 1;
//   for (int i = 0; i < circle_num_points; i++) {
//     float xi = circle_points[i].x + worldCenterX;
//     float yi = circle_points[i].y + worldCenterY;
//     float xj = circle_points[j].x + worldCenterX;
//     float yj = circle_points[j].y + worldCenterY;
//     // Check if the ray crosses the edge
//     if (((yi > py) != (yj > py)) &&
//          (px < (xj - xi) * (py - yi) / (yj - yi) + xi)) {
//       inside = !inside;
//     }
//     j = i;
//   }
//   return inside;
// }