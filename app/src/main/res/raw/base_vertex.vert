//把顶点坐标传给这个变量，确定要画的形状
attribute vec4 vPosition;
//传给片元着色器 像素点
attribute vec4 vCoord;

void main(){

    gl_Position = vPosition;
    aCoord = vCoord;
}