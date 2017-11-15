#include "ros/ros.h"

#include <sstream>
#include <simple_chat_msgs/SimpleChat.h>
#include <simple_chat_msgs/RichChat.h>
#include <simple_chat_msgs/Stat.h>


struct msgs {
    std::string login;
    std::string text;
};

struct ss {
    std::string login;
    int messages;
};

std::vector<struct msgs> msgs_array;
std::vector<struct ss> stat_array;

bool stat(simple_chat_msgs::Stat::Request  &req,
         simple_chat_msgs::Stat::Response &res)
{

    std::vector<std::string> logins;
    for (int i = 0; i < msgs_array.size(); ++i) {
        if (std::find(logins.begin(), logins.end(), msgs_array[i].login) != logins.end())
            ;
        else
            logins.push_back(msgs_array[i].login);

    }
    switch (req.stat_type) {
    case simple_chat_msgs::Stat::Request::USERS_COUNT:
        res.result  = logins.size();
        break;

    case simple_chat_msgs::Stat::Request::TOTAL_MSGS:
        res.result  = msgs_array.size();
        break;
    case simple_chat_msgs::Stat::Request::USERS_LOGINS:
        for (int i = 0; i < logins.size(); ++i) {
            res.logins.push_back(logins[i]);
        }
        break;
    case simple_chat_msgs::Stat::Request::MOST_CHATTY:
        int max = 0;
        int index = 0;
        std::string result_login;
        for (int i = 0; i < stat_array.size(); ++i) {
            if (stat_array[i].messages > max) {
                max = stat_array[i].messages;
                index = i;
            }
        }
        res.result = stat_array[index].messages;
        res.logins.push_back(stat_array[index].login);
        break;
    }

    return true;
}


void simple_cb(const simple_chat_msgs::SimpleChatPtr& input) {
    std::cout << input->text << std::endl;
    struct msgs msg;
    msg.login = "NoLogin";
    msg.text = input->text;
    msgs_array.push_back(msg);
    for (int i = 0; i < stat_array.size(); ++i) {
        if (stat_array[i].login == "NoLogin") {
            stat_array[i].messages++;
            return;
        }
    }
    ss sss;
    sss.login = "NoLogin";
    sss.messages = 1;
    stat_array.push_back(sss);

}

void rich_cb(const simple_chat_msgs::RichChatPtr& input) {
    //struct tm *tm = localtime((time_t *) &input->header.stamp.sec);
    //char date[25];
    //strftime(date, sizeof(date), "%H:%M:%S", tm);
    std::cout << input->login << "(" << input->header.stamp << "): " << input->text <<  std::endl;
    struct msgs msg;
    msg.login = input->login;
    msg.text = input->text;
    msgs_array.push_back(msg);
    for (int i = 0; i < stat_array.size(); ++i) {
        if (stat_array[i].login == input->login) {
            stat_array[i].messages++;
            return;
        }
    }
    ss sss;
    sss.login = input->login;
    sss.messages = 1;
    stat_array.push_back(sss);

}


int main(int argc, char **argv)
{
  /**
   * The ros::init() function needs to see argc and argv so that it can perform
   * any ROS arguments and name remapping that were provided at the command line.
   * For programmatic remappings you can use a different version of init() which takes
   * remappings directly, but for most command-line programs, passing argc and argv is
   * the easiest way to do it.  The third argument to init() is the name of the node.
   *
   * You must call one of the versions of ros::init() before using any other
   * part of the ROS system.
   */
  ros::init(argc, argv, "simple_chat");

  /**
   * NodeHandle is the main access point to communications with the ROS system.
   * The first NodeHandle constructed will fully initialize this node, and the last
   * NodeHandle destructed will close down the node.
   */
  ros::NodeHandle n;

  ros::Subscriber  simple_text_sub = n.subscribe("/simple_chat", 1, &simple_cb);
  ros::Subscriber  rich_text_sub = n.subscribe("/rich_chat", 1, &rich_cb);

  ros::ServiceServer service = n.advertiseService("stat", stat);

  ros::spin();

  return 0;
}

