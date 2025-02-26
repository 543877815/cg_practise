//
// Created by LEI XU on 5/16/19.
//

#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "Vector.hpp"

enum MaterialType { DIFFUSE, MICROFACET};

class Material{
private:

    float eps = 1e-6;
    // Compute reflection direction
    Vector3f reflect(const Vector3f &I, const Vector3f &N) const
    {
        return I - 2 * dotProduct(I, N) * N;
    }

    // Compute refraction direction using Snell's law
    //
    // We need to handle with care the two possible situations:
    //
    //    - When the ray is inside the object
    //
    //    - When the ray is outside.
    //
    // If the ray is outside, you need to make cosi positive cosi = -N.I
    //
    // If the ray is inside, you need to invert the refractive indices and negate the normal N
    Vector3f refract(const Vector3f &I, const Vector3f &N, const float &ior) const
    {
        float cosi = clamp(-1, 1, dotProduct(I, N));
        float etai = 1, etat = ior;
        Vector3f n = N;
        if (cosi < 0) { cosi = -cosi; } else { std::swap(etai, etat); n= -N; }
        float eta = etai / etat;
        float k = 1 - eta * eta * (1 - cosi * cosi);
        return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
    }

    // Compute Fresnel equation
    //
    // \param I is the incident view direction
    //
    // \param N is the normal at the intersection point
    //
    // \param ior is the material refractive index
    //
    // \param[out] kr is the amount of light reflected
    void fresnel(const Vector3f &I, const Vector3f &N, const float &ior, float &kr) const
    {
        float cosi = clamp(-1, 1, dotProduct(I, N));
        float etai = 1, etat = ior;
        if (cosi > 0) {  std::swap(etai, etat); }
        // Compute sini using Snell's law
        float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
        // Total internal reflection
        if (sint >= 1) {
            kr = 1;
        }
        else {
            float cost = sqrtf(std::max(0.f, 1 - sint * sint));
            cosi = fabsf(cosi);
            float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
            float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
            kr = (Rs * Rs + Rp * Rp) / 2;
        }
        // As a consequence of the conservation of energy, transmittance is given by:
        // kt = 1 - kr;
    }

    Vector3f toWorld(const Vector3f &a, const Vector3f &N){
        Vector3f B, C;
        if (std::fabs(N.x) > std::fabs(N.y)){
            float invLen = 1.0f / std::sqrt(N.x * N.x + N.z * N.z);
            C = Vector3f(N.z * invLen, 0.0f, -N.x *invLen);
        }
        else {
            float invLen = 1.0f / std::sqrt(N.y * N.y + N.z * N.z);
            C = Vector3f(0.0f, N.z * invLen, -N.y *invLen);
        }
        B = crossProduct(C, N);
        return a.x * B + a.y * C + a.z * N;
    }

public:
    MaterialType m_type;
    //Vector3f m_color;
    Vector3f m_emission;
    float ior;
    Vector3f Kd, Ks;
    float specularExponent;
    //Texture tex;

    inline Material(MaterialType t=DIFFUSE, Vector3f e=Vector3f(0,0,0));
    inline MaterialType getType();
    //inline Vector3f getColor();
    inline Vector3f getColorAt(double u, double v);
    inline Vector3f getEmission();
    inline bool hasEmission();

    // sample a ray by Material properties
    inline Vector3f sample(const Vector3f &wi, const Vector3f &N);
    // given a ray, calculate the PdF of this ray
    inline float pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);
    // given a ray, calculate the contribution of this ray
    inline Vector3f eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N);

    // Microfacet
    inline float DistributionGGX(Vector3f n, Vector3f h, float roughness);
    inline float geometrySchlickGGX(float nv, float k);
    inline float geometrySmith(Vector3f n, Vector3f v, Vector3f l, float roughness);
    inline float distributionGGX(Vector3f n, Vector3f h, float roughness);
};

Material::Material(MaterialType t, Vector3f e){
    m_type = t;
    //m_color = c;
    m_emission = e;
}

MaterialType Material::getType(){return m_type;}
///Vector3f Material::getColor(){return m_color;}
Vector3f Material::getEmission() {return m_emission;}
bool Material::hasEmission() {
    if (m_emission.norm() > EPSILON) return true;
    else return false;
}

Vector3f Material::getColorAt(double u, double v) {
    return Vector3f();
}


Vector3f Material::sample(const Vector3f &wi, const Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        case MICROFACET:
        {
            // uniform sample on the hemisphere
            float x_1 = get_random_float(), x_2 = get_random_float();
            float z = std::fabs(1.0f - 2.0f * x_1);
            float r = std::sqrt(1.0f - z * z), phi = 2 * M_PI * x_2;
            Vector3f localRay(r*std::cos(phi), r*std::sin(phi), z);
            return toWorld(localRay, N);
            
            break;
        }
    }
}


float Material::pdf(const Vector3f &wi, const Vector3f &wo, const Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        case MICROFACET:
        {
            // uniform sample probability 1 / (2 * PI)
            if (dotProduct(wo, N) > 0.0f)
                return 0.5f / M_PI;
            else
                return 0.0f;
            break;
        }
    }
}

Vector3f Material::eval(const Vector3f &wi, const Vector3f &wo, const Vector3f &N){
    switch(m_type){
        case DIFFUSE:
        {
            // calculate the contribution of diffuse   model
            float cosalpha = dotProduct(N, wo);
            if (cosalpha > 0.0f) {
                Vector3f diffuse = Kd / M_PI;
                return diffuse;
            }
            else
                return Vector3f(0.0f);
            break;
        }
        case MICROFACET:
        {
            float cosa = dotProduct(N, wo);
            if (cosa > 0.0f)
            {
                float roughness = 0.4f;
                Vector3f v = -wi, l = wo, h = normalize(v + l), n = N;
                float D = DistributionGGX(n, h, roughness);
                float G = geometrySmith(n, v, l, roughness);
                float F;
                float etat = 1.85;// 介质折射率
                fresnel(wi, n, etat, F);

                Vector3f div1 = D * G * F;
                float div2 = 4 * std::max(dotProduct(n, v), 0.0f) * std::max(dotProduct(n, l), 0.0f);
                Vector3f fr = div1 / std::max(div2, eps);// 防止div0

                // 能量守恒
                float ks_ = F;// 反射比率
                float kd_ = 1.0f - ks_;// 折射比率
                Vector3f diffuse = 1.0f / M_PI;
                // 因为在 specular 项里已经考虑了反射部分的比例：F。所以反射部分不需要再乘以 ks_ 
                // Ks为镜面反射项，Kd为漫反射项。
                return Ks * fr + kd_ * Kd * diffuse;
            }
            else
                return Vector3f(0.0f);
            break;
        }
    }
}


// 法线分布函数D
float Material::DistributionGGX(Vector3f n, Vector3f h, float roughness)
{
    float a = roughness * roughness, a2 = a * a;
    float nh = std::max(dotProduct(n, h), 0.0f), nh2 = nh * nh;
    float div = (nh2 * (a2 - 1.0) + 1.0);
    div = M_PI * div * div;
    return a2 / std::max(div, eps);// 防止分母为0
}

// 几何分布函数G
float Material::geometrySchlickGGX(float nv, float k)
{
    float div = nv * (1.0 - k) + k;
    return nv / div;
}

// 几何分布函数G
float Material::geometrySmith(Vector3f n, Vector3f v, Vector3f l, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    float nv = std::max(dotProduct(n, v), 0.0f);
    float nl = std::max(dotProduct(n, l), 0.0f);
    float ggx2 = geometrySchlickGGX(nv, k);
    float ggx1 = geometrySchlickGGX(nl, k);
    return ggx1 * ggx2;
}

// 法线分布函数D
float Material::distributionGGX(Vector3f n, Vector3f h, float roughness)
{
    float a = roughness * roughness, a2 = a * a;
    float nh = std::max(dotProduct(n, h), 0.0f), nh2 = nh * nh;
    float div = (nh2 * (a2 - 1.0) + 1.0);
    div = M_PI * div * div;
    return a2 / std::max(div, eps);// 防止分母为0
}

#endif //RAYTRACING_MATERIAL_H
