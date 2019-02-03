#version 330 core
in highp vec2 v_texcoord;
out highp vec4 fragColor;

uniform bool is_green;
uniform sampler2D texture0;
uniform sampler2D texture1;

void main() {
  vec4 color0 = texture2D(texture0, v_texcoord);
  vec4 color1 = texture2D(texture1, v_texcoord);

  float factor = 0.5;
  if (color1.x > 0.5 && color1.y > 0.25 && color1.z > 0.25) // ignore white space
    factor = 0;

  fragColor = mix(color0, color1, factor);
  if (is_green)
    fragColor = mix(fragColor, vec4(0.0, 1.0, 0.0, 1.0), 0.2);
}
