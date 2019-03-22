# CarND-Extended-Kalman-Filter

Self-Driving Car Engineer Nanodegree Program

The goal of this project is to build an Extended Kalman Filter using C++ and use it to estimate the state of a moving object of interest with noisy LIDAR and RADAR measurements.


The key metrics are RMSE values for both position and velocity of the tracke object.

## Results

The success metrics for this project are the RMSE values for 2 datasets.

The values shoule be below:
- `0.11` for `P x` and `P y`.
- `0.52` for `V x` and `V y`.

### RMSE values

The folowing table lists the results of both datasets:

| RMSE | Dataset 1 | Dataset 2 |
|------|-----------|-----------|
| P x  |  0.0977   |  0.0732   |
| P y  |  0.0854   |  0.0963   |
| V x  |  0.4512   |  0.3812   |
| V y  |  0.4704   |  0.4778   |

Here, I observed that dataset2 being the tougher one gave more precise results than dataset1.


#### Using only one senor

For both datasets a run with only one sensor, `radar` or `lidar` was also measured. 

> You can test this yourself by setting the vars `use_laser_` and `use_radar_` in `src/FusionEKF.cpp`.

Here are the results:

##### Dataset 1

| RMSE | only RADAR | only LIDAR |
|------|------------|------------|
| P x  |  0.2235    |  0.1473    |
| P y  |  0.3459    |  0.1152    |
| V x  |  0.6079    |  0.6781    |
| V y  |  0.8214    |  0.5324    |

Observations:
- The velocity metrics show low values (higher than combined) when tested separately with RADAR or LASER but the position metrics show high uncertainty.
- RADAR provides better metrics for velocity while LIDAR for position.
- Combined metrics prove to be more certain of the position and velocity.

##### Dataset 2

| RMSE | only RADAR | only LIDAR |
|------|------------|------------|
| P x  |  0.2706    |  0.1166    |
| P y  |  0.3873    |  0.1257    |
| V x  |  0.6787    |  0.5933    |
| V y  |  0.9595    |  0.5780    |

Obsevations:
- In this, the uncertainty is pretty close to Dataset 1.
- The LIDAR provides more precise metrics compared to RADAR in both cases (position and velocity).


### Images from the simulator

> With both `Radar` and `Lidar` data.

#### Dataset 1

![Kalman Filter for Dataset 1](results/kalman_dataset1.png "Dataset 1")
![Kalman Filter for Dataset 1 - Radar only](results/kalman_dataset1_radar_only.png "Dataset 1 - Radar")
![Kalman Filter for Dataset 1 - Lidar only](results/kalman_dataset1_laser_only.png "Dataset 1 - Lidar")

#### Dataset 2

![Kalman Filter for Dataset 2](results/kalman_dataset2.png "Dataset 1")
![Kalman Filter for Dataset 2 - Radar only](results/kalman_dataset2_radar_only.png "Dataset 2 - Radar")
![Kalman Filter for Dataset 2 - Lidar only](results/kalman_dataset2_laser_only.png "Dataset 2 - Lidar")

## Implementation

The main changes were to the folowing files:

- `main.cpp` - reads in data, runs the Kalman filter and calculates RMSE values after each measurement.
- `FusionEKF.cpp` - initializes the filter, calls the `Predict` function and the `Update` function
- `kalman_filter.cpp`- implementation of the `Predict` and `Update` function, for both `lidar` and `radar`.
- `tools.cpp` - tool functions to calculate `RMSE` and the `Jacobian` matrix, used to convert polar to cartesian coordinates.

### Code running instructions:

`cd /home/workspace/CarND-Extended-Kalman-Filter-Project/build/
 cmake .. && make
 ./ExtendedKF`

 

