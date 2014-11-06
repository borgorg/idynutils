/*
 * Copyright (C) 2014 Walkman
 * Author: Mirko Ferrati, Enrico Mingo, Alessio Rocchi
 * email:  mirko.ferrati@gmail.com, enrico.mingo@iit.it, alessio.rocchi@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU Lesser General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#ifndef COMANUTILS_H
#define COMANUTILS_H

#include "drc_shared/yarp_single_chain_interface.h"
#include "drc_shared/idynutils.h"
#include <yarp/math/Math.h>
#include <yarp/sig/all.h>

/**
 * @brief The ComanUtils class eases whole body control for the coman robot.
 */
class ComanUtils
{
public:
    typedef std::pair<yarp::sig::Vector, yarp::sig::Vector> Impedance;
    typedef std::map<std::string,  Impedance> ImpedanceMap;
    typedef std::map<std::string,  yarp::sig::Vector> VelocityMap;

    /**
     * @brief ComanUtils creates interfaces for all kinematic chains.
     * At creation control mode is not changed. Use the methods
     * setTorqueMode, setPositionDirectMode, setPositionMode, setImpedanceControlMode
     * to switch control mode on all chains, or call the chain methods to
     * switch mode for each kinematic chain.
     * @param moduleName the name of the module which uses the facility
     */
    ComanUtils(const std::string moduleName);

    walkman::drc::yarp_single_chain_interface right_hand, left_hand;
    walkman::drc::yarp_single_chain_interface right_arm, left_arm;
    walkman::drc::yarp_single_chain_interface torso;
    walkman::drc::yarp_single_chain_interface right_leg, left_leg;
    iDynUtils idynutils;

    /**
     * @brief hasHands check whether thboth hands are available
     * @return true if connection to both hands is successful
     */
    bool hasHands();

    /**
     * @brief sense returns position, velocities, torques sensed by the robot
     * @param q
     * @param qdot
     * @param tau
     */
    void sense(yarp::sig::Vector& q,
               yarp::sig::Vector& qdot,
               yarp::sig::Vector& tau);

    /**
     * @brief sensePosition returns the position of the robot's joints
     * @return
     */
    yarp::sig::Vector& sensePosition();

    /**
     * @brief senseVelocity returns the velocities of the robot's joints
     * @return
     */
    yarp::sig::Vector& senseVelocity();

    /**
     * @brief senseTorque returns the torques of the robot's joints
     * @return
     */
    yarp::sig::Vector& senseTorque();

    /**
     * @brief sensePosition returns the position of the robot's joints
     * @param q_left_hand a vector where the left hand position will be stored
     * @param q_right_hand a vector where the right hand position will be stored
     * @return true if hands are available
     */
    bool senseHandsPosition(yarp::sig::Vector &q_left_hand,
                            yarp::sig::Vector &q_right_hand);


    /**
     * @brief move send potision commands to all the robot joints (except the hands). Works only when the robot is in joint poisition control mode.
     * @param q the desired joint position vector
     */
    void move(const yarp::sig::Vector &q);

    /**
     * @brief move send potision commands to the robot hands.
     * @param q_left_hand the desired joint position vector for the left hand
     * @param q_right_hand the desired joint position vector for the right hand
     * @return true if hands are available
     */
    bool moveHands(const yarp::sig::Vector &q_left_hand,
                   const yarp::sig::Vector &q_right_hand);

//    /**
//     * @brief move send inputs to all robot joints. The type of input depends on the control mode.
//     * @param u the joint input.  It can imply a position command or a torque command depending on the joint control mode used.
//     */
//    void move(const std::map<std::string,yarp::sig::Vector&> &u);

//    /**
//     * @brief move send potision commands and torque offsets to all the robot joints. Works when the robot is in joint impedance or position control mode.
//     * @param q the desired joint position vector
//     * @param torqueOffset_map a map <chain_name, offset_torques> of torque offsets to send to the robot.
//     * Chains that accept a torque offset should be controlled in joint impedance mode.
//     */
//    void move(const yarp::sig::Vector &q,
//              const std::map<std::string,yarp::sig::Vector&> &torqueOffset_map);

//    /**
//     * @brief move send potision commands and torque offsets to all the robot joints. Works when the robot is in joint impedance or position control mode.
//     * @param q the desired joint position vector
//     * @param kq a map <chain_name, kq> of joint stiffness references to send to the robot.
//     * Chains that accept a torque offset should be controlled in joint impedance mode.
//     * @param torqueOffset_map a map <chain_name, offset_torques> of torque offsets to send to the robot.
//     * Chains that accept a torque offset should be controlled in joint impedance mode.
//     */
//    void move(const yarp::sig::Vector &q,
//              const std::map<std::string,yarp::sig::Vector&> &kq_map,
//              const std::map<std::string,yarp::sig::Vector&> &torqueOffset_map);

//    /**
//     * @brief move send potision commands and torque offsets to all the robot joints. Works when the robot is in joint impedance or position control mode.
//     * @param q the desired joint position vector
//     * @param kq a map <chain_name, kq> of joint stiffness references to send to the robot.
//     * Chains that accept a torque offset should be controlled in joint impedance mode.
//     * @param kd a map <chain_name, kd> of joint damping references to send to the robot.
//     * Chains that accept a torque offset should be controlled in joint impedance mode.
//     * @param torqueOffset_map a map <chain_name, offset_torques> of torque offsets to send to the robot.
//     * Chains that accept a torque offset should be controlled in joint impedance mode.
//     */
//    void move(const yarp::sig::Vector &q,
//              const std::map<std::string,yarp::sig::Vector&> &kq_map,
//              const std::map<std::string,yarp::sig::Vector&> &kd_map,
//              const std::map<std::string,yarp::sig::Vector&> &torqueOffset_map);

    /**
     * @brief setReferenceSpeeds sets the reference joint speeds used in position mode when moving, for all joints exluding hands
     * @param maximum_velocity the maximum velocity vector, a \f$R^{n_{robot\_joints}}\f$ in \f$[\frac{rad}{s}]\f$
     * @return true if all chains are in position mode and we are able to
     * succesfully set the reference velocity for position move
     * for all joints including hands
     */
    bool setReferenceSpeeds(const yarp::sig::Vector& maximum_velocity);

    /**
     * @brief setReferenceSpeeds sets the reference joint speeds used in position mode when moving
     * @param maximum_velocity_map a map<chain_name, chain_velocity_vector>
     * with chain_velocity vector a \f$R^{n_{chain\_joints}}\f$ in \f$[\frac{rad}{s}]\f$
     * @return true if all desired chains are in position mode and we are
     * able to set desired reference velocity to all desired chains
     */
    bool setReferenceSpeeds(const VelocityMap& maximum_velocity_map);

    /**
     * @brief setReferenceSpeed sets reference speed for position mode move, for all joints including hands
     * @param maximum_velocity the maximum velocity for all joints in \f$[\frac{rad}{s}]\f$
     * @return true if all chains are in position mode and we are able to
     * succesfully set the reference velocity for position move
     * for all joints including hands
     */
    bool setReferenceSpeed(const double& maximum_velocity);

    /**
     * @brief setImpedance sets stiffness for all joints except hands
     * @param Kq a \f$R^{n_\text{robot\_joints}}\f$ vector in \f$\frac{Nm}{\text{rad}}\f$,
     * the desired joint stiffness for all joints except hands
     * @param Dq \f$R^{n_\text{robot\_joints}}\f$ vector in \f$\frac{Nms}{\text{rad}}\f$,
     * the desired joint damping for all joints except hands
     * @return true if the whole robot is in impedance control mode and
     * we are able to set the desired impedance for all joints except hands
     */
    bool setImpedance(const yarp::sig::Vector& Kq, const yarp::sig::Vector& Dq);

    /**
     * @brief getImpedance returns stiffness for all joints except hands
     * @param Kq a \f$R^{n_\text{robot\_joints}}\f$ vector in \f$\frac{Nm}{\text{rad}}\f$,
     * the actual joint stiffness for all joints except hands
     * @param Dq \f$R^{n_\text{robot\_joints}}\f$ vector in \f$\frac{Nms}{\text{rad}}\f$,
     * the actual joint damping for all joints except hands
     * @return true if the whole robot is in impedance control mode and
     * we are able to get impedance for all joints except hands
     */
    bool getImpedance(yarp::sig::Vector& Kq, yarp::sig::Vector& Dq);

    /**
     * @brief setImpedance set stifness for chains defined in the impedance map
     * @param impedance_map a map<chain_name, pair<chain_stiffness_vector, chain_damping_vector>>,
     * with chain_stiffness_vector a \f$R^{n_\text{chain\_joints}}\f$ vector in \f$\frac{Nm}{\text{rad}}\f$
     * and chain_damping_vector a \f$R^{n_\text{chain\_joints}}\f$ vector in \f$\frac{Nms}{\text{rad}}\f$
     * @return true if all desired chains are in impedance mode and we are
     * able to set desired stiffness to all desired chains
     */
    bool setImpedance(const ImpedanceMap& impedance_map);

    /**
     * @brief getImpedance returns a map<chain_name, pair<chain_stiffness_vector, chain_damping_vector>>
     * @param impedance_map a map<chain_name, pair<chain_stiffness_vector, chain_damping_vector>>
     * with chain_stiffness_vector a \f$R^{n_\text{chain\_joints}}\f$ vector in \f$\frac{Nm}{\text{rad}}\f$
     * and chain_damping_vector a \f$R^{n_\text{chain\_joints}}\f$ vector in \f$\frac{Nms}{\text{rad}}\f$
     * @return true if at least a chain is in impedance control mode and we are able to
     * succesfully obtain impedance from it
     */
    bool getImpedance(ImpedanceMap& impedance_map);

    /**
     * @brief getNumberOfJoints gets the robot number of joints
     * @return the number of joints
     */
    const unsigned int& getNumberOfJoints() const;

    /**
     * @brief getJointNames returns a vector of joints, in model order
     * @return a vector of joints, in model order
     */
    const std::vector<std::string> &getJointNames() const;
    
    void fromIdynToRobot(const yarp::sig::Vector& _q,
                           yarp::sig::Vector& _right_arm,
                           yarp::sig::Vector& _left_arm,
                           yarp::sig::Vector& _torso,
                           yarp::sig::Vector& _right_leg,
                           yarp::sig::Vector& _left_leg);

    void fromRobotToIdyn(const yarp::sig::Vector &_right_arm,
                         const yarp::sig::Vector &_left_arm,
                         const yarp::sig::Vector &_torso,
                         const yarp::sig::Vector &_right_leg,
                         const yarp::sig::Vector &_left_leg,
                         yarp::sig::Vector& _q);

    /**
     * @brief setPositionMode sets position mode for all kinematic chains
     * @return true if setPositionMode for all chains is successfull
     */
    bool setPositionMode();

    /**
     * @brief getPositionMode checks the control mode for all kinematic chains is position
     * @return true if all chains are in position mode
     */
    bool isInPositionMode();

    /**
     * @brief setPositionDirectMode sets position direct mode for all kinematic chains
     * @return true if set PositionDirectMode() for all chains is successfull
     */
    bool setPositionDirectMode();

    /**
     * @brief setTorqueMode sets torque mode on all chains except hands
     * @return true if setPositionDirectMode() on hands and setTorqueMode() on all other chains is succesfull
     */
    bool setTorqueMode();

    /**
     * @brief setIdleMode sets idle mode for all chains
     * @return true if setIdleMode() is succesfull on all chains
     */
    bool setIdleMode();

    /**
     * @brief setImpedanceMode sets impedance control mode for all chains except hands
     * @return true if setPositionDirectMode() on hands and setImpedanceMode() on all other chains is succesfull
     */
    bool setImpedanceMode();

    /**
     * @brief isImpedanceMode checks the control mode of the whole robot except hands is impedance
     * @return true if all robot chains except hands are in impedance mode
     */
    bool isInImpedanceMode();

private:
    unsigned int number_of_joints;
    /// @brief q_commanded_right_arm q sento to the right hand, in robot joint ordering
    yarp::sig::Vector q_commanded_right_hand;
    /// @brief q_commanded_left_arm q sent to the left hand, in robot joint ordering
    yarp::sig::Vector q_commanded_left_hand;
    /// @brief q_commanded_left_arm q sent to the left arm, in robot joint ordering
    yarp::sig::Vector q_commanded_left_arm;
    /// @brief q_commanded_right_arm q sento to the right arm, in robot joint ordering
    yarp::sig::Vector q_commanded_right_arm;
    /// @brief q_commanded_left_leg q sento to the left leg, in robot joint ordering
    yarp::sig::Vector q_commanded_left_leg;
    /// @brief q_commanded_right_leg q sento to the right leg, in robot joint ordering
    yarp::sig::Vector q_commanded_right_leg;
    /// @brief q_commanded_torso q sento to the torso, in robot joint ordering
    yarp::sig::Vector q_commanded_torso;

    yarp::sig::Vector q_sensed;

    yarp::sig::Vector q_sensed_left_hand;
    yarp::sig::Vector q_sensed_right_hand;
    yarp::sig::Vector q_sensed_left_arm;
    yarp::sig::Vector q_sensed_right_arm;
    yarp::sig::Vector q_sensed_left_leg;
    yarp::sig::Vector q_sensed_right_leg;
    yarp::sig::Vector q_sensed_torso;

    yarp::sig::Vector qdot_sensed;

    yarp::sig::Vector qdot_sensed_left_arm;
    yarp::sig::Vector qdot_sensed_right_arm;
    yarp::sig::Vector qdot_sensed_left_leg;
    yarp::sig::Vector qdot_sensed_right_leg;
    yarp::sig::Vector qdot_sensed_torso;

    yarp::sig::Vector tau_sensed;

    yarp::sig::Vector tau_sensed_left_arm;
    yarp::sig::Vector tau_sensed_right_arm;
    yarp::sig::Vector tau_sensed_left_leg;
    yarp::sig::Vector tau_sensed_right_leg;
    yarp::sig::Vector tau_sensed_torso;

    walkman::drc::yarp_single_chain_interface* const getChainByName(const std::string chain_name);

    bool bodyIsInPositionMode();

    bool handsAreInPositionMode();
};

#endif // COMANUTILS_H
