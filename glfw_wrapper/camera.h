//
// Created by freezestudio on 2017/10/13.
//

#ifndef FREEGL_CAMERA_H
#define FREEGL_CAMERA_H

namespace freeze {
    // 定义摄像机移动的几种可能选项。用作抽象以避开特定于窗口系统的输入方法
    enum camera_movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
    };

   // 摄像机缺省值
    constexpr float YAW = -90.0f;//偏航y -- 指向左侧
    constexpr float PITCH = 0.0f; //俯仰x -- 指向前方
    constexpr float SPEED = 2.5f; //速度
    constexpr float SENSITIVTY = 0.1f; //灵敏度
    constexpr float ZOOM = 45.0f;//缩放


    // 一个抽象相机类，用于处理输入并计算用于OpenGL的相应的欧拉角，向量和矩阵
    template<typename = void>
    class void_camera {
    public:
        // 摄像机属性

        //在世界坐标系中的位置
        glm::vec3 Position;
        //方向，代表z
        glm::vec3 Front;
        //向上，代表y
        glm::vec3 Up;
        //向右，代表x
        glm::vec3 Right;
        //世界坐标系中的向上方向
        glm::vec3 WorldUp;

        // 欧拉角

        float Pitch; //x 俯仰角
        float Yaw;   //y 偏航角
        //float Roll;  //z 滚转角

        // 摄像机选项

        //移动速度
        float MovementSpeed;
        //鼠标灵敏度
        float MouseSensitivity;
        //缩放
        float Zoom;

        // 使用向量构造
        void_camera(glm::vec3 position = glm::vec3{0.0f, 0.0f, 0.0f}, //位置
                    glm::vec3 up = glm::vec3{0.0f, 1.0f, 0.0f},       //世界坐标系中的向上方向
                    float yaw = YAW,                                 //y偏航
                    float pitch = PITCH )                            //x俯仰
                : Front{0.0f, 0.0f, -1.0f}
                , MovementSpeed(SPEED)
                , MouseSensitivity(SENSITIVTY)
                , Zoom(ZOOM) {

            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;

            update_camera_vectors();
        }

        // 使用标量值构造
        void_camera(float posX, float posY, float posZ,                      //位置
                    float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f,    //世界坐标系中的向上方向
                    float yaw = YAW,                                         //y偏航
                    float pitch = PITCH)                                     //x俯仰
                : Front{0.0f, 0.0f, -1.0f}
                , MovementSpeed(SPEED)
                , MouseSensitivity(SENSITIVTY)
                , Zoom(ZOOM) {

            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;

            update_camera_vectors();
        }

        // 返回使用欧拉角和LookAt矩阵计算的视图矩阵
        glm::mat4 get_view_matrix() {
            return glm::lookAt(Position, Position + Front, Up);
        }

        // 处理从任何类似键盘的输入系统接收的输入。
        // 以摄像机定义的ENUM形式接受输入参数（从窗口系统抽象）
        void process_keyboard(camera_movement direction, float deltaTime) {
            //速度
            float velocity = MovementSpeed * deltaTime;
            //W,S,A,D
            switch (direction){
                case FORWARD : Position += Front * velocity; break;
                case BACKWARD: Position -= Front * velocity; break;
                case LEFT    : Position -= Right * velocity; break;
                case RIGHT   : Position += Right * velocity; break;
            }
        }

        // 处理从鼠标输入系统接收的输入。 期望x和y方向上的偏移值。
        void process_mouse_movement(float xoffset, float yoffset, bool constrainPitch = true) {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw   += xoffset;
            Pitch += yoffset;

            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch) {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // Update Front, Right and Up Vectors using the updated Eular angles
            update_camera_vectors();
        }

        // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void process_mouse_scroll(float yoffset) {
            if (Zoom >= 1.0f && Zoom <= 45.0f)
                Zoom -= yoffset;
            if (Zoom <= 1.0f)
                Zoom = 1.0f;
            if (Zoom >= 45.0f)
                Zoom = 45.0f;
        }

    private:
        // 从相机的（更新的）欧拉角计算前向量
        void update_camera_vectors() {
            // Calculate the new Front vector
            glm::vec3 front;
            front.x = cosf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
            front.y = sinf(glm::radians(Pitch));
            front.z = sinf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
            Front = glm::normalize(front);
            // Also re-calculate the Right and Up vector
            //归一化矢量，因为它们的长度越来越接近于0，因此您的仰视越多，移动越慢。
            Right = glm::normalize(glm::cross(Front,WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }
    };
}

namespace freeze {
    using camera = void_camera<>;
}


#endif //FREEGL_CAMERA_H
