#version 330

in vec3 v_tex;
out vec4 fragColor;

uniform float u_opacity;
uniform samplerCube skybox;
//uniform sampler2D skybox;

void main(){
	vec3 tex = texture(skybox, v_tex).xyz;
    fragColor = vec4(tex, u_opacity);
}
