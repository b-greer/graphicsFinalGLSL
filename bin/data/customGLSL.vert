#version 430
#define MAX_LIGHTS 1

in vec2 texcoord;
in vec4 position;
in vec4 color;
in vec4 normal;

//uniforms from the oF
uniform mat3 normalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelViewProjectionMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;

//float time is updated from oF
uniform float time; 

//the outputs
out float v_terrainHeight;
out vec2 v_texcoord;
out vec3 v_viewSpaceNormal;
out vec3 v_viewSpacePosition;


// a borrowed random function 
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}


// Based on Morgan McGuire @morgan3d (book of shaders)
// https://www.shadertoy.com/view/4dS3Wd

float perlin(in vec2 st) {
//perlin noise is basically creating a 2-D grid of noisy values 
//made by smoothstepping between random values (using the borrowed random above)
//and then mixing the points of the tile we are using 

//separate the int and dec 
	vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
// i, i + (1,0) are top two cornerse 
//like cartesian coords 
    float a = random(i);
    float b = random(i + vec2(2.0, 0.0));
    float c = random(i + vec2(0.0, 2.0));
    float d = random(i + vec2(2.0, 2.0));

// u is just a smooth interpolation line between two points 
    vec2 u = smoothstep(0.,3.,f);

//then we mix the 4 corner values based on the smooth curve we just made 
//which weights the percentages of each:)
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}


//
// Description : GLSL 2D simplex noise function
//      Author : Ian McEwan, Ashima Arts
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License :
//  Copyright (C) 2011 Ashima Arts. All rights reserved.
//  Distributed under the MIT License. See LICENSE file.
//  https://github.com/ashima/webgl-noise
//
// Some useful math functions
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float simplex(in vec2 v) {

    // Precompute values for skewed triangular grid
    const vec4 C = vec4(0.211324865405187,
                        // (3.0-sqrt(3.0))/6.0
                        0.366025403784439,
                        // 0.5*(sqrt(3.0)-1.0)
                        -0.577350269189626,
                        // -1.0 + 2.0 * C.x
                        0.024390243902439);
                        // 1.0 / 41.0

    // First corner (x0)
    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    // Other two corners (x1, x2)
    vec2 i1 = vec2(0.0);
    i1 = (x0.x > x0.y)? vec2(1.0, 0.0):vec2(0.0, 1.0);
    vec2 x1 = x0.xy + C.xx - i1;
    vec2 x2 = x0.xy + C.zz;

    // Do some permutations to avoid
    // truncation effects in permutation
    i = mod289(i);
    vec3 p = permute(
            permute( i.y + vec3(0.0, i1.y, 1.0))
                + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(
                        dot(x0,x0),
                        dot(x1,x1),
                        dot(x2,x2)
                        ), 0.0);

    m = m*m ;
    m = m*m ;

    // Gradients:
    //  41 pts uniformly over a line, mapped onto a diamond
    //  The ring size 17*17 = 289 is close to a multiple
    //      of 41 (41*7 = 287)

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    // Normalise gradients implicitly by scaling m
    // Approximation of: m *= inversesqrt(a0*a0 + h*h);
    m *= 1.79284291400159 - 0.85373472095314 * (a0*a0+h*h);

    // Compute final noise value at P
    vec3 g = vec3(0.0);
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * vec2(x1.x,x2.x) + h.yz * vec2(x1.y,x2.y);
    return 130.0 * dot(m, g);
}


float fbm (in vec2 st) {
    // Initial values
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    
    // Loop of octaves
	for (int i = 0; i < 5; i++) {
		value += -amplitude * simplex(st);
		st *= 2.;
		amplitude *= .5;
	}

    return value;
}

void main (void){

//normals!

//this wiggles/animates the position by offsetting the 
//y position by a fluctuating sin (controlled by time)
	vec4 pos = position;

//first y offset according to Z sets the front/back wave 
	float r = pos.z - (time);
	pos.y += (sin(r) * 0.3);

//this distorts y slightly using fbm (fractal brownian motion) 
	pos = pos + vec4(0.0, -0.75*fbm(0.05 * pos.xz), 0.0,0.0);
	
// this y offset distorts based on x position (the left to right wave) 
	float u = pos.x - ( time);
	pos.y = pos.y + (sin(u) * 0.45);

// tex coord is an input so assigned to an output
//because i need it in the frag
	v_texcoord = texcoord;
//make a new normal for the reflection
	mat4 normalMatrix =  transpose(inverse(modelViewMatrix));
    v_viewSpaceNormal = normalize((normalMatrix * normal).xyz);
//accounts for the angles
	v_viewSpacePosition = (modelViewMatrix * position).xyz;
//puts the textureMatrix to scale the tex coordinates
    v_texcoord = (textureMatrix*vec4(texcoord.x,texcoord.y,0,1)).xy;

//final output is the projection view multiplied by the pixel
//lets us have 3D that seems more constant
 gl_Position = modelViewProjectionMatrix * pos;

}
