#version 330
in vec3 position;

#pragma include "global_uniforms_incl.glsl"
// defines the following uniforms
// uniform mat4 g_view;
// uniform mat4 g_projection;
// uniform vec4 g_viewport;
// uniform vec4 g_cameraPos;
// uniform vec4 g_ambientLight;
// uniform vec4 g_lightColorRange[4];
// uniform vec4 g_lightPosType[4];
// uniform mat4 g_model;
// uniform mat3 g_model_it;
// uniform mat3 g_model_view_it;

void main(void) {
    vec4 wsPos = g_model * vec4(position,1.0);
    gl_Position = g_projection * g_view * wsPos;

}