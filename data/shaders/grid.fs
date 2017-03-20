#version 330 core

in vec3 pos;

out vec4 color;

void main()
{
	//Y
	if(pos.y != 0.0)
	{
		color = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	//X (900,0,0) - (-900,0,0)
	else if(pos.x !=0 && pos.z == 0)
	{
		color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	//Z (0,0,900)-(0,0,-900)
	else if(pos.x == 0 && pos.z != 0)
	{
		color = vec4(0.0f,0.0f,1.0f,1.0f);
	}
	else
	{
		color = vec4(0.1f,0.1f,0.1f,1.0f);
	}
	
}
