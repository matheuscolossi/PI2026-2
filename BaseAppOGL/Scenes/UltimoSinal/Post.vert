#version 330 core
// One oversized triangle covers the target; no vertex buffer is required.
out vec2 uv;
void main(){
    vec2 corner=vec2(float((gl_VertexID<<1)&2),float(gl_VertexID&2));
    uv=corner;
    gl_Position=vec4(corner*2.0-1.0,0.0,1.0);
}
