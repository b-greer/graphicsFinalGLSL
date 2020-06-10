#version 430
#define PI 3.1415926538
//pushes out a color basically 
out vec4 fragColor;

//from the vertex shader
in vec2 v_texcoord;
in vec3 v_viewSpaceNormal;
in vec3 v_viewSpacePosition;
in vec3 ReflectDir;

//window size passed in
uniform vec2 size;

//light properties
uniform vec4 lightPosition;
uniform vec4 lightColor;

//material properties
uniform float matShininess;
uniform float matTransparency;
uniform float matReflectivity; 
uniform vec4 matAmbient;
uniform vec4 matDiffuse;
uniform vec4 matSpecular;
uniform vec4 MaterialColor;

//time updated in oF and pushed in 
uniform float time;

// These are passed in from OF programmable renderer
//to calculate reflection from the pic (phong!)
uniform mat4 modelViewMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 textureMatrix;
uniform mat4 modelViewProjectionMatrix;

//image textures to edit
//2 because i want to map multiple layers later 
uniform sampler2D environmentMap;
uniform sampler2D imageTexture;

//return a random number
float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

//again inspired by Morgan McGuire, 
//using tiles to get noise from the above random
float noise (in vec2 _st) {
    vec2 i = floor(_st);
    vec2 f = fract(_st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

#define NUM_OCTAVES 9
//again with fbm
float fbm ( in vec2 _st) {
//velocity & amplitude
    float v = 0.0;
    float a = 0.5;
	//offset
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5),
                    -sin(0.5), cos(0.50));

    for (int i = 0; i < NUM_OCTAVES; ++i) {
	//moves it in a direction 
	//similar to flow fields (like i did in gen drawing)
        v += a * noise(_st);
        _st = rot * _st * 2.0 + shift;
        a *= 0.5;
    }
    return v;
}

float ridge(float h, float offset) {
    h = abs(h);     // create creases
    h = offset - h; // invert so creases are at top
    h = h * h;      // sharpen creases
    return h;
}

//this uses the veins and brightness 
float ridgedMF(vec2 p) 
{
    float veins = 2.0;
    float brightness =  -0.3;

    float sum = 0.0;
    float freq = 1.0, a = 0.5;
    float prev = 1.0;

    for(int i=0; i < 10; i++) {
	//finds the noise of the location
        float n = abs(noise(p*freq));
	//up the sum
        sum += n*a;
        sum += n*a*prev; 
	//save previous octave
        prev = n;
	//multiply the frequency into the veins 
	//over many octaves this "carves" a path since they are all
	//in similar locations
        freq *= veins;
	//multiply the amplitude by brightness 
	//this makes the colour
        a *= brightness;
    }
    return sum;

}



vec4 calcReflection(in vec3 normal, in vec3 surfacePosition) {
// Compute direction from eye to the surface position
// Note: eye is at (0,0,0) in view space
    vec3 eyeDir = normalize(surfacePosition);

// Compute reflection direction
    vec3 reflDir = reflect(eyeDir, normal);

// Calculate matrix to convert from view space to world space
    mat4 viewToWorldMatrix = inverse(viewMatrix);

// Convert refl dir from view space to world space
    vec4 temp = viewToWorldMatrix * vec4(reflDir, 0);
    vec3 worldSpaceReflDir = normalize(temp.xyz);

// Calculate direction of reflection in polar coordinates
    float theta = atan(worldSpaceReflDir.z, worldSpaceReflDir.x);
    float phi = asin(worldSpaceReflDir.y);

// Use polar coordinates to calculate the texture co-ordinates to lookup from the reflection map
    vec2 envLookupCoords = vec2(0.5 * theta / PI + 0.5, 0.5 - phi / PI);

// Lookup the color from the reflection map
    return texture(environmentMap, envLookupCoords);
}

void main (void) {
//start with black colors
	vec3 ambient = vec3(0,0,0);
    vec3 diffuse = vec3(0,0,0);
    vec3 specular = vec3(0,0,0);

//define pixelPos (aka the pixel we're messin with)
    vec2 pixelPos = gl_FragCoord.xy;


//st is a place holder to define the "size" of noise (the interval)
//if its bigger then the noise is much more densely packed
//since we are sampling points along a wave
	vec2 st = gl_FragCoord.xy/size.xy*3.;

	vec2 stime = st; 
//popping time into this animates the dark patches
	stime.x =  fbm(st + vec2(1.7,9.2)+ 0.15*time);
	stime.y = fbm( st + vec2(8.3,2.8)+ 0.126*time);

//get normals
	vec3 normal = normalize(v_viewSpaceNormal);
//start with black here 
	vec3 color = vec3(0.0);

//this mixes up q to another layer of fbm
    vec2 q = vec2(0.);
    q.x = fbm( st + 0.00 * time);
    q.y = fbm( st + vec2(1.0));

	vec2 r = vec2(0.);
    r.x = fbm( st + 1.0*q + vec2(1.7,9.2)+ 0.15*time );
    r.y = fbm( st + 1.0*q + vec2(8.3,2.8)+ 0.126*time);

    float f = fbm(st+r);
//midtones
	color = mix(vec3(0.477,0.608,0.667),
                vec3(0.356,0.549,0.667),
                clamp((f*f)*4.0,0.0,1.0));
//darkest 
    color = mix(color,
                vec3(0.085,0.094,0.165),
                clamp(length(q),0.0,1.0));
//highlights
    color = mix(color,
                vec3(0.773,0.950,0.904),
                clamp(length(r.x),0.0,1.0));

//final mix of color!
	vec4 finalColor = vec4((f*f*f+.6*f*f+.5*f)*color,1.);
//this is the big where the webcam comes in
    vec4 textureColor = texture(imageTexture, pixelPos) ;
//initial frag color
	fragColor = textureColor * matAmbient;
//reflections
	vec4 reflectionColor = calcReflection(normal, v_viewSpacePosition);
 //final output 
   fragColor = (finalColor  + vec4(reflectionColor) * 0.5 + vec4(specular,1.0)) - (0.2*(ridgedMF(stime *- 8.0)));
    
}