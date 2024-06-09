#include "direction_service.h"


DirectionService::DirectionService() : Node("direction_service_node") 
{
  srv_ = create_service<GetDirection>("/direction_service", std::bind(&DirectionService::directionCallBack, this, _1, _2));
}

DirectionService::~DirectionService()
{
}


void DirectionService::directionCallBack(const std::shared_ptr<GetDirection::Request> request, const std::shared_ptr<GetDirection::Response> response)
{
  double min_angle = -M_PI / 2;
  double minus_sixty = -M_PI / 6;
  double sixty = M_PI / 6;
  double max_angle = M_PI / 2;
  double clear_distance = 0.5;
  double max_distance = 1.3; // 1.3 sim , 1.8 real

  double minus_pi_2_index  = int((min_angle - request->laser_data.angle_min) / request->laser_data.angle_increment);
  double minus_sixty_index = int((minus_sixty - request->laser_data.angle_min) / request->laser_data.angle_increment);
  double sixty_index       = int((sixty - request->laser_data.angle_min) / request->laser_data.angle_increment);
  double pi_2_index        = int((max_angle - request->laser_data.angle_min) / request->laser_data.angle_increment);
  
  // Right side
  double right_distance = 0;
  for (int i = minus_pi_2_index; i < minus_sixty_index; i++)
  {
    if (!std::isnan(request->laser_data.ranges[i]) && !std::isinf(request->laser_data.ranges[i]) && request->laser_data.ranges[i] > clear_distance && request->laser_data.ranges[i] < max_distance)
    {
        right_distance += request->laser_data.ranges[i];
    }   
  }
  // Middle
  double middle_distance = 0;
  for (int i = minus_sixty_index; i < sixty_index; i++)
  {
    if (!std::isnan(request->laser_data.ranges[i]) && !std::isinf(request->laser_data.ranges[i]) && request->laser_data.ranges[i] > clear_distance && request->laser_data.ranges[i] < max_distance)
    {
        middle_distance += request->laser_data.ranges[i];
    }   
  }
  // Left side 
  double left_distance = 0;
  for (int i = sixty_index; i < pi_2_index; i++)
  {
    if (!std::isnan(request->laser_data.ranges[i]) && !std::isinf(request->laser_data.ranges[i]) && request->laser_data.ranges[i] > clear_distance && request->laser_data.ranges[i] < max_distance)
    {
        left_distance += request->laser_data.ranges[i];
    }   
  }

  if (left_distance > middle_distance && left_distance > right_distance)
  {
    response->direction = "left";
  }

  else if (middle_distance > left_distance && middle_distance > right_distance) 
  {
    response->direction = "forward";
  }

  else 
  {
    response->direction = "right";
  }
}
