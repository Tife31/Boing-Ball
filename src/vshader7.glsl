#version 150

in vec4 vPosition;

uniform int vaoIndex, wallColor; 
uniform mat4 ModelView, Projection;

flat out vec4 f_colour;

void main()
{
  int sectorNum = 36;
  vec4  v = vPosition;
  int row = gl_VertexID /sectorNum;
  int col = gl_VertexID % sectorNum;
  
  if (vaoIndex == 0)  // The wall
  {
    
     if(wallColor == 1) //the quad 
     {
       f_colour = vec4(0.0, 0.0, 0.5, 1.0);
     }
     else if(wallColor == 2) // then it is the grid. 
     {
       f_colour = vec4(0.8, 0.8, 0.8, 1.0);
     }

  } 
  else if(vaoIndex == 1)// the shadow
  {
    f_colour = vec4(0.2, 0.2, 0.2, 1); //dark grey

  }
  else if(vaoIndex == 2) 
  {
  
    if (row % 2 == col % 2) {
      f_colour = vec4(1,0,0,1);
    } 
    else 
    {
      f_colour = vec4(0,0,1,1);
    }

  }
  else if(vaoIndex == 3)
  {

    f_colour = vec4(0.5, 0, 1, 1);

  }

  gl_Position = Projection * ModelView * v;
}

