//把顶点坐标传给这个变量，确定要画的形状
attribute vec4 vPosition;
//传给片元着色器 像素点
attribute vec4 vCoord;
//摄像机矩阵
uniform mat4 vMatrix;
//  x   y   z   向量
// x  y
//传给片元着色器 像素点   0 0    1 0
varying vec2 aCoord;
void main(){

    gl_Position = vPosition;
    aCoord = (vMatrix*vCoord).xy;
}