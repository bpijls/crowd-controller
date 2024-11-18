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

difference(){
    filletCylinder(dialDiameter, dialHeight, topFilletRadius, [true, false]);

    for (i = [0:nDimples-1])
    {
        rotate([0,0,i*360/nDimples])    
            translate([dialDiameter/2+dimpleDistance,0,dialHeight/2])        
                cylinder(r=dimpleRadius, h=dialHeight, center=true);
    }
    
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


module filletCylinder(diameter, height, filletRadius, filletTopBottom){
    radius = diameter/2;
    rotate_extrude()
    {
    hull()
    {
        square([radius-filletRadius, height], center = false);

        translate([radius-filletRadius/2,filletRadius/2,0])
            if (filletTopBottom[1])
                circle(r=filletRadius/2, center = true);
                else
                    square(size=filletRadius, center = true);
                   

        translate([radius-filletRadius/2,height-filletRadius/2,0])
            if (filletTopBottom[0])
                circle(r=filletRadius/2);
                else                    
                    square(size=filletRadius, center = true);
    }
    }   
}