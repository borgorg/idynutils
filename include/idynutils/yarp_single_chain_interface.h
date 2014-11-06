#ifndef YARP_SINGLE_CHAIN_INTERFACE_H
#define YARP_SINGLE_CHAIN_INTERFACE_H
#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <vector>
#include <iostream>
#include <mutex>
#include <math.h>
#include <yarp/os/RateThread.h>
#include <yarp/os/BufferedPort.h>
#include <yarp/dev/IInteractionMode.h>

#define VOCAB_CM_NONE VOCAB3('d','i','o')

/**
 * These strings are supposed to be found into the SRDF of any robot we are working with
 */
namespace walkman{
    namespace robot{
        static const std::string left_hand = "left_hand";
        static const std::string right_hand = "right_hand";
        static const std::string left_arm = "left_arm";
        static const std::string right_arm = "right_arm";
        static const std::string left_leg = "left_leg";
        static const std::string right_leg = "right_leg";
        static const std::string torso = "torso";
        static const std::string chains = "chains";
        static const std::string sensors = "sensors";
        static const std::string base = "base";
    }
}

namespace walkman{
    namespace drc{

/**
 * @brief The yarp_single_chain_interface class
 */
class yarp_single_chain_interface
{
public:
    /**
     * @brief yarp_single_chain_interface is a simple interface for control of kinematic chains
     * @param kinematic_chain the name of the kinematic chain as defined in the robot srdf
     * @param robot_name the name of the robot, will be used to open polydrivers
     * @param module_prefix_with_no_slash the module name
     * @param useSI does the sense() and move() use SI units? defaults to false
     * @param controlModeVocab is the controlMode used to initialize the interface, default is IDLE
     */
    yarp_single_chain_interface(std::string kinematic_chain,
                                std::string module_prefix_with_no_slash,
                                std::string robot_name,
                                bool useSI = false,
                                const int controlModeVocab = VOCAB_CM_IDLE
                                );

    /**
     * @brief sense returns joint positions
     * @return a \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[rad]\f$ if useSI is true
     * \f$[deg]\f$ is useSI is false
     */
    virtual yarp::sig::Vector sense();

    /**
     * @brief sense returns joint positions
     * @param q_sensed \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[rad]\f$ if useSI is true
     * \f$[deg]\f$ is useSI is false
     */
    virtual void sense(yarp::sig::Vector& q_sensed);

    /**
     * @brief sensePosition returns joint positions
     * @return a \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[rad]\f$ if useSI is true
     * \f$[deg]\f$ is useSI is false
     */
    yarp::sig::Vector sensePosition();

    /**
     * @brief sensePosition returns joint positions
     * @param q_sensed \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[rad]\f$ if useSI is true
     * \f$[deg]\f$ is useSI is false
     */
    void sensePosition(yarp::sig::Vector &q_sensed);

    /**
     * @brief senseVelocity returns joint velocities
     * @return a \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[\frac{rad}{s}]\f$ if useSI is true
     * \f$[\frac{deg}{s}]\f$ is useSI is false
     */
    yarp::sig::Vector senseVelocity();

    /**
     * @brief senseVelocity returns joint velocities
     * @param velocity_sensed \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[\frac{rad}{s}]\f$ if useSI is true
     * \f$[\frac{deg}{s}]\f$ is useSI is false
     */
    void senseVelocity(yarp::sig::Vector &velocity_sensed);

    /**
     * @brief senseTorque returns joint torques
     * @return a \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[\frac{rad}{s}]\f$ if useSI is true
     * \f$[\frac{deg}{s}]\f$ is useSI is false
     */
    yarp::sig::Vector senseTorque();

    /**
     * @brief senseTorque returns joint torques
     * @param tau_sensed \f$R^{n_\text{chain\_joints}}\f$ vector which is in
     * \f$[\frac{N}]\f$
     */
    void senseTorque(yarp::sig::Vector &tau_sensed);

    /**
     * @brief move moves all joints of the chain
     * @param u_d is a \f$R^{n_\text{chain\_joints}}\f$ which is in
     * \f$[rad]\f$ if useSI is true and control mode is position or position direct
     * \f$[deg]\f$ is useSI is false and control mode is position or position direct
     * \f$[N]\f$ is control mode is torque
     */
    virtual void move(const yarp::sig::Vector& u_d);

    /**
     * @brief setReferenceSpeed set a desired reference speed vector for all joints in the chain
     * when moving using position mode
     * @param maximum_velocity a reference velocity vector \f$R^{n_\text{chain\_joints}}\f$ which is
     * in \f$\frac{rad}{s}\f$ is useSI is true
     * in \f$\frac{deg}{s}\f$ is useSI is true
     * @return true if the chain is in position mode and
     * we are able to set reference velocity for all joints in the chain
     */
    bool setReferenceSpeeds(const yarp::sig::Vector& maximum_velocity);

    /**
     * @brief setReferenceSpeed set a desired reference speed for all joints in the chain
     * when moving using position mode
     * @param maximum_velocity a reference velocity which is
     * in \f$\frac{rad}{s}\f$ is useSI is true
     * in \f$\frac{deg}{s}\f$ is useSI is true
     * @return true if the chain is in position mode and
     * we are able to set reference velocity for all joints in the chain
     */
    bool setReferenceSpeed(const double& maximum_velocity);

    /**
     * @brief setImpedance sets joint impedance for all joints in the chain
     * @param Kq a \f$R^{n_\text{chain\_joints}}\f$ desired joint stiffness vector
     * in \f$\frac{Nm}{\text{rad}}\f$ if useSI is set to true,
     * in \f$\frac{Nm}{\text{deg}}\f$ othwerwise
     * @param Dq a \f$R^{n_\text{chain\_joints}}\f$ desired joint damping vector
     * in \f$\frac{Nm}{\text{rad}}\f$ if useSI is set to true,
     * in \f$\frac{Nm}{\text{deg}}\f$ othwerwise
     * @return true if the chain is in impedance mode and we are able
     * to set impedance for all joins
     */
    bool setImpedance(const yarp::sig::Vector& Kq,
                      const yarp::sig::Vector& Dq);

    /**
     * @brief getImpedance gets joint impedance for all joints in the chain
     * @param Kq a \f$R^{n_\text{chain\_joints}}\f$ actual joint stiffness vector
     * in \f$\frac{Nm}{\text{rad}}\f$ if useSI is set to true,
     * in \f$\frac{Nm}{\text{deg}}\f$ othwerwise
     * @param Dq a \f$R^{n_\text{chain\_joints}}\f$ actual joint damping vector
     * in \f$\frac{Nm}{\text{rad}}\f$ if useSI is set to true,
     * in \f$\frac{Nm}{\text{deg}}\f$ othwerwise
     * @return true if the chain is in impedance mode and we are able
     * to get impedance for all joins
     */
    bool getImpedance(yarp::sig::Vector& Kq,
                      yarp::sig::Vector& Dq);


    const int& getNumberOfJoints() const;

    const std::string &getChainName() const;

    bool setPositionMode();

    bool isInPositionMode() const;

    bool setPositionDirectMode();

    bool  isInPositionDirectMode() const;

    bool setTorqueMode();

    bool isInTorqueMode() const;

    bool setIdleMode();

    bool isInIdleMode() const;

    bool setImpedanceMode();

    bool isInImpedanceMode() const;

    const int getControlMode() const;

    bool useSI() const;

    ~yarp_single_chain_interface();

private:

    bool createPolyDriver ( const std::string &kinematic_chain, const std::string &robot_name, yarp::dev::PolyDriver &polyDriver );
    std::string kinematic_chain;
    int joint_numbers;
    std::string module_prefix;
    yarp::sig::Vector q_buffer;
    yarp::sig::Vector qdot_buffer;
    yarp::sig::Vector tau_buffer;
    bool internal_isAvailable;
    yarp::dev::PolyDriver polyDriver;
    bool _useSI;
    int _controlMode;
    std::string _robot_name;

    void convertEncoderToSI(yarp::sig::Vector& vector);
    void convertMotorCommandFromSI(yarp::sig::Vector& vector);
    yarp::sig::Vector convertMotorCommandFromSI(const yarp::sig::Vector& vector);
    double convertMotorCommandFromSI(const double& in) const;


    int computeControlMode();
public:
    const bool& isAvailable;

    yarp::dev::IEncodersTimed *encodersMotor;
    yarp::dev::IControlMode2 *controlMode;
    yarp::dev::IInteractionMode *interactionMode;
    yarp::dev::IPositionControl2 *positionControl;
    yarp::dev::IPositionDirect *positionDirect;
    yarp::dev::IImpedanceControl *impedancePositionControl;
    yarp::dev::ITorqueControl *torqueControl;

};


}
}


#endif // YARP_SINGLE_CHAIN_INTERFACE_H
