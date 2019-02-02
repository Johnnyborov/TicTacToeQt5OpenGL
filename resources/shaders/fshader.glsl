#version 330 core
in highp vec2 v_texcoord;
out highp vec4 fragColor;
uniform sampler2D texture0;

void main() {
  fragColor = texture2D(texture0, v_texcoord);
}
