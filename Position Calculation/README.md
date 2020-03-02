# Position Calculation Overview : Indoor-Localization-Capstone-Project
This module will take data from receivers (through database or stubbed with the testing flag set to `True`) and esitmate the locaiton of the object. The position is estimated using the following process:
1. Find the "equivalent" lines for the angle of arrival(AoA)
2. Compute the point(s) of intersection between the lines
3. Find the equivalent gaussian distribution around each point of intersection (see below for further information)

## Gaussian Distribution
The gaussian distribution is calculated using the [standard formula](https://www.thoughtco.com/normal-distribution-bell-curve-formula-3126278). In this case the mean is assumed to be 0 (to provide a standardized value that can be used across all points of intersection) and the standard deviation is calculated using the maxium distance between any two intersection points, and dividing by 3. This allows means that the distribution of any given point of intersection will span to any other intersection point.

