/*
 * Copyright (C) 2014 Walkman
 * Author: Alessio Rocchi, Enrico Mingo
 * email:  alessio.rocchi@iit.it, enrico.mingo@iit.it
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


#include "drc_shared/utils/convex_hull.h"
#include <pcl/surface/convex_hull.h>
#include <iCub/iDynTree/yarp_kdl.h>
#include <ros/ros.h>

using namespace drc_shared;

convex_hull::convex_hull():
    _ransac_distance_thr(0.001),
    _pointCloud(new pcl::PointCloud<pcl::PointXYZ>()),
    _projectedPointCloud(new pcl::PointCloud<pcl::PointXYZ>())
{

}

convex_hull::~convex_hull()
{

}

void convex_hull::getConvexHull(const std::list<KDL::Vector>& points, yarp::sig::Matrix& A, yarp::sig::Vector& b)
{
    fromSTDList2PCLPointCloud(points, _pointCloud);

    //Filtering
    projectPCL2Plane(_pointCloud, _ransac_distance_thr, _projectedPointCloud);


    pcl::PointCloud<pcl::PointXYZ> pointsInConvexHull;
    std::vector<pcl::Vertices> indicesOfVertexes;

    // hullVertices.vertices is the list of vertices...
    // by taking each point and the consequent in the list
    // (i.e. vertices[1]-vertices[0] it is possible to compute
    // bounding segments for the hull
    pcl::ConvexHull<pcl::PointXYZ> huller;
    huller.setInputCloud (_projectedPointCloud);
    huller.reconstruct(pointsInConvexHull, indicesOfVertexes);
    if(indicesOfVertexes.size() != 1) {
        ROS_ERROR("Error: more than one polygon found!");
        return;
    } else
        pcl::Vertices hullVertices = indicesOfVertexes[0];

    //printIndexAndPointsInfo(pointsInConvexHull, indicesOfVertexes);

    getConstraints(pointsInConvexHull, indicesOfVertexes, A, b);

    _pointCloud->clear();
    _projectedPointCloud->clear();
}

pcl::PointXYZ convex_hull::fromKDLVector2PCLPointXYZ(const KDL::Vector &point)
{
    pcl::PointXYZ p;
    p.x = point.x();
    p.y = point.y();
    p.z = point.z();
    return p;
}

void convex_hull::fromSTDList2PCLPointCloud(const std::list<KDL::Vector> &points, pcl::PointCloud<pcl::PointXYZ>::Ptr point_cloud)
{
    for(std::list<KDL::Vector>::const_iterator i = points.begin(); i != points.end(); ++i)
        point_cloud->push_back(fromKDLVector2PCLPointXYZ(*i));
}

void convex_hull::getLineCoefficients(const pcl::PointXYZ &p0, const pcl::PointXYZ &p1, double &a, double &b, double&c)
{
    double x1 = p0.x;
    double x2 = p1.x;
    double y1 = p0.y;
    double y2 = p1.y;

    a = y1 - y2;
    b = x2 - x1;
    c = -b*y1 -a*x1;
}

void convex_hull::getConstraints(const pcl::PointCloud<pcl::PointXYZ> &pointsInConvexHull, const std::vector<pcl::Vertices> &indicesOfVertexes,
                                 yarp::sig::Matrix &A, yarp::sig::Vector &b)
{
    double _a, _b, _c;
    A.resize(pointsInConvexHull.size() ,2);
    b.resize(pointsInConvexHull.size());

    unsigned int z = 0;
    for(unsigned int i = 0; i < indicesOfVertexes.size(); ++i)
    {
        const pcl::Vertices& vs = indicesOfVertexes[i];
        for(unsigned int j = 0; j < vs.vertices.size(); ++j)
        {
            unsigned int k = (j + 1)%vs.vertices.size();
            getLineCoefficients(pointsInConvexHull[vs.vertices[j]], pointsInConvexHull[vs.vertices[k]], _a, _b, _c);
            if(_c <= 0.0) { // see Moleskine
                A(z,0) = + _a;
                A(z,1) = + _b;
                b[z] =   - _c;
            } else {
                A(z,0) = - _a;
                A(z,1) = - _b;
                b[z] =   + _c;
            }
            double ch_boundary = 1e-2;
            if(fabs(_c) <= ch_boundary)
                b[z] = 0.0;
            else 
                b[z] -= ch_boundary;
            z++;
        }
    }
}

void convex_hull::projectPCL2Plane(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, const double ransac_distance_thr,
                                   pcl::PointCloud<pcl::PointXYZ>::Ptr projected_point_cloud)
{
    pcl::ModelCoefficients::Ptr coefficients (new pcl::ModelCoefficients);

    //We projects ALL the points in the plane (0 0 1)
    coefficients->values.clear();
    coefficients->values.resize(4, 0.0);
    coefficients->values[0] = 0.0;
    coefficients->values[1] = 0.0;
    coefficients->values[2] = 1.0;
    coefficients->values[3] = 0.0;

//    pcl::PointIndices::Ptr inliers (new pcl::PointIndices);
//    // Create the segmentation object
//    pcl::SACSegmentation<pcl::PointXYZ> seg;
//    // Optional
//    seg.setOptimizeCoefficients (true);
//    // Mandatory
//    seg.setModelType (pcl::SACMODEL_PLANE);
//    seg.setMethodType (pcl::SAC_RANSAC);
//    seg.setDistanceThreshold (ransac_distance_thr);
//    seg.setInputCloud (cloud);
//    seg.segment (*inliers, *coefficients);


    pcl::ProjectInliers<pcl::PointXYZ> proj;
    proj.setModelType (pcl::SACMODEL_PLANE);
    proj.setInputCloud (cloud);
    proj.setModelCoefficients (coefficients);
    proj.filter (*projected_point_cloud);
}

void convex_hull::printIndexAndPointsInfo(const pcl::PointCloud<pcl::PointXYZ>& pointsInConvexHull, const std::vector<pcl::Vertices>& indicesOfVertexes)
{
    ROS_WARN("Indices of vertex has size %i", indicesOfVertexes.size());
    for(unsigned int i = 0; i < indicesOfVertexes.size(); ++i){
        pcl::Vertices vertices = indicesOfVertexes[i];
        for(unsigned int ii = 0; ii < vertices.vertices.size(); ++ii)
            ROS_INFO("vertex %i (%f, %f, %f) has index %i ", ii,
                     pointsInConvexHull.at(vertices.vertices[ii]).x, pointsInConvexHull.at(vertices.vertices[ii]).y, pointsInConvexHull.at(vertices.vertices[ii]).z,
                     vertices.vertices[ii]);
    }
}

void convex_hull::getSupportPolygonPoints(iDynUtils &robot,
                                          std::list<KDL::Vector>& points)
{
    std::vector<std::string> names;
    names.push_back("l_foot_lower_left_link");
    names.push_back("l_foot_lower_right_link");
    names.push_back("l_foot_upper_left_link");
    names.push_back("l_foot_upper_right_link");
    names.push_back("r_foot_lower_left_link");
    names.push_back("r_foot_lower_right_link");
    names.push_back("r_foot_upper_left_link");
    names.push_back("r_foot_upper_right_link");

    KDL::Frame waist_T_CoM;
    KDL::Frame waist_T_point;
    KDL::Frame CoM_T_point;
    for(unsigned int i = 0; i < names.size(); ++i)
    {
        // get points in world frame
        waist_T_point = robot.coman_iDyn3.getPositionKDL(robot.coman_iDyn3.getLinkIndex(names[i]));
        // get CoM in the world frame
        YarptoKDL(robot.coman_iDyn3.getCOM(), waist_T_CoM.p);

        CoM_T_point = waist_T_CoM.Inverse() * waist_T_point;
        points.push_back(CoM_T_point.p);
    }
}
