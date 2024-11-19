// Description: Utility functions for 2D vectors
function vec2Length(v) = sqrt(v[0]*v[0] + v[1]*v[1]);
function vec2Diff(v1, v2) = [v2[0] - v1[0], v2[1] - v1[1]];
function vec2Sum(v1, v2) = [v2[0] + v1[0], v2[1] + v1[1]];
function vec2Scale(v, s) = [v[0] * s, v[1] * s];
function vec2Dot(v1, v2) = v1[0] * v2[0] + v1[1] * v2[1];
function vec2Cross(v1, v2) = v1[0] * v2[1] - v1[1] * v2[0];
function vec2Angle(v1, v2) = acos(vec2Dot(v1, v2) / (vec2Length(v1) * vec2Length(v2)));
function vec2Rotate(v, angle) = [v[0] * cos(angle) - v[1] * sin(angle), v[0] * sin(angle) + v[1] * cos(angle)];
function vec2Normal(v) = [v[1], -v[0]];
function vec2Distance(p1, p2) = vec2Length([p2[0] - p1[0], p2[1] - p1[1]]);
function vec2Normalize(p) = let (l = vec2Length(p)) [p[0] / l, p[1] / l];


// 3d vector functions
function vec3Length(v) = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
function vec3Diff(v1, v2) = [v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2]];
function vec3Sum(v1, v2) = [v2[0] + v1[0], v2[1] + v1[1], v2[2] + v1[2]];
function vec3Scale(v, s) = [v[0] * s, v[1] * s, v[2] * s];
function vec3Dot(v1, v2) = v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
function vec3Cross(v1, v2) = [v1[1] * v2[2] - v1[2] * v2[1], v1[2] * v2[0] - v1[0] * v2[2], v1[0] * v2[1] - v1[1] * v2[0]];
function vec3Angle(v1, v2) = acos(vec3Dot(v1, v2) / (vec3Length(v1) * vec3Length(v2)));
function vec3Rotate(v, angle, axis) = let (c = cos(angle), s = sin(angle), t = 1 - c, x = axis[0], y = axis[1], z = axis[2], x2 = x*x, y2 = y*y, z2 = z*z, xy = x*y, xz = x*z, yz = y*z, xs = x*s, ys = y*s, zs = z*s) [ (t*x2 + c) * v[0] + (t*xy - zs) * v[1] + (t*xz + ys) * v[2], (t*xy + zs) * v[0] + (t*y2 + c) * v[1] + (t*yz - xs) * v[2], (t*xz - ys) * v[0] + (t*yz + xs) * v[1] + (t*z2 + c) * v[2]];
function vec3Normal(v) = let (l = vec3Length(v)) [v[0] / l, v[1] / l, v[2] / l];
function vec3Distance(p1, p2) = vec3Length([p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2]]);
function vec3Normalize(p) = let (l = vec3Length(p)) [p[0] / l, p[1] / l, p[2] / l];

