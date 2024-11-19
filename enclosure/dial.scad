$fn=50;
dialDiameter = 62;
dialHeight = 15;
dialInnerDiameter = 53;
shaftOuterDiameterUpper = 12;
shaftOuterDiameterLower = 8;
shaftDiameter = 6.2;
shaftFlatSide = 4.6;

topFilletRadius = 8;

nDimples = 8;
dimpleRadius = 12;
dimpleDistance = 3*dimpleRadius/4;

// Base shape for the dial
difference(){
    // Create a filleted cylinder for the dial
    filletCylinder(dialDiameter, dialHeight, topFilletRadius, [true, false]);

    // Use a polar array of cylinders to subtract dimples from the dial
    for (i = [0:nDimples-1])
    {
        rotate([0,0,i*360/nDimples])    
            translate([dialDiameter/2+dimpleDistance,0,dialHeight/2])        
                cylinder(r=dimpleRadius, h=dialHeight, center=true);
    }
    
    // Subtract the inner dial cylinder to make it hollow
    filletCylinder(dialInnerDiameter, dialHeight-2, topFilletRadius, [true, false]);
}

difference(){
    cylinder(d2=shaftOuterDiameterUpper, d1=shaftOuterDiameterLower, h=dialHeight);

    difference(){
        cylinder(d=shaftDiameter, h=dialHeight-3);
            translate([-shaftDiameter/2+shaftFlatSide, -shaftDiameter/2, 0])
                cube([shaftDiameter, shaftDiameter, dialHeight-3], center=false);
    }
}

// Function to create a filleted cylinder
// diameter: diameter of the cylinder
// height: height of the cylinder
// filletRadius: radius of the fillet
// filletTopBottom: array of two booleans, whether to fillet the top and bottom of the cylinder
module filletCylinder(diameter, height, filletRadius, filletTopBottom){
    radius = diameter/2;

    // To create the cylinder, a flat shape is revolved around the z-axis
    rotate_extrude()
    {
        hull()
        {
            // The space between the axis and the fillets
            square([radius-filletRadius, height], center = false);

            // Either a circle or a square is used to create the fillet
            translate([radius-filletRadius/2,filletRadius/2,0])
                // if the fillet for the bottom is enabled, create a circle
                if (filletTopBottom[1])
                    circle(r=filletRadius/2, center = true);
                    else // otherwise, create a square
                        square(size=filletRadius, center = true);
                    
            // Same for the top fillet
            translate([radius-filletRadius/2,height-filletRadius/2,0])
                if (filletTopBottom[0])
                    circle(r=filletRadius/2);
                    else                    
                        square(size=filletRadius, center = true);
        }
    }   
}