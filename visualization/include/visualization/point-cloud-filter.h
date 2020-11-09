#ifndef VISUALIZATION_POINT_CLOUD_FILTER_H_
#define VISUALIZATION_POINT_CLOUD_FILTER_H_

#include <fstream>

#include <pcl/common/transforms.h>
#include <pcl/filters/voxel_grid.h>
#include <pcl_conversions/pcl_conversions.h>
#include <pointmatcher/PointMatcher.h>
#include <pointmatcher_ros/point_cloud.h>
#include <ros/ros.h>

namespace visualization {

template <typename T>
using PclPointCloudPtr = typename boost::shared_ptr<pcl::PointCloud<T>>;

template <typename T_point>
void voxelGridPointCloud(
    const float vg_leaf_size_m, PclPointCloudPtr<T_point> cloud) {
  CHECK_NOTNULL(cloud);
  CHECK_GT(vg_leaf_size_m, 0.0f);
  pcl::VoxelGrid<T_point> voxel_grid_;
  voxel_grid_.setLeafSize(vg_leaf_size_m, vg_leaf_size_m, vg_leaf_size_m);
  voxel_grid_.setInputCloud(cloud);
  voxel_grid_.filter(*cloud);
}

template <typename T_point>
void beautifyPointCloud(
    PclPointCloudPtr<T_point> cloud, sensor_msgs::PointCloud2* cloud_msg) {
  CHECK_NOTNULL(cloud_msg);
  pcl::toROSMsg(*cloud, *cloud_msg);
  PointMatcher<double>::DataPoints cloud_points =
      PointMatcher_ros::rosMsgToPointMatcherCloud<double>(*cloud_msg);
  // todo(lbern): make fleg
  std::ifstream input_filter_config(
      "/home/berlukas/Documents/workspace/darpa/src/darpa_subt_mapping/maplab/"
      "algorithms/registration-toolbox/cfg/lpm-input-filter-for-viz.yaml");
  std::unique_ptr<PointMatcher<double>::DataPointsFilters> input_filters;
  input_filters.reset(
      new PointMatcher<double>::DataPointsFilters(input_filter_config));
  input_filters->apply(cloud_points);
  /*
  const std::string map = "map";
  *cloud_msg = PointMatcher_ros::pointMatcherCloudToRosMsg<double>(
      cloud_points, map, ros::Time::now());
      */

  const size_t n_points = cloud_points.getNbPoints();
  cloud->clear();
  cloud->resize(n_points);
  for (size_t idx = 0u; idx < n_points; ++idx) {
    // points.col(idx).head<3>() =
    cloud->points[idx].getVector3fMap() =
        cloud_points.features.col(idx).head<3>().cast<float>();
  }
}

}  // namespace visualization

#endif  // VISUALIZATION_POINT_CLOUD_FILTER_H_
