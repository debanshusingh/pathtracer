#version 150

uniform mat4 u_Model;
uniform mat4 u_ModelInvTr;
uniform mat4 u_ViewProj;
uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform vec3 u_CamPos;

in vec3 vs_Position;
in vec3 vs_Normal;
in vec3 vs_Color;

out vec4 fs_Normal;
out vec4 fs_LightVector;
out vec4 fs_Color;
out vec4 fs_LightColor;
out vec4 fs_ViewVector;

void main()
{
    fs_Color = vec4(vs_Color, 1);
    fs_LightColor = vec4(u_LightColor,1);
    fs_Normal = u_ModelInvTr * vec4(vs_Normal, 0);

    // Set up our vector for the light
    fs_LightVector = vec4(u_LightPos,1) - u_Model * vec4(vs_Position, 1);
    fs_ViewVector = vec4(u_CamPos,1) - u_Model * vec4(vs_Position, 1);

    //built-in things to pass down the pipeline
    gl_Position = u_ViewProj * u_Model * vec4(vs_Position, 1);
}
