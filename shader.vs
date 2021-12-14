#version 410

in vec3 Position;
in vec3 normID;
out vec3 Color;
uniform mat4 gWorld;
uniform int curr;

out vec3 Posi;

out vec3 Normal;
out vec3 FragPos;


float colormap_red(float x) {
    return 4.04377880184332E+00 * x - 5.17956989247312E+02;
}

float colormap_green(float x) {
    if (x < (5.14022177419355E+02 + 1.13519230769231E+01) / (4.20313644688645E+00 + 4.04233870967742E+00)) {
        return 4.20313644688645E+00 * x - 1.13519230769231E+01;
    } else {
        return -4.04233870967742E+00 * x + 5.14022177419355E+02;
    }
}

float colormap_blue(float x) {
    if (x < 1.34071303331385E+01 / (4.25125657510228E+00 - 1.0)) { // 4.12367649967
        return x;
    } else if (x < (255.0 + 1.34071303331385E+01) / 4.25125657510228E+00) { // 63.1359518278
        return 4.25125657510228E+00 * x - 1.34071303331385E+01;
    } else if (x < (1.04455240613432E+03 - 255.0) / 4.11010047593866E+00) { // 192.100512082
        return 255.0;
    } else {
        return -4.11010047593866E+00 * x + 1.04455240613432E+03;
    }
}

vec3 colormap(float x) {
    float t = x * 255.0;
    float r = clamp(colormap_red(t) / 255.0, 0.0, 1.0);
    float g = clamp(colormap_green(t) / 255.0, 0.0, 1.0);
    float b = clamp(colormap_blue(t) / 255.0, 0.0, 1.0);
    return vec3(r, g, b);
}



void main()
{
    
	
        gl_Position = gWorld * vec4( Position, 1.0);
        //currPos=vec3(gWorld * vec4( Position, 1.0));
         FragPos = vec3(gWorld * vec4(Position, 1.0));
    
	Color = colormap(1-(1+gl_Position[0])/2);
	//Color = vec4(gl_Position[2]/2+0.5,gl_Position[2]/2+0.5,gl_Position[2]/2+0.5,1.0) ;
    Posi=vec3(1.0,0.0,0.0);
    Normal=normID;
}
