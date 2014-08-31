#include <gtest/gtest.h>
#include <drc_shared/cartesian_utils.h>

#include <random>

namespace {

class testQuaternion: public ::testing::Test, public quaternion
{
protected:
    testQuaternion()
    {

    }

    virtual ~testQuaternion() {

    }

    virtual void SetUp() {

    }

    virtual void TearDown() {

    }
};

class testCartesianUtils: public ::testing::Test
{
protected:
    testCartesianUtils()
    {

    }

    virtual ~testCartesianUtils() {

    }

    virtual void SetUp() {

    }

    virtual void TearDown() {

    }
};

TEST_F(testQuaternion, testQuaternionError)
{
    EXPECT_DOUBLE_EQ(this->x, 0.0);
    EXPECT_DOUBLE_EQ(this->y, 0.0);
    EXPECT_DOUBLE_EQ(this->z, 0.0);
    EXPECT_DOUBLE_EQ(this->w, 1.0);

    KDL::Rotation rot_desired;
    rot_desired.DoRotZ(M_PI_2);
    double x, y, z, w;
    rot_desired.GetQuaternion(x, y, z, w);;
    quaternion q2(x, y, z, w);
    EXPECT_DOUBLE_EQ(q2.x, x);
    EXPECT_DOUBLE_EQ(q2.y, y);
    EXPECT_DOUBLE_EQ(q2.z, z);
    EXPECT_DOUBLE_EQ(q2.w, w);

    double dot_product = quaternion::dot(quaternion(this->x, this->y, this->z, this->w), q2);
    EXPECT_DOUBLE_EQ(dot_product, w*this->w);

    std::uniform_real_distribution<double> unif;
    std::default_random_engine re;
    double a = unif(re);
    EXPECT_DOUBLE_EQ(a*q2.x, a*x);
    EXPECT_DOUBLE_EQ(a*q2.y, a*y);
    EXPECT_DOUBLE_EQ(a*q2.z, a*z);
    EXPECT_DOUBLE_EQ(a*q2.w, a*w);

    KDL::Rotation skew_q2 = q2.skew();
    EXPECT_DOUBLE_EQ(skew_q2(0,0), 0.0);   EXPECT_DOUBLE_EQ(skew_q2(0,1), -q2.z); EXPECT_DOUBLE_EQ(skew_q2(0,2), q2.y);
    EXPECT_DOUBLE_EQ(skew_q2(1,0), q2.z);  EXPECT_DOUBLE_EQ(skew_q2(1,1), 0.0);   EXPECT_DOUBLE_EQ(skew_q2(1,2), -q2.x);
    EXPECT_DOUBLE_EQ(skew_q2(2,0), -q2.y); EXPECT_DOUBLE_EQ(skew_q2(2,1), q2.x);  EXPECT_DOUBLE_EQ(skew_q2(2,2), 0.0);

    quaternion q1(q2.x, q2.y, q2.z, q2.w);
    KDL::Vector quaternion_error = quaternion::error(q1, q2);
    EXPECT_DOUBLE_EQ(quaternion_error[0], 0.0);
    EXPECT_DOUBLE_EQ(quaternion_error[1], 0.0);
    EXPECT_DOUBLE_EQ(quaternion_error[2], 0.0);
}

TEST_F(testCartesianUtils, testMatrixConversions)
{
    KDL::Rotation rot;
    rot.DoRotZ(M_PI);

    double x = 1.0;
    double y = -1.0;
    double z = -2.0;

    double Roll, Pitch, Yaw;
    rot.GetRPY(Roll, Pitch, Yaw);
    yarp::sig::Matrix T1(4,4);
    cartesian_utils::homogeneousMatrixFromRPY(T1, x, y, z, Roll, Pitch, Yaw);

    double qx, qy, qz, qw;
    yarp::sig::Matrix T2(4,4);
    rot.GetQuaternion(qx, qy, qz, qw);
    cartesian_utils::homogeneousMatrixFromQuaternion(T2, x, y, z, qx, qy, qz, qw);

    for(unsigned int i = 0; i < T1.cols(); ++i)
    {
        for(unsigned int j = 0; j < T2.rows(); ++j)
        {
            EXPECT_DOUBLE_EQ(T1(i,j), T2(i,j));
        }
    }

    KDL::Frame A;
    cartesian_utils::fromYARPMatrixtoKDLFrame(T2, A);
    for(unsigned int i = 0; i < 3; ++i)
        EXPECT_DOUBLE_EQ(A.p.data[i], T2(i,3));
    for(unsigned int i = 0; i < 3; ++i)
    {
        for(unsigned int j = 0; j < 3; ++j)
        {
            EXPECT_DOUBLE_EQ(A.M(i,j), T2(i,j));
        }
    }

    yarp::sig::Matrix B(4,4);
    cartesian_utils::fromKDLFrameToYARPMatrix(A, B);
    for(unsigned int i = 0; i < 3; ++i)
        EXPECT_DOUBLE_EQ(A.p.data[i], B(i,3));
    for(unsigned int i = 0; i < 3; ++i)
    {
        for(unsigned int j = 0; j < 3; ++j)
        {
            EXPECT_DOUBLE_EQ(A.M(i,j), B(i,j));
        }
    }
}

TEST_F(testCartesianUtils, testComputeCartesianError)
{
    yarp::sig::Vector position_error(3, 0.0);
    yarp::sig::Vector orientation_error(3, 0.0);

    KDL::Rotation rot;
    rot.DoRotZ(M_PI);

    double x = 1.0;
    double y = -1.0;
    double z = -2.0;

    double qx, qy, qz, qw;
    rot.GetQuaternion(qx, qy, qz, qw);
    yarp::sig::Matrix Td(4,4);
    cartesian_utils::homogeneousMatrixFromQuaternion(Td, x, y, z, qx, qy, qz, qw);

    yarp::sig::Matrix T(4,4);
    T = Td;

    cartesian_utils::computeCartesianError(T, Td, position_error, orientation_error);

    for(unsigned int i = 0; i < position_error.size(); ++i)
    {
        EXPECT_DOUBLE_EQ(position_error[i], 0.0);
        EXPECT_DOUBLE_EQ(orientation_error[i], 0.0);
    }
}

}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
