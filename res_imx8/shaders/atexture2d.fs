varying vec2 Textcoord;
uniform sampler2D text;
void main()
{
   gl_FragColor = texture(text, Textcoord);
}
