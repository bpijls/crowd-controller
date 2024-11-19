include <tangent-circles.scad>;
$fn = 100;
dialLocation = [ 0, 70];
pushButtonLocation = [0,0];

dialAreaDiameter = 75;
pushButtonAreaDiameter = 55;

pushButtonWidth = 13;
pushButtonDepth = 12.1;

dialDiameter = 6.7;

largeRingInnerDiameter = 31;
largeRingOuterDiameter = 49;

smallRingInnerDiameter = 11.5;
smallRingOuterDiameter = 28;

baseThickness = 2;
ringSpacerThickness = 2;
ringSpacerHeight = 2;

bottomCurveDiameter = 200;
topCurveDiameter = 100;

// draw the base plate
translate([0,0,-baseThickness]){
    difference(){    
        linear_extrude(baseThickness){
            basePlate();
        }
                
        translate(pushButtonLocation){
            // create a hole for the wires of the LED-ring
            rotate([0,0,-60])
                arcRing(largeRingInnerDiameter,largeRingOuterDiameter, 60, ringSpacerHeight);            
            // create a rectangular hole for the push button
            translate([0,0,baseThickness/2])
                cube([pushButtonWidth, pushButtonDepth, baseThickness], center=true);
        }
        
        translate(dialLocation){
            // create a hole for the dial
            cylinder(d=dialDiameter, h=baseThickness);            
            // create a hole for the wires of the LED-ring
            rotate([0,0,60])
                arcRing(largeRingInnerDiameter,largeRingOuterDiameter, 60, ringSpacerHeight);            
        }
    }
}

// place a ring around the LED-rings
translate(dialLocation)
    ring(largeRingOuterDiameter, largeRingOuterDiameter+ringSpacerThickness, ringSpacerHeight);

translate(pushButtonLocation)
    ring(largeRingOuterDiameter, largeRingOuterDiameter+ringSpacerThickness, ringSpacerHeight);


// module to creste a ring
module ring(innerDiameter, outerDiameter, height){
    
    difference(){
        cylinder(d=outerDiameter, h=height);
        cylinder(d=innerDiameter, h=height);
    }
}

// module to create an arc with a width
module arcRing(innerDiameter, outerDiameter, rotationAngle, height){
    ringWidth = outerDiameter/2-innerDiameter/2;
    rotate_extrude(angle = rotationAngle){
        translate([innerDiameter/2,0])
            square([ringWidth, height], center=false);
        
    }
}

// module to create the base plate
module basePlate(){
    union(){
        translate(dialLocation)
            circle(d = dialAreaDiameter);
        translate(pushButtonLocation)
            circle(d = pushButtonAreaDiameter);

        tanShape(pushButtonLocation, dialLocation, pushButtonAreaDiameter, dialAreaDiameter, bottomCurveDiameter, 0, true);
        tanShape(pushButtonLocation, dialLocation, pushButtonAreaDiameter, dialAreaDiameter, topCurveDiameter, 0, false);
    }
}