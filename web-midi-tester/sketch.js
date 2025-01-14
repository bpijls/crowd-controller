// p5.js sketch to visualize MIDI input by creating circles
// Circles grow larger and brighter based on MIDI control values

let nControls = 16; // Number of MIDI controls
let circles = []; // Array to store circle data
class Circle {
    constructor(x, y, size, brightness, index) {
        this.x = x;
        this.y = y;
        this.size = size;
        this.index = index;
        this.color = color(map(this.index, 0, 16, 0, 255),255,255);
    }

    draw() {
        fill(this.color);
        noStroke();
        ellipse(this.x, this.y, this.size);
        textSize(20);        
        textAlign(CENTER, CENTER);
        fill(255);
        text(this.index, this.x, this.y);
    }
}

function setup() {
    createCanvas(600, 600);
    colorMode(HSB, 255);
    
    initButtons();

        
    // add 8 circles to the array and position them in a circle
    for (let i = 0; i < nControls; i++) {
        let angle = TWO_PI / nControls * i;
        let x = width / 2 + cos(angle) * 200;
        let y = height / 2 + sin(angle) * 200;
        let size = 50;
        let brightness = 255;
        circles.push(new Circle(x, y, size, brightness, i));
    }

}

function draw() {
    background(0);

    // Draw all circles
    circles.forEach(circle => {
        circle.draw();
    });
    
}

function handleControlChange(e) {
    const control = e.controller.number;
    const value = e.value;
    //console.log("Control change:", control, value);
    // Map MIDI control value to size and brightness
    const size = map(value, 0, 1, 10, 200);
    const brightness = map(value, 0, 1, 50, 255);

    // Update circle data
    circles[control].index = control;
    circles[control].size = size;
    circles[control].brightness = brightness;
}


function initButtons(){

    // create a midi connect button
    let midiButton = createButton('Connect MIDI');
    midiButton.position(10, 10);
    midiButton.mousePressed(() => {
        WebMidi.enable(err => {
            if (err) {
                console.error("WebMidi could not be enabled.", err);
            } else {
                console.log("WebMidi enabled!");

                // Get all connected MIDI input devices
                WebMidi.inputs.forEach(input => {
                    console.log("Detected input device:", input.name);
                    input.addListener("controlchange", "all", handleControlChange);
                });
            }
        });
    });

    // create a midi disconnect button
    let midiDisconnectButton = createButton('Disconnect MIDI');
    midiDisconnectButton.position(10, 40);
    midiDisconnectButton.mousePressed(() => {
        WebMidi.disable();
        console.log("WebMidi disabled!");
    });

}