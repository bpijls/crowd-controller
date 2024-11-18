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
