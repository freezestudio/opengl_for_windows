//
// Created by ysb on 2017/10/26.
//

#ifndef VENDOR_COLOR_H
#define VENDOR_COLOR_H

namespace freeze {
    struct color{
        float r,g,b,a;

        color() : r{0.0f},g{0.0f},b{0.0f},a{0.0f}{

        }

        color(float r,float g,float b,float a= 1.0f) : r{r},g{g},b{b},a{a}{
            glClearColor(this->r,this->g,this->b,this->a);
        }

        color(glm::vec4 rgba) : r{rgba.r},g{rgba.g},b{rgba.b},a{rgba.a}{
            glClearColor(this->r,this->g,this->b,this->a);
        }

        color(glm::vec3 rgb) : r{rgb.r},g{rgb.g},b{rgb.b},a{1.0f}{
            glClearColor(this->r,this->g,this->b,this->a);
        }

        color set(float r,float g,float b,float a=1.0){
            internal_set(r,g,b,a);
            return *this;
        }

        color set(glm::vec4 rgba){
            internal_set(rgba.r,rgba.g,rgba.b,rgba.a);
            return *this;
        }

        color set(glm::vec3 rgb){
            internal_set(rgb.r,rgb.g,rgb.b);
            return *this;
        }

        color set(){
            glClearColor(this->r,this->g,this->b,this->a);
            return *this;
        }

        color clear(){
            glClear(GL_COLOR_BUFFER_BIT);
            return *this;
        }
    private:
        void internal_set(float r,float g,float b,float a=1.0f){
            this->r=r;
            this->g=g;
            this->b=b;
            this->a=a;
            glClearColor(this->r,this->g,this->b,this->a);
        }
    };
}

namespace freeze {
    constexpr auto make_color = make<color>;
}
#endif //VENDOR_COLOR_H
