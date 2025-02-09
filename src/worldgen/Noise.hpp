#pragma once
#ifndef CGPROJECT_NOISE_HPP
#define CGPROJECT_NOISE_HPP

//https://www.shadertoy.com/view/4djSRW
//Generates a pseudorandom number between 0 and 1 based on the input values
inline float hash2to1(vec2 p)
{
    vec3 p3  = fract(vec3(p.x,p.y,p.x) * 0.1031f);
    p3 += dot(p3, vec3(p3.y,p3.z,p3.y) + 33.33f);
    return fract((p3.x + p3.y) * p3.z);
}

//https://www.shadertoy.com/view/4djSRW
//Generates two pseudorandom number between 0 and 1 based on the input values
inline vec2 hash2to2(vec2 p)
{
    vec3 p3 = fract(vec3(p.x,p.y,p.x) * vec3(.1031, .1030, .0973));
    p3 += dot(p3, vec3(p3.y,p3.z,p3.y)+33.33f);
    return fract((vec2(p3.x,p3.x)+vec2(p3.y,p3.z))*vec2(p3.z,p3.y));

}


//https://github.com/Rudraksha20/CIS565-GPU-Final-Project-Vulkan-Procedural-Terrain
inline float noise(vec2 p){
    vec2 ip = floor(p);
    vec2 u = fract(p);
    u = u*u*(3.0f-(2.0f*u));

    float res = mix(
            mix(hash2to1(ip), hash2to1(ip + vec2(1.0, 0.0)), u.x),
            mix(hash2to1(ip + vec2(0.0, 1.0)), hash2to1(ip + vec2(1.0, 1.0)), u.x), u.y);
    return res*res;
}

// http://flafla2.github.io/2014/08/09/perlinnoise.html
inline float smoothNoise(vec2 p){
    float total = 0.0;
    float ampl = 100;
    float freq = 1/ampl;
    float maxVal = 0.0;
    for (int i = 0; i < 4; i++) {
        total += noise(p * freq) * ampl;
        maxVal += ampl;
        ampl *= 0.5;
        freq *= 2.0;

    }
    return total / maxVal;
}

#endif //CGPROJECT_NOISE_HPP
