//
// Created by freezestudio on 2017/10/13.
//

#ifndef FREEGL_CAMERA_H
#define FREEGL_CAMERA_H

namespace freeze
{
    // ����������ƶ��ļ��ֿ���ѡ�
    // ���������Աܿ��ض��ڴ���ϵͳ�����뷽��
    enum camera_movement
    {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
    };

    // �����ȱʡֵ
    constexpr float YAW = -90.0f;      //ƫ��y -- ָ�����
    constexpr float PITCH = 0.0f;      //����x -- ָ��ǰ��
    constexpr float SPEED = 2.5f;      //�ٶ�
    constexpr float SENSITIVTY = 0.1f; //������
    constexpr float ZOOM = 45.0f;      //����


    // һ����������࣬���ڴ������벢��������OpenGL����Ӧ��ŷ���ǣ������;���
    template<typename = void>
    class camera_t
    {
    public:
        // ���������

        //����������ϵ�е�λ��
        glm::vec3 Position;
        //��ǰ������z
        glm::vec3 Front;
        //���ϣ�����y
        glm::vec3 Up;
        //���ң�����x
        glm::vec3 Right;
        //��������ϵ�е����Ϸ���
        glm::vec3 WorldUp;

        // ŷ����

        float Pitch; //x ������
        float Yaw;   //y ƫ����
        //float Roll;  //z ��ת��

        // �����ѡ��

        //�ƶ��ٶ�
        float MovementSpeed;
        //���������
        float MouseSensitivity;
        //����
        float Zoom;

    public:
        // ʹ����������
        camera_t(glm::vec3 position = glm::vec3{ 0.0f, 0.0f, 0.0f }, // λ��
            glm::vec3 up = glm::vec3{ 0.0f, 1.0f, 0.0f },    // ��������ϵ�е����Ϸ���
            float yaw = YAW,                                 // yƫ��
            float pitch = PITCH)                             // x����
            : Front{ 0.0f, 0.0f, -1.0f }
            , MovementSpeed(SPEED)
            , MouseSensitivity(SENSITIVTY)
            , Zoom(ZOOM)
        {

            Position = position;
            WorldUp = up;
            Yaw = yaw;
            Pitch = pitch;

            update_camera_vectors();
        }

        // ʹ�ñ���ֵ����
        camera_t(float posX, float posY, float posZ,                 //λ��
            float upX = 0.0f, float upY = 1.0f, float upZ = 0.0f,    //��������ϵ�е����Ϸ���
            float yaw = YAW,                                         //yƫ��
            float pitch = PITCH)                                     //x����
            : Front{ 0.0f, 0.0f, -1.0f }
            , MovementSpeed(SPEED)
            , MouseSensitivity(SENSITIVTY)
            , Zoom(ZOOM)
        {

            Position = glm::vec3(posX, posY, posZ);
            WorldUp = glm::vec3(upX, upY, upZ);
            Yaw = yaw;
            Pitch = pitch;

            update_camera_vectors();
        }

        // ����ʹ��ŷ���Ǻ�LookAt����������ͼ����
        glm::mat4 get_view_matrix()
        {
            return glm::lookAt(Position, Position + Front, Up);
        }

        // ������κ����Ƽ��̵�����ϵͳ���յ����롣
        // ������������ENUM��ʽ��������������Ӵ���ϵͳ����
        void process_keyboard(camera_movement direction, float deltaTime)
        {
            //�ٶ�
            float velocity = MovementSpeed * deltaTime;
            //W,S,A,D
            switch (direction) {
            case FORWARD: Position += Front * velocity; break;
            case BACKWARD: Position -= Front * velocity; break;
            case LEFT: Position -= Right * velocity; break;
            case RIGHT: Position += Right * velocity; break;
            }
        }

        // ������������ϵͳ���յ����롣 ����x��y�����ϵ�ƫ��ֵ��
        void process_mouse_movement(float xoffset, float yoffset, 
            bool constrainPitch = true)
        {
            xoffset *= MouseSensitivity;
            yoffset *= MouseSensitivity;

            Yaw += xoffset;
            Pitch += yoffset;

            // Make sure that when pitch is out of bounds, screen doesn't get flipped
            if (constrainPitch)
            {
                if (Pitch > 89.0f)
                    Pitch = 89.0f;
                if (Pitch < -89.0f)
                    Pitch = -89.0f;
            }

            // Update Front, Right and Up Vectors using the updated Eular angles
            update_camera_vectors();
        }

        // Processes input received from a mouse scroll-wheel event.
        // Only requires input on the vertical wheel-axis
        void process_mouse_scroll(float yoffset)
        {
            if (Zoom >= 1.0f && Zoom <= 45.0f)
                Zoom -= yoffset;
            if (Zoom <= 1.0f)
                Zoom = 1.0f;
            if (Zoom >= 45.0f)
                Zoom = 45.0f;
        }

    private:
        // ������ģ����µģ�ŷ���Ǽ���ǰ����
        void update_camera_vectors()
        {
            // Calculate the new Front vector
            glm::vec3 front;
            front.x = cosf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
            front.y = sinf(glm::radians(Pitch));
            front.z = sinf(glm::radians(Yaw)) * cosf(glm::radians(Pitch));
            Front = glm::normalize(front);
            // Also re-calculate the Right and Up vector
            //��һ��ʸ������Ϊ���ǵĳ���Խ��Խ�ӽ���0�������������Խ�࣬�ƶ�Խ����
            Right = glm::normalize(glm::cross(Front, WorldUp));
            Up = glm::normalize(glm::cross(Right, Front));
        }
    };
}

namespace freeze
{
    using camera = camera_t<>;
}


#endif //FREEGL_CAMERA_H
