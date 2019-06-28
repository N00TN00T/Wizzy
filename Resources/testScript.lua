color = new(Color)
color.r = 0.2
color.g = 0.1
color.b = 0.7
color.a = 0.25

vec2 = new(Vec2)
vec2.x = 2
vec2.y = 8

vec2_1 = new(Vec2)
vec2_1.x = 5
vec2_1.y = 3

vec3 = new(Vec3)
vec3.x = 7
vec3.y = 2
vec3.z = 9

vec4 = new(Vec4)
vec4.x = 10
vec4.y = 1
vec4.z = 54
vec4.w = 23

print ( "Color: r:" .. color.r .. ", g:" .. color.g .. ", b:" .. color.b .. ", a:" .. color.a )
print ( "Vec2: x:" .. vec2.x .. ", y:" .. vec2.y )
print ( "Vec2_1: x:" .. vec2_1.x .. ", y:" .. vec2_1.y )
print ( "Vec3: x:" .. vec3.x .. ", y:" .. vec3.y .. ", z:" .. vec3.z )
print ( "Vec4: x:" .. vec4.x .. ", y:" .. vec4.y .. ", z:" .. vec4.z .. ", w:" .. vec4.w )

targetColor = new(Color)
targetColor.r = 1; targetColor.g = 1; targetColor.b = 1; targetColor.a = 1;
-- color = color:Lerp(targetColor, 1.0)
-- color = color.Lerp(color, targetColor, 1.0)


--print ( "Color: r:" .. color.r .. ", g:" .. color.g .. ", b:" .. color.b .. ", a:" .. color.a )
