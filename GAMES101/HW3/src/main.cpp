#include <iostream>
#include <opencv2/opencv.hpp>

#include "global.hpp"
#include "rasterizer.hpp"
#include "Triangle.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "OBJ_Loader.h"
#include <opencv2/core/utils/logger.hpp>

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1,0,0,-eye_pos[0],
                 0,1,0,-eye_pos[1],
                 0,0,1,-eye_pos[2],
                 0,0,0,1;

    view = translate*view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float angle)
{
    Eigen::Matrix4f rotation;
    angle = angle * MY_PI / 180.f;
    rotation << cos(angle), 0, sin(angle), 0,
                0, 1, 0, 0,
                -sin(angle), 0, cos(angle), 0,
                0, 0, 0, 1;

    Eigen::Matrix4f scale;
    scale << 2.5, 0, 0, 0,
              0, 2.5, 0, 0,
              0, 0, 2.5, 0,
              0, 0, 0, 1;

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1;

    return translate * rotation * scale;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, // ��ֱ�Ƕ�
    float aspect_ratio, // �ݺ��
    float zNear, // ��ƽ��
    float zFar)  // Զƽ��
{
    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f pto = Eigen::Matrix4f::Identity();
    pto << zNear, 0, 0, 0,
        0, zNear, 0, 0,
        0, 0, (zNear + zFar), (-1 * zFar * zNear),
        0, 0, 1, 0;

    float halfAngle = eye_fov * MY_PI / 180.0f;
    float height = zNear * tan(halfAngle) * 2;
    float width = height * aspect_ratio;

    auto top = -zNear * tan(halfAngle / 2);
    auto right = top * aspect_ratio;
    auto left = -right;
    auto bottom = -top;


    Eigen::Matrix4f m_s = Eigen::Matrix4f::Identity();
    m_s << 2 / (right - left), 0, 0, 0,
        0, 2 / (top - bottom), 0, 0,
        0, 0, 2 / (zNear - zFar), 0,
        0, 0, 0, 1;

    Eigen::Matrix4f m_t = Eigen::Matrix4f::Identity();
    m_t << 1, 0, 0, -(right + left) / 2,
        0, 1, 0, -(top + bottom) / 2,
        0, 0, 1, -(zFar + zNear) / 2,
        0, 0, 0, 1;

    projection = m_s * m_t * pto * projection;
    return projection;

}


Eigen::Vector3f vertex_shader(const vertex_shader_payload& payload)
{
    return payload.position;
}

// ���ݷ��߽��в�ͬ��ɫ
Eigen::Vector3f normal_fragment_shader(const fragment_shader_payload& payload)
{
    // ��һ������������һ����[-1,1]������Ĳ�����������ӳ�䵽[0,1]
    Eigen::Vector3f return_color = (payload.normal.head<3>().normalized() + Eigen::Vector3f(1.0f, 1.0f, 1.0f)) / 2.f; 
    Eigen::Vector3f result;
    result << return_color.x() * 255, return_color.y() * 255, return_color.z() * 255;
    return result;
}

static Eigen::Vector3f reflect(const Eigen::Vector3f& vec, const Eigen::Vector3f& axis)
{
    auto costheta = vec.dot(axis);
    return (2 * costheta * axis - vec).normalized();
}

struct light
{
    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};

Eigen::Vector3f texture_fragment_shader(const fragment_shader_payload& payload)
{
    Eigen::Vector3f return_color = {0, 0, 0};
    if (payload.texture)
    {
        // TODO: Get the texture value at the texture coordinates of the current fragment
        return_color = payload.texture->getColor(payload.tex_coords.x(), payload.tex_coords.y());// ��ȡ������ɫ��Ϣ

    }
    Eigen::Vector3f texture_color;
    texture_color << return_color.x(), return_color.y(), return_color.z();

    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = texture_color / 255.f;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = texture_color;
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f view_dir = (eye_pos - point).normalized();  // �۲췽��
        Eigen::Vector3f light_dir = (light.position - point).normalized(); // ���շ���
        Eigen::Vector3f half_vector = (view_dir + light_dir).normalized(); // �������
        Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);
        float r2 = (light.position - point).dot(light.position - point);
        Eigen::Vector3f Ld = kd.cwiseProduct(light.intensity / r2) * std::max(0.0f, normal.dot(light_dir)); //cwiseProduct()��������Matrixֱ�ӽ��е�Ե�˷�,������ת����Array
        Eigen::Vector3f Ls = ks.cwiseProduct(light.intensity / r2) * std::pow(std::max(0.0f, normal.dot(half_vector)), p);
        result_color += La + Ld + Ls;
    }

    return result_color * 255.f;
}

Eigen::Vector3f phong_fragment_shader(const fragment_shader_payload& payload)
{
    // ���⡢�����䡢�߹�ϵ��
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    // �ƹ�λ�ú�ǿ��
    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2}; // ����
    Eigen::Vector3f amb_light_intensity{10, 10, 10}; // ������ǿ��
    Eigen::Vector3f eye_pos{0, 0, 10}; // ���λ��

    float p = 150;

    // ping point����Ϣ
    Eigen::Vector3f color = payload.color;
    Eigen::Vector3f point = payload.view_pos; // ��ά��������λ�ã����������õĵ�
    Eigen::Vector3f normal = payload.normal;  // ��ά����ķ���

    Eigen::Vector3f result_color = {0, 0, 0}; // ���ս��(RGB)

    // ����ÿһ����
    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f light_dir = (light.position - point).normalized(); // ���շ���
        Eigen::Vector3f view_dir = (eye_pos - point).normalized(); // �۲췽��
        Eigen::Vector3f half_vector = (light_dir + view_dir).normalized(); // �������
        Eigen::Vector3f I = light.intensity; // ��ǿ
        float r2 = (light.position - point).dot(light.position - point); // ˥������
        // ambient
        Eigen::Vector3f La = ka.cwiseProduct(amb_light_intensity);
        // diffuse
        Eigen::Vector3f Ld = kd.cwiseProduct(I / r2) * std::max(0.0f, normal.dot(light_dir)); // cwiseProduct()��������Matrixֱ�ӽ��е�Ե�˷�,������ת����Array
        // specular
        Eigen::Vector3f Ls = ks.cwiseProduct(I / r2) * std::pow(std::max(0.0f, normal.dot(half_vector)), p);
        result_color += La + Ld + Ls;
    }


    return result_color * 255.f;
}



Eigen::Vector3f displacement_fragment_shader(const fragment_shader_payload& payload)
{
    
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;

    float kh = 0.2, kn = 0.1;
    
    float nx = normal.x(), ny = normal.y(), nz = normal.z();
    Eigen::Vector3f t{ nx * ny / std::sqrt(nx * nx + nz * nz), std::sqrt(nx * nx + nz * nz), nz * ny / std::sqrt(nx * nx + nz * nz )};
    Eigen::Vector3f b = normal.cross(t);

    Eigen::Matrix3f TBN;
    TBN << t.x(), b.x(), normal.x(),
        t.y(), b.x(), normal.y(),
        t.z(), b.z(), normal.z();

    float u = payload.tex_coords.x(), v = payload.tex_coords.y(); // ����������Ƕ�Ӧ��ͼ��uv�����ֵ
    float w = payload.texture->width, h = payload.texture->height; //����ĳ��ȺͿ��

    float dU = kh * kn * (payload.texture->getColorBilinear(u + 1.0f / w, v).norm() - payload.texture->getColorBilinear(u, v).norm());
    float dV = kh * kn * (payload.texture->getColorBilinear(u, v + 1.0f / h).norm() - payload.texture->getColorBilinear(u, v).norm());

    Eigen::Vector3f ln(-dU, -dV, 1);
    //�밼͹��ͼ������
    point += (kn * normal * payload.texture->getColorBilinear(u, v).norm());//ֱ�Ӹı��˶�������ĸ߶�
    normal = (TBN * ln).normalized(); //��ʱ�����Ѿ��任��[0,1]������Χ��

    // TODO: Implement displacement mapping here
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Position p = p + kn * n * h(u,v)
    // Normal n = normalize(TBN * ln)


    Eigen::Vector3f result_color = {0, 0, 0};

    for (auto& light : lights)
    {
        // TODO: For each light source in the code, calculate what the *ambient*, *diffuse*, and *specular* 
        // components are. Then, accumulate that result on the *result_color* object.
        Eigen::Vector3f l = (light.position - point).normalized(), v = (eye_pos - point).normalized();// ���շ���͹۲췽��
        Eigen::Vector3f h = (l + v).normalized();// �������
        Eigen::Vector3f I = light.intensity;// ��ǿ
        float r2 = (light.position - point).dot(light.position - point);
        Eigen::Vector3f Ld = kd.cwiseProduct(I / r2) * std::max(0.0f, normal.dot(l));//cwiseProduct()��������Matrixֱ�ӽ��е�Ե�˷�,������ת����Array
        Eigen::Vector3f Ls = ks.cwiseProduct(I / r2) * std::pow(std::max(0.0f, normal.dot(h)), p);
        result_color += Ld + Ls;
    }

    return result_color * 255.f;
}

// �Է��߽����Ŷ�Ȼ����ɫ
Eigen::Vector3f bump_fragment_shader(const fragment_shader_payload& payload)
{
    
    Eigen::Vector3f ka = Eigen::Vector3f(0.005, 0.005, 0.005);
    Eigen::Vector3f kd = payload.color;
    Eigen::Vector3f ks = Eigen::Vector3f(0.7937, 0.7937, 0.7937);

    auto l1 = light{{20, 20, 20}, {500, 500, 500}};
    auto l2 = light{{-20, 20, 0}, {500, 500, 500}};

    std::vector<light> lights = {l1, l2};
    Eigen::Vector3f amb_light_intensity{10, 10, 10};
    Eigen::Vector3f eye_pos{0, 0, 10};

    float p = 150;

    Eigen::Vector3f color = payload.color; 
    Eigen::Vector3f point = payload.view_pos;
    Eigen::Vector3f normal = payload.normal;


    float kh = 0.2, kn = 0.1;

    // TODO: Implement bump mapping here
    float nx = normal.x(), ny = normal.y(), nz = normal.z();
    // t�����߿ռ��x����������ռ��µı�ʾ
    Eigen::Vector3f t{ nx * ny / sqrt(nx * nx + nz * nz), std::sqrt(nx * nx + nz * nz), nz * ny / std::sqrt(nx * nx + nz * nz) };  
    // b�����߿ռ��y���������������µı�ʾ
    Eigen::Vector3f b = normal.cross(t);

    // TBN�����߿ռ�任������ռ�ľ���ע��ռ䶨��ı任����
    Eigen::Matrix3f TBN;
    TBN << t.x(), b.x(), normal.x(),
        t.y(), b.y(), normal.y(),
        t.z(), b.z(), normal.z();

    float u = payload.tex_coords.x(), v = payload.tex_coords.y(); // ����������Ƕ�Ӧ��ͼ��uv�����ֵ
    float w = payload.texture->width, h = payload.texture->height; //����ĳ��ȺͿ��

    // ����ͼ�ϵ���ɫֵ������Ǵ洢��ʸ����Ϣ������ʸ���������RGBֵ
    float dU = kh * kn * (payload.texture->getColor(u + 1.0f / w, v).norm() - payload.texture->getColor(u, v).norm());
    float dV = kh * kn * (payload.texture->getColor(u, v + 1.0f / h).norm() - payload.texture->getColor(u, v).norm());

    Eigen::Vector3f ln{ -dU, -dV, 1.0f }; //lnΪ�Ŷ�֮��ķ��ߣ���ʱ���������߿ռ��еı�ʾ����
    normal = (TBN * ln).normalized(); //��ʱ�����Ѿ��任��[0,1]������Χ��
    // Let n = normal = (x, y, z)
    // Vector t = (x*y/sqrt(x*x+z*z),sqrt(x*x+z*z),z*y/sqrt(x*x+z*z))
    // Vector b = n cross product t
    // Matrix TBN = [t b n]
    // dU = kh * kn * (h(u+1/w,v)-h(u,v))
    // dV = kh * kn * (h(u,v+1/h)-h(u,v))
    // Vector ln = (-dU, -dV, 1)
    // Normal n = normalize(TBN * ln)

    Eigen::Vector3f result_color = {0, 0, 0};
    result_color = normal;

    return result_color * 255.f;
}



int main(int argc, const char** argv)
{

    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT); // see: https://blog.csdn.net/benobug/article/details/117653268

    std::vector<Triangle*> TriangleList;

    float angle = 140.0;
    bool command_line = false;

    // �ļ�Ŀ¼
    std::string filename = "output.png";
    objl::Loader Loader;
    std::string obj_path = "../../GAMES101/resources/models/spot/";

    // ���� .obj File
    bool loadout = Loader.LoadFile("../../GAMES101/resources/models/spot/spot_triangulated_good.obj");

    // ����ģ�͵�ÿ����
    for(auto mesh:Loader.LoadedMeshes)
    {   
        // ��¼ͼ��ÿ�����������������㣨С�����Σ�
        for(int i=0;i<mesh.Vertices.size();i+=3)
        {
            Triangle* t = new Triangle();
            // ��ÿ��С�����εĶ�����Ϣ��¼��Triangle���t��
            for(int j=0;j<3;j++)
            {
                t->setVertex(j,Vector4f(mesh.Vertices[i+j].Position.X,mesh.Vertices[i+j].Position.Y,mesh.Vertices[i+j].Position.Z,1.0)); // ����
                t->setNormal(j,Vector3f(mesh.Vertices[i+j].Normal.X,mesh.Vertices[i+j].Normal.Y,mesh.Vertices[i+j].Normal.Z)); // ���㷨�������Դ˵�Ϊ�������������(�Ľ�)�εķ�����֮�ͼ�Ϊ���㷨������
                t->setTexCoord(j,Vector2f(mesh.Vertices[i+j].TextureCoordinate.X, mesh.Vertices[i+j].TextureCoordinate.Y));  // ����
            }
            // ����������Ϣ�����б�
            TriangleList.push_back(t);
        }
    }

    rst::rasterizer r(700, 700); // �����դ������

    // ��¼������դ��
    auto texture_path = "hmap.jpg";
    r.set_texture(Texture(obj_path + texture_path));

    std::function<Eigen::Vector3f(fragment_shader_payload)> active_shader = phong_fragment_shader;


    // ���봦�� �ò�������shader��function�������������
    if (argc >= 2)
    {
        command_line = true;
        filename = std::string(argv[1]);

        if (argc == 3 && std::string(argv[2]) == "texture")
        {
            std::cout << "Rasterizing using the texture shader\n";
            active_shader = texture_fragment_shader;
            texture_path = "spot_texture.png";
            r.set_texture(Texture(obj_path + texture_path));
        }
        else if (argc == 3 && std::string(argv[2]) == "normal")
        {
            std::cout << "Rasterizing using the normal shader\n";
            active_shader = normal_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "phong")
        {
            std::cout << "Rasterizing using the phong shader\n";
            active_shader = phong_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "bump")
        {
            std::cout << "Rasterizing using the bump shader\n";
            active_shader = bump_fragment_shader;
        }
        else if (argc == 3 && std::string(argv[2]) == "displacement")
        {
            std::cout << "Rasterizing using the displacement shader\n";
            active_shader = displacement_fragment_shader;
        }
    }

    Eigen::Vector3f eye_pos = {0,0,10};

    r.set_vertex_shader(vertex_shader); // ���ö�����ɫ��ʽ
    r.set_fragment_shader(active_shader); // ����ƬԪ��ɫ��ʽ

    int key = 0;
    int frame_count = 0;

    if (command_line)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth); // ��ջ�����
        // �ֱ�õ�MVP�任����
        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        // Ӧ�ñ任���� �����й�դ����ƬԪ����֡����
        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imwrite(filename, image);

        return 0;
    }

    while(key != 27)
    {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45.0, 1, 0.1, 50));

        //r.draw(pos_id, ind_id, col_id, rst::Primitive::Triangle);
        r.draw(TriangleList);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        cv::imshow("image", image);
        cv::imwrite(filename, image);
        key = cv::waitKey(10);

        if (key == 'a' )
        {
            angle -= 0.1;
        }
        else if (key == 'd')
        {
            angle += 0.1;
        }

    }
    return 0;
}
