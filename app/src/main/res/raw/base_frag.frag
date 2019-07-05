//数据的精度
precision mediump float;
//采样点的坐标
varying vec2 aCoord;
uniform sampler2D vTexture;

void main(){
    //变量 接收像素值
    //texture2D：采样器 采集aCoord的像素
    gl_FragColor = texture2D(vTexture,aCoord);
}