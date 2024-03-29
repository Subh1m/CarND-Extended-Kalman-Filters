#include "FusionEKF.h"
#include "tools.h"
#include "Eigen/Dense"
#include <iostream>
#include <math.h>

#define EPS 0.0001 // A very small number

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;
bool laser_flag, radar_flag;
/*
 * Constructor.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  // if this is false, laser measurements will be ignored (except during init)
  laser_flag = true;
  // if this is false, radar measurements will be ignored (except during init)
  radar_flag = true;

  previous_timestamp_ = 0;

  // initializing matrices
  R_laser_ = MatrixXd(2, 2);
  R_radar_ = MatrixXd(3, 3);
  H_laser_ = MatrixXd(2, 4);

  Hj_ = MatrixXd(3, 4);

  //measurement covariance matrix - laser
  R_laser_ << 0.0225, 0,
              0,      0.0225;

  //measurement covariance matrix - radar
  R_radar_ << 0.09, 0,      0,
              0,    0.0009, 0,
              0,    0,      0.09;

  /**
  TODO:
    * Finish initializing the FusionEKF.
    * Set the process and measurement noises
  */
  H_laser_ << 1, 0, 0, 0,
              0, 1, 0, 0;

  // Initial covariance Matrix
  ekf_.P_ = MatrixXd(4,4);
  ekf_.P_ << 10,  0,   0,   0,
             0,   10,  0,   0,
             0,   0,   100, 0,
             0,   0,   0,   100;

}

/**
* Destructor.
*/
FusionEKF::~FusionEKF() {}

void FusionEKF::ProcessMeasurement(const MeasurementPackage &measurement_pack) {


  /*****************************************************************************
   *  Initialization
   ****************************************************************************/
  if (!is_initialized_) {
    /**
      * Initialize the state ekf_.x_ with the first measurement.
      * Create the covariance matrix.
      * Remember: you'll need to convert radar from polar to cartesian coordinates.
    */
    // first measurement
    ekf_.x_ = VectorXd(4);
    cout << "measurement_type: " << measurement_pack.sensor_type_ << endl;

    if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR) {
      /**
      Convert radar from polar to cartesian coordinates and initialize state.
      */
      float rho     = measurement_pack.raw_measurements_[0]; // range
      float phi     = measurement_pack.raw_measurements_[1]; // bearing
      float rho_dot = measurement_pack.raw_measurements_[2]; // rate

      cout << "rho: " << rho << endl;
      cout << "phi: " << phi << endl;
      cout << "rho_dot: " << rho_dot << endl;

      // Normalize phi to [-pi, pi]
      while (phi > M_PI)  phi -= 2.0 * M_PI;
      while (phi < -M_PI) phi += 2.0 * M_PI;

      // Convert each coordinate
      float x  = rho * cos(phi);
      float y  = rho * sin(phi);
      float vx = rho_dot * cos(phi);
      float vy = rho_dot * sin(phi);

      ekf_.x_ << x, y, vx, vy;

    } else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER) {
      /**
      Initialize state. Default velocities to 0
      */
      float x = measurement_pack.raw_measurements_[0];
      float y = measurement_pack.raw_measurements_[1];
      ekf_.x_ << x, y, 0, 0;
    }

    // Deal with special case where values are < than a very small number
    if (fabs(ekf_.x_(0)) < EPS and fabs(ekf_.x_(1)) < EPS){
      ekf_.x_(0) = EPS;
      ekf_.x_(1) = EPS;
    }

    // Initial results
    cout << "EKF init: " << ekf_.x_ << endl;
    // Save initial timestamp
    previous_timestamp_ = measurement_pack.timestamp_;
    // done initializing, no need to predict or update
    is_initialized_ = true;
    return;
  }

  /*****************************************************************************
   *  Prediction
   ****************************************************************************/

  /**
     * Update the state transition matrix F according to the new elapsed time.
      - Time is measured in seconds.
     * Update the process noise covariance matrix.
   */
  float dt = (measurement_pack.timestamp_ - previous_timestamp_);
  dt /= 1000000.0; // dt - expressed in seconds
  previous_timestamp_ = measurement_pack.timestamp_;

  // State transiction Matrix update
  ekf_.F_ = MatrixXd(4, 4);
  ekf_.F_ << 1,  0,  dt, 0,
             0,  1,  0,  dt,
             0,  0,  1,  0,
             0,  0,  0,  1;


  // Compute the Noise Covatiance Matrix Q
  float noise_ax = 9.0;
  float noise_ay = 9.0;
  // Pre-calculate some variables for the matrix
  float dt_2   = dt * dt;
  float dt_3   = dt_2 * dt;
  float dt_4   = dt_3 * dt;
  float dt_4_4 = dt_4 / 4;
  float dt_3_2 = dt_3 / 2;

  ekf_.Q_ = MatrixXd(4, 4);
  ekf_.Q_ << dt_4_4*noise_ax, 0,               dt_3_2*noise_ax, 0,
             0,               dt_4_4*noise_ay, 0,               dt_3_2*noise_ay,
             dt_3_2*noise_ax, 0,               dt_2*noise_ax,   0,
             0,               dt_3_2*noise_ay, 0,               dt_2*noise_ay;

  ekf_.Predict();

  /*****************************************************************************
   *  Update
   ****************************************************************************/

  /**
     * Use the sensor type to perform the update step.
     * Update the state and covariance matrices.
   */

  if (measurement_pack.sensor_type_ == MeasurementPackage::RADAR && radar_flag) {
    ekf_.H_ = tools.CalculateJacobian(ekf_.x_);
    ekf_.R_ = R_radar_;
    ekf_.UpdateEKF(measurement_pack.raw_measurements_);
  } else if (measurement_pack.sensor_type_ == MeasurementPackage::LASER && laser_flag) {
    ekf_.H_ = H_laser_;
    ekf_.R_ = R_laser_;
    ekf_.Update(measurement_pack.raw_measurements_);
  }

  // print the output
  cout << "x_ = " << ekf_.x_ << endl;
  cout << "P_ = " << ekf_.P_ << endl;
}