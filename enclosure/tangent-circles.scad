include <vecutil.scad>;

// Function to calculate the intersection points of two circles
function circle_circle_intersection(x1, y1, r1, x2, y2, r2) = let (
    dx = x2 - x1,
    dy = y2 - y1,
    d = sqrt(dx*dx + dy*dy),
    sum_r = r1 + r2,
    diff_r = abs(r1 - r2),
    epsilon = 1e-9  // Small value to handle floating-point errors
)
(d > sum_r + epsilon || d < diff_r - epsilon)  ? undef  :  // No intersection    
(abs(d) < epsilon && abs(r1 - r2) < epsilon) ? undef  :  // Circles are coincident (infinite intersections)
let (
    a = (r1*r1 - r2*r2 + d*d) / (2*d),
    h_sq = r1*r1 - a*a,
    h = (h_sq >= 0) ? sqrt(h_sq) : 0,
    x3 = x1 + a * dx / d,
    y3 = y1 + a * dy / d,
    rx = -dy * (h / d),
    ry = dx * (h / d),
    intersection1 = [x3 + rx, y3 + ry],
    intersection2 = [x3 - rx, y3 - ry]
)
(abs(h) < epsilon) ?
    [intersection1]  // Circles are tangent (one intersection point)
:
    [intersection1, intersection2];  // Two intersection points

module tanShape(pC1, pC2, d1, d2, d3, intersectionIndex, inner=false){    
    if (inner){
        intersectionPoints = circle_circle_intersection(pC1[0], pC1[1], d3/2 - d1/2, pC2[0], pC2[1], d3/2 - d2/2);        
        ip = intersectionPoints[intersectionIndex];        
        ip1 = vec2Sum(vec2Scale(vec2Normalize(vec2Diff(ip, pC1)),d3), pC1);
        ip2 = vec2Sum(vec2Scale(vec2Normalize(vec2Diff(ip, pC2)),d3), pC2);
        intersection(){
            polygon([pC2, pC1, ip1,   ip2]);
            translate(ip)
                circle(d = d3);
        }
    } else {
        intersectionPoints = circle_circle_intersection(pC1[0], pC1[1], d1/2 + d3/2, pC2[0], pC2[1], d2/2 + d3/2);
        ip = intersectionPoints[intersectionIndex];
        difference(){
            polygon([pC2, ip, pC1]);
            translate(ip)
                circle(d = d3);
        }
    }
}
