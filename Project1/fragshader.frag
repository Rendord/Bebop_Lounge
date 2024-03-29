#version 430 core

// Input from vertex shader
in VS_OUT
{
    vec3 N;
    vec3 L;
    vec3 V;
} fs_in;

in vec2 UV;

layout(location = 0) out vec4 calculated_color;

// Material properties
uniform vec3 mat_ambient;
uniform vec3 mat_diffuse;
uniform float mat_power;
uniform vec3 mat_specular;

uniform sampler2D texsampler;


void main()
{
    // Normalize the incoming N, L and V vectors
    vec3 N = normalize(fs_in.N);
    vec3 L = normalize(fs_in.L);
    vec3 V = normalize(fs_in.V);

    // Calculate R locally
    vec3 R = reflect(-L, N);

    //calculate light direction

	
    // Compute the diffuse and specular components for each fragment
    vec3 diffuseoverlay = max(dot(N, L), 0.0) * mat_diffuse;

	//texture texture texture
	vec3 diffuse = max(dot(N, L), 0.0) * texture2D(texsampler, UV).rgb * mat_diffuse;
    vec3 ambient = texture2D(texsampler, UV).rgb * mat_ambient;


    vec3 specular = pow(max(dot(R, V), 0.0), mat_power) * texture2D(texsampler, UV).rgb * mat_specular;



    calculated_color = vec4(ambient + diffuse + specular, 1.0);


    // Write final color to the framebuffer
    //gl_FragColor = calculated_color;
     



}
